/*
 * c-calc — Pure C arithmetic expression evaluator
 * Supports: + - * / % ^ ( ) and floating-point numbers
 * No external libraries. Compile: gcc -O2 -o c-calc c-calc.c -lm
 *
 * Usage:
 *   c-calc "1 + 2 * (3 - 4.5)"
 *   echo "2^10" | c-calc
 *   c-calc -h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#define MAX_EXPR 4096

typedef struct {
    const char *s;
    size_t pos;
    int error;
    char errmsg[128];
} Parser;

static void set_error(Parser *p, const char *msg) {
    if (!p->error) {
        p->error = 1;
        snprintf(p->errmsg, sizeof(p->errmsg), "%s (at position %zu)", msg, p->pos);
    }
}

static void skip_ws(Parser *p) {
    while (p->s[p->pos] && isspace((unsigned char)p->s[p->pos]))
        p->pos++;
}

static double parse_expr(Parser *p);   /* forward */

static double parse_number(Parser *p) {
    skip_ws(p);
    char *end;
    errno = 0;
    double v = strtod(p->s + p->pos, &end);
    if (end == p->s + p->pos) {
        set_error(p, "expected number");
        return 0;
    }
    if (errno == ERANGE) {
        set_error(p, "number out of range");
        return 0;
    }
    p->pos = (size_t)(end - p->s);
    return v;
}

static double parse_primary(Parser *p) {
    skip_ws(p);
    if (p->s[p->pos] == '(') {
        p->pos++;
        double v = parse_expr(p);
        skip_ws(p);
        if (p->s[p->pos] != ')') {
            set_error(p, "expected ')'");
            return 0;
        }
        p->pos++;
        return v;
    }
    if (p->s[p->pos] == '+') {
        p->pos++;
        return parse_primary(p);
    }
    if (p->s[p->pos] == '-') {
        p->pos++;
        return -parse_primary(p);
    }
    return parse_number(p);
}

/* Power is right-associative: 2^3^2 = 2^(3^2) = 512 */
static double parse_power(Parser *p) {
    double left = parse_primary(p);
    skip_ws(p);
    if (p->s[p->pos] == '^') {
        p->pos++;
        double right = parse_power(p);
        if (p->error) return 0;
        return pow(left, right);
    }
    return left;
}

static double parse_term(Parser *p) {
    double left = parse_power(p);
    for (;;) {
        skip_ws(p);
        char op = p->s[p->pos];
        if (op != '*' && op != '/' && op != '%') break;
        p->pos++;
        double right = parse_power(p);
        if (p->error) return 0;
        if (op == '*') left *= right;
        else if (op == '/') {
            if (right == 0.0) { set_error(p, "division by zero"); return 0; }
            left /= right;
        } else { /* % */
            if (right == 0.0) { set_error(p, "modulo by zero"); return 0; }
            left = fmod(left, right);
        }
    }
    return left;
}

static double parse_expr(Parser *p) {
    double left = parse_term(p);
    for (;;) {
        skip_ws(p);
        char op = p->s[p->pos];
        if (op != '+' && op != '-') break;
        p->pos++;
        double right = parse_term(p);
        if (p->error) return 0;
        if (op == '+') left += right;
        else left -= right;
    }
    return left;
}

static double evaluate(const char *expr, char *errbuf, size_t errlen) {
    Parser p = { .s = expr, .pos = 0, .error = 0 };
    p.errmsg[0] = '\0';
    double result = parse_expr(&p);
    skip_ws(&p);
    if (!p.error && p.s[p.pos] != '\0') {
        set_error(&p, "unexpected trailing characters");
    }
    if (p.error) {
        snprintf(errbuf, errlen, "%s", p.errmsg);
        return 0;
    }
    errbuf[0] = '\0';
    return result;
}

static void usage(const char *prog) {
    fprintf(stderr,
        "c-calc — pure C arithmetic expression evaluator\n"
        "Usage:\n"
        "  %s \"expression\"\n"
        "  echo \"expression\" | %s\n"
        "  %s -h | --help\n\n"
        "Operators (precedence high → low):\n"
        "  ^          power (right-associative)\n"
        "  * / %%      multiply, divide, modulo\n"
        "  + -        add, subtract\n"
        "  ( )        grouping\n"
        "  unary + -  signs\n\n"
        "Examples:\n"
        "  %s \"2 + 3 * 4\"\n"
        "  %s \"(1+2)^3\"\n"
        "  %s \"2^3^2\"          # 512\n"
        "  %s \"3.14159 * 2\"\n",
        prog, prog, prog, prog, prog, prog, prog);
}

int main(int argc, char **argv) {
    char expr[MAX_EXPR];
    expr[0] = '\0';

    if (argc >= 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            usage(argv[0]);
            return 0;
        }
        /* join all args into one expression (allows unquoted multi-word) */
        size_t off = 0;
        for (int i = 1; i < argc && off < MAX_EXPR - 1; i++) {
            size_t len = strlen(argv[i]);
            if (off + len + 1 >= MAX_EXPR) break;
            if (off > 0) expr[off++] = ' ';
            memcpy(expr + off, argv[i], len);
            off += len;
        }
        expr[off] = '\0';
    } else {
        /* read from stdin */
        if (!fgets(expr, sizeof(expr), stdin)) {
            fprintf(stderr, "error: no expression provided\n");
            return 1;
        }
        /* strip trailing newline */
        size_t n = strlen(expr);
        if (n > 0 && expr[n-1] == '\n') expr[n-1] = '\0';
    }

    if (expr[0] == '\0') {
        fprintf(stderr, "error: empty expression\n");
        return 1;
    }

    char err[128];
    double result = evaluate(expr, err, sizeof(err));
    if (err[0]) {
        fprintf(stderr, "error: %s\n", err);
        return 1;
    }

    /* print nicely: integer if whole number, else with reasonable precision */
    if (result == (long long)result && fabs(result) < 1e15)
        printf("%lld\n", (long long)result);
    else
        printf("%.15g\n", result);

    return 0;
}