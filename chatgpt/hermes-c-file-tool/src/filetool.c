#define _XOPEN_SOURCE 700
#include "filetool.h"
#include <dirent.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

static const char *kind(mode_t m) {
    if (S_ISREG(m)) return "file";
    if (S_ISDIR(m)) return "directory";
    if (S_ISLNK(m)) return "symlink";
    if (S_ISCHR(m)) return "char_device";
    if (S_ISBLK(m)) return "block_device";
    if (S_ISFIFO(m)) return "fifo";
    if (S_ISSOCK(m)) return "socket";
    return "other";
}
static int stat_path(const char *path, struct stat *st) {
    if (lstat(path, st) != 0) { fprintf(stderr, "filetool: cannot stat '%s': %s\n", path, strerror(errno)); return 1; }
    return 0;
}
int command_info(const char *path) {
    struct stat st; char t[64] = "unknown";
    if (stat_path(path, &st)) return 1;
    struct tm tmv;
    if (localtime_r(&st.st_mtime, &tmv)) strftime(t, sizeof t, "%Y-%m-%dT%H:%M:%S%z", &tmv);
    printf("path: %s\ntype: %s\nsize_bytes: %" PRIuMAX "\nmode: %04o\nuid: %ju\ngid: %ju\nmodified: %s\n", path, kind(st.st_mode), (uintmax_t)st.st_size, (unsigned)(st.st_mode & 07777), (uintmax_t)st.st_uid, (uintmax_t)st.st_gid, t);
    return 0;
}
int command_list(const char *path) {
    DIR *d = opendir(path); if (!d) { fprintf(stderr, "filetool: cannot open directory '%s': %s\n", path, strerror(errno)); return 1; }
    struct dirent *e; errno = 0;
    while ((e = readdir(d)) != NULL) { if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue; puts(e->d_name); }
    int rc = errno ? 1 : 0; if (rc) fprintf(stderr, "filetool: directory read failed: %s\n", strerror(errno));
    closedir(d); return rc;
}
static int find_walk(const char *root, const char *name, unsigned depth) {
    if (depth > 64) return 0;
    DIR *d = opendir(root); if (!d) return 0;
    struct dirent *e; int found = 0;
    while ((e = readdir(d)) != NULL) {
        if (!strcmp(e->d_name, ".") || !strcmp(e->d_name, "..")) continue;
        char path[PATH_MAX]; int n = snprintf(path, sizeof path, "%s/%s", root, e->d_name);
        if (n < 0 || (size_t)n >= sizeof path) continue;
        if (strcmp(e->d_name, name) == 0) { puts(path); found++; }
        struct stat st; if (lstat(path, &st) == 0 && S_ISDIR(st.st_mode) && !S_ISLNK(st.st_mode)) found += find_walk(path, name, depth + 1);
    }
    closedir(d); return found;
}
int command_find(const char *root, const char *name) {
    struct stat st; if (stat_path(root, &st)) return 1;
    if (!S_ISDIR(st.st_mode)) { fprintf(stderr, "filetool: search root is not a directory: %s\n", root); return 2; }
    int n = find_walk(root, name, 0); if (!n) { fprintf(stderr, "filetool: no matching entry named '%s' under '%s' (or some directories were unreadable)\n", name, root); return 3; }
    return 0;
}
int command_search(const char *path, const char *needle) {
    FILE *f = fopen(path, "rb"); if (!f) { fprintf(stderr, "filetool: cannot open '%s': %s\n", path, strerror(errno)); return 1; }
    char *line = NULL; size_t cap = 0; ssize_t len; uintmax_t num = 0; int hits = 0;
    while ((len = getline(&line, &cap, f)) >= 0) { (void)len; num++; if (strstr(line, needle)) { printf("%" PRIuMAX ":%s", num, line); if (!len || line[len-1] != '\n') putchar('\n'); hits++; } }
    int rc = ferror(f) ? 1 : (hits ? 0 : 3);
    if (ferror(f)) fprintf(stderr, "filetool: read error for '%s'\n", path);
    else if (!hits) fprintf(stderr, "filetool: no matches found\n");
    free(line); fclose(f); return rc;
}
/* FNV-1a 64-bit checksum for quick integrity comparisons; not cryptographic. */
int command_hash(const char *path) {
    FILE *f = fopen(path, "rb"); if (!f) { fprintf(stderr, "filetool: cannot open '%s': %s\n", path, strerror(errno)); return 1; }
    uint64_t h = UINT64_C(14695981039346656037); unsigned char buf[16384]; size_t n;
    while ((n = fread(buf, 1, sizeof buf, f)) > 0) for (size_t i=0; i<n; i++) { h ^= buf[i]; h *= UINT64_C(1099511628211); }
    if (ferror(f)) { fprintf(stderr, "filetool: read error for '%s'\n", path); fclose(f); return 1; }
    fclose(f); printf("algorithm: FNV-1a-64 (non-cryptographic)\nsize_bytes: ");
    struct stat st; if (stat_path(path, &st)) return 1;
    printf("%" PRIuMAX "\nhash: %016" PRIx64 "\n", (uintmax_t)st.st_size, h); return 0;
}
