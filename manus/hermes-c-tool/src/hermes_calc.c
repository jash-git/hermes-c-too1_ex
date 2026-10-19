#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

static void print_error(const char *message) {
    printf("{\"ok\":false,\"error\":\"%s\"}\n", message);
}

static int append_number(double **values, size_t *count, size_t *capacity, double value) {
    if (*count == *capacity) {
        size_t next = (*capacity) * 2;
        double *grown = (double *)realloc(*values, next * sizeof(double));
        if (grown == NULL) return 0;
        *values = grown;
        *capacity = next;
    }
    (*values)[(*count)++] = value;
    return 1;
}

static int parse_token(const char *token, double *out) {
    char *end = NULL;
    double value;
    errno = 0;
    value = strtod(token, &end);
    if (token == end || *end != '\0' || errno == ERANGE || !isfinite(value)) return 0;
    *out = value;
    return 1;
}

static int read_stdin(double **values, size_t *count, size_t *capacity) {
    char token[256];
    while (scanf("%255s", token) == 1) {
        double value;
        if (!parse_token(token, &value)) return 0;
        if (!append_number(values, count, capacity, value)) return 0;
    }
    return ferror(stdin) == 0;
}

static void print_number(double value) {
    printf("%.17g", value);
}

int main(int argc, char **argv) {
    const char *operation;
    double *values = NULL;
    size_t count = 0, capacity = INITIAL_CAPACITY, i;
    double result = 0.0;

    if (argc < 2) {
        print_error("missing operation");
        return 2;
    }
    operation = argv[1];
    values = (double *)malloc(capacity * sizeof(double));
    if (values == NULL) {
        print_error("out of memory");
        return 3;
    }

    for (i = 2; i < (size_t)argc; ++i) {
        double value;
        if (!parse_token(argv[i], &value) || !append_number(&values, &count, &capacity, value)) {
            free(values);
            print_error("invalid number");
            return 2;
        }
    }
    if (count == 0 && !read_stdin(&values, &count, &capacity)) {
        free(values);
        print_error("invalid number in stdin");
        return 2;
    }
    if (count == 0) {
        free(values);
        print_error("no numbers supplied");
        return 2;
    }

    if (strcmp(operation, "sum") == 0 || strcmp(operation, "avg") == 0 || strcmp(operation, "average") == 0) {
        for (i = 0; i < count; ++i) result += values[i];
        if (strcmp(operation, "avg") == 0 || strcmp(operation, "average") == 0) result /= (double)count;
    } else if (strcmp(operation, "min") == 0) {
        result = values[0];
        for (i = 1; i < count; ++i) if (values[i] < result) result = values[i];
    } else if (strcmp(operation, "max") == 0) {
        result = values[0];
        for (i = 1; i < count; ++i) if (values[i] > result) result = values[i];
    } else {
        free(values);
        print_error("unsupported operation; use sum, avg, min, or max");
        return 2;
    }

    printf("{\"ok\":true,\"operation\":\"");
    printf("%s", (strcmp(operation, "average") == 0) ? "avg" : operation);
    printf("\",\"count\":%zu,\"result\":", count);
    print_number(result);
    printf("}\n");
    free(values);
    return 0;
}
