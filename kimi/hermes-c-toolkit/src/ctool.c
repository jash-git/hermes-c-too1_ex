/*
 * ctool.c — 純 C 多功能 CLI 工具 (C99, 無外部依賴)
 * 子命令: stats | hash | b64e | b64d | calc | upper | lower | rev
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

static const char *USAGE =
    "ctool - 純 C 多功能工具\n"
    "用法:\n"
    "  ctool stats <file>        統計行數/字數/字元數/位元組數\n"
    "  ctool hash  <file>        FNV-1a 64-bit 雜湊\n"
    "  ctool b64e  <string>      Base64 編碼\n"
    "  ctool b64d  <string>      Base64 解碼\n"
    "  ctool calc  \"<expr>\"    四則運算 (+-*/% 與括號)\n"
    "  ctool upper <string>      轉大寫\n"
    "  ctool lower <string>      轉小寫\n"
    "  ctool rev   <string>      反轉字串\n";

/* ---------------- stats ---------------- */
static int cmd_stats(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "ctool: 無法開啟檔案 %s\n", path); return 1; }
    unsigned long bytes = 0, lines = 0, words = 0;
    int in_word = 0, c;
    while ((c = fgetc(f)) != EOF) {
        bytes++;
        if (c == '\n') lines++;
        if (isspace(c)) in_word = 0;
        else if (!in_word) { words++; in_word = 1; }
    }
    fclose(f);
    printf("file: %s\nlines: %lu\nwords: %lu\nbytes: %lu\n", path, lines, words, bytes);
    return 0;
}

/* ---------------- hash (FNV-1a 64) ---------------- */
static int cmd_hash(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "ctool: 無法開啟檔案 %s\n", path); return 1; }
    uint64_t h = 14695981039346656037ULL;
    int c;
    while ((c = fgetc(f)) != EOF) {
        h ^= (unsigned char)c;
        h *= 1099511628211ULL;
    }
    fclose(f);
    printf("fnv1a64: %016llx\n", (unsigned long long)h);
    return 0;
}

/* ---------------- base64 ---------------- */
static const char B64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int cmd_b64e(const char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i < n; i += 3) {
        unsigned a = (unsigned char)s[i];
        unsigned b = i + 1 < n ? (unsigned char)s[i+1] : 0;
        unsigned c = i + 2 < n ? (unsigned char)s[i+2] : 0;
        putchar(B64[a >> 2]);
        putchar(B64[((a & 3) << 4) | (b >> 4)]);
        putchar(i + 1 < n ? B64[((b & 15) << 2) | (c >> 6)] : '=');
        putchar(i + 2 < n ? B64[c & 63] : '=');
    }
    putchar('\n');
    return 0;
}

static int b64val(int c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

static int cmd_b64d(const char *s) {
    size_t n = strlen(s);
    if (n % 4 != 0) { fprintf(stderr, "ctool: base64 長度須為 4 的倍數\n"); return 1; }
    for (size_t i = 0; i < n; i += 4) {
        int v0 = b64val(s[i]), v1 = b64val(s[i+1]);
        int v2 = s[i+2] == '=' ? -2 : b64val(s[i+2]);
        int v3 = s[i+3] == '=' ? -2 : b64val(s[i+3]);
        if (v0 < 0 || v1 < 0 || v2 == -1 || v3 == -1) {
            fprintf(stderr, "ctool: 無效的 base64 字元\n"); return 1;
        }
        putchar((v0 << 2) | (v1 >> 4));
        if (v2 >= 0) putchar(((v1 & 15) << 4) | (v2 >> 2));
        if (v3 >= 0) putchar(((v2 & 3) << 6) | v3);
    }
    putchar('\n');
    return 0;
}

/* ---------------- calc (recursive descent) ---------------- */
static const char *g_pos;

static void skip_ws(void) { while (*g_pos == ' ' || *g_pos == '\t') g_pos++; }

static double parse_expr(void);

static double parse_num(void) {
    skip_ws();
    if (*g_pos == '(') {
        g_pos++;
        double v = parse_expr();
        skip_ws();
        if (*g_pos != ')') { fprintf(stderr, "ctool: 缺少右括號\n"); exit(1); }
        g_pos++;
        return v;
    }
    if (*g_pos == '-') { g_pos++; return -parse_num(); }
    if (*g_pos == '+') { g_pos++; return parse_num(); }
    char *end;
    double v = strtod(g_pos, &end);
    if (end == g_pos) { fprintf(stderr, "ctool: 無法解析數字 at '%s'\n", g_pos); exit(1); }
    g_pos = end;
    return v;
}

static double parse_term(void) {
    double v = parse_num();
    for (;;) {
        skip_ws();
        if (*g_pos == '*') { g_pos++; v *= parse_num(); }
        else if (*g_pos == '/') { g_pos++; double d = parse_num();
            if (d == 0) { fprintf(stderr, "ctool: 除以零\n"); exit(1); }
            v /= d; }
        else if (*g_pos == '%') { g_pos++; long l = (long)parse_num();
            if (l == 0) { fprintf(stderr, "ctool: 除以零\n"); exit(1); }
            v = (double)((long)v % l); }
        else return v;
    }
}

static double parse_expr(void) {
    double v = parse_term();
    for (;;) {
        skip_ws();
        if (*g_pos == '+') { g_pos++; v += parse_term(); }
        else if (*g_pos == '-') { g_pos++; v -= parse_term(); }
        else return v;
    }
}

static int cmd_calc(const char *expr) {
    g_pos = expr;
    double v = parse_expr();
    skip_ws();
    if (*g_pos != '\0') { fprintf(stderr, "ctool: 多餘的字元 at '%s'\n", g_pos); return 1; }
    printf("%g\n", v);
    return 0;
}

/* ---------------- text transforms ---------------- */
static int cmd_upper(const char *s) { for (; *s; s++) putchar(toupper((unsigned char)*s)); putchar('\n'); return 0; }
static int cmd_lower(const char *s) { for (; *s; s++) putchar(tolower((unsigned char)*s)); putchar('\n'); return 0; }
static int cmd_rev(const char *s) { size_t n = strlen(s); while (n > 0) putchar(s[--n]); putchar('\n'); return 0; }

/* ---------------- main ---------------- */
int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "%s", USAGE); return 2; }
    const char *cmd = argv[1];
    if (!strcmp(cmd, "stats") && argc == 3) return cmd_stats(argv[2]);
    if (!strcmp(cmd, "hash")  && argc == 3) return cmd_hash(argv[2]);
    if (!strcmp(cmd, "b64e")  && argc == 3) return cmd_b64e(argv[2]);
    if (!strcmp(cmd, "b64d")  && argc == 3) return cmd_b64d(argv[2]);
    if (!strcmp(cmd, "calc")  && argc == 3) return cmd_calc(argv[2]);
    if (!strcmp(cmd, "upper") && argc == 3) return cmd_upper(argv[2]);
    if (!strcmp(cmd, "lower") && argc == 3) return cmd_lower(argv[2]);
    if (!strcmp(cmd, "rev")   && argc == 3) return cmd_rev(argv[2]);
    fprintf(stderr, "%s", USAGE);
    return 2;
}
