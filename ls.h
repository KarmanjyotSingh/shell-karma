#ifndef SHELL_ls
#define SHELL_ls
#include "header.h"

#define max_dir 50

int num_dir_ls;
bool a_flag;
bool l_flag;

bool not_hidden(const struct dirent *s);
void execute_ls(int arg_count, char **argument);
char *get_file_permissions(char *file_path);
void ls(char *file_path);

#endif