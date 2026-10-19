#ifndef FILETOOL_H
#define FILETOOL_H
int command_info(const char *path);
int command_list(const char *path);
int command_find(const char *root, const char *name);
int command_search(const char *path, const char *needle);
int command_hash(const char *path);
#endif
