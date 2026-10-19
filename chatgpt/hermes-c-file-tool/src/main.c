#include "filetool.h"
#include <stdio.h>
#include <string.h>
static void usage(FILE *f) {
    fprintf(f, "Usage:\n  c-file-tool info PATH\n  c-file-tool list DIRECTORY\n  c-file-tool find DIRECTORY BASENAME\n  c-file-tool search FILE TEXT\n  c-file-tool hash FILE\n  c-file-tool --help\n\nRead-only file inspection utility. Exit codes: 0=success, 1=I/O error, 2=invalid argument/type, 3=no matches.\n");
}
int main(int argc, char **argv) {
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))) { usage(stdout); return 0; }
    if (argc < 2) { usage(stderr); return 2; }
    if (!strcmp(argv[1], "info") && argc == 3) return command_info(argv[2]);
    if (!strcmp(argv[1], "list") && argc == 3) return command_list(argv[2]);
    if (!strcmp(argv[1], "find") && argc == 4) return command_find(argv[2], argv[3]);
    if (!strcmp(argv[1], "search") && argc == 4) return command_search(argv[2], argv[3]);
    if (!strcmp(argv[1], "hash") && argc == 3) return command_hash(argv[2]);
    usage(stderr); return 2;
}
