#include "fg.h"
#include "jobs.h"
#include "header.h"

void execute_fg(int arg_count, char *argument[])
{
    if (arg_count != 2)
    {
        fprintf(stderr, ERROR "Invalid Number of arguments, correct syntax fg <job_num>\n" RESET);
        return;
    }
    int job_no = atoi(argument[1]);
    if (job_no <= 0)
    {
        fprintf(stderr, ERROR "<job_number> should be a positive integer, syntax : fg <job_num>\n" RESET);
        return;
    }
    fg(job_no);
}
void fg(int job_num)
{
    jobs *proc = get_data_by_id(job_num);
    if (proc == NULL)
    {
        fprintf(stderr, ERROR "No process exists with given <job_number>\n" RESET);
        return;
    }
    pid_t proc_pid = proc->proc_pid;
    char proc_name[name_len];
    strcpy(proc_name, proc->proc_name);
    // bring process into foreground by giving apt permissions to it
    //ignore I/O output for shell
    print_GREEN();
    printf("Process %s with pid %d continuing in foreground .... \n", proc_name, proc_pid);
    print_RESET();
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(0, getpgid(proc_pid));
    int x = kill(proc_pid, SIGCONT);
    if (x == -1)
    {
        perror("");
        return;
    }
    int w_st;
    waitpid(proc_pid, &w_st, WUNTRACED);
    fflush(stdout);
    tcsetpgrp(0, getpid());
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    if (WIFSTOPPED(w_st))
    {
        printf("Process %s with pid %d stopped .... \n", proc_name, proc_pid);
    }
    else
        remove_node(proc_pid);
}