#ifndef SHELL_JOBS
#define SHELL_JOBS
#include "header.h"
typedef struct jobs
{
    char proc_name[name_len];
    pid_t proc_pid;
    int job_num;
    struct jobs *next;
} jobs;
int bg_num_proc;
typedef struct job_list
{
    int num_proc;
    jobs *start;
} job_list;
job_list proc_list;
void init_list();
jobs *make_node(char *proc, pid_t pid, int job_num);
void add_node(char *proc_name, pid_t pid);
void remove_node(pid_t pid);
jobs *get_data_by_pid(pid_t pid);
jobs *get_data_by_id(int job_id);
void execute_jobs(int arg_count, char *argument[]);
#endif