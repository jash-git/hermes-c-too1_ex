/* src/filestat.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <time.h>

static void print_usage(const char *prog)
{
    fprintf(stderr,
        "Usage:\n"
        "  %s summary <file>\n"
        "  %s hash <file>\n",
        prog, prog);
}

static int cmd_summary(const char *path)
{
    struct stat st;

    if (stat(path, &st) != 0) {
        fprintf(stderr, "ERROR: cannot stat file: %s\n", path);
        return 2;
    }

    char timebuf[32];
    struct tm *tm = localtime(&st.st_mtime);
    if (tm != NULL) {
        strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm);
    } else {
        strncpy(timebuf, "unknown", sizeof(timebuf) - 1);
        timebuf[sizeof(timebuf) - 1] = '\0';
    }

    printf("path=%s\n", path);
    printf("size_bytes=%lld\n", (long long)st.st_size);
    printf("modified=%s\n", timebuf);
    printf("readable=%s\n", access(path, R_OK) == 0 ? "true" : "false");
    printf("is_regular_file=%s\n", S_ISREG(st.st_mode) ? "true" : "false");

    return 0;
}

static int cmd_hash(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: cannot open file: %s\n", path);
        return 2;
    }

    uint64_t hash = 1469598103934665603ULL;
    unsigned char buffer[65536];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        for (size_t i = 0; i < n; i++) {
            hash ^= buffer[i];
            hash *= 1099511628211ULL;
        }
    }

    if (ferror(fp)) {
        fprintf(stderr, "ERROR: failed while reading file: %s\n", path);
        fclose(fp);
        return 2;
    }

    fclose(fp);

    printf("path=%s\n", path);
    printf("fnv1a64=%016llx\n", (unsigned long long)hash);

    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "summary") == 0) {
        return cmd_summary(argv[2]);
    }

    if (strcmp(argv[1], "hash") == 0) {
        return cmd_hash(argv[2]);
    }

    print_usage(argv[0]);
    return 1;
}