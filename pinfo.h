#ifndef SHELL_PINFO
#define SHELL_PINFO
#include "header.h"

typedef struct proc_info
{
    pid_t proc_pid;
    char virtual_mem_size[name_len];
    char exec_path[name_len];
    char proc_status[3];
} proc_info;

void execute_pinfo(int arg_count, char *arguments[]);

#endif