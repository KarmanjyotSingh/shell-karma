#include "bg.h"
#include "header.h"
#include "jobs.h"
void execute_bg(int arg_count, char *argument[])
{
    if (arg_count != 2)
    {
        fprintf(stderr, ERROR "Invalid Number of arguments, correct syntax bg <job_num>\n" RESET);
        return;
    }
    int job_no = atoi(argument[1]);
    if (job_no <= 0)
    {
        fprintf(stderr, ERROR "<job_number> should be a positive integer, syntax : bg <job_num>\n" RESET);
        return;
    }
    bg(job_no);
}
void bg(int job_num)
{
    // process to be runned in the background
    // dont remove from list of processes , just send a SIGCONT signal
    // no I/O permissions need to be changed
    jobs *job = get_data_by_id(job_num);
    if (job == NULL)
    {
        fprintf(stderr, ERROR "No process exists with given <job_number>\n" RESET);
        return;
    }
    print_YELLOW();
    printf("[%d] ", job->job_num);
    print_GREEN();
    printf("%s running in background ", job->proc_name);
    print_YELLOW();
    printf(" [%d]\n", job->proc_pid);
    print_RESET();
    pid_t proc_pid = job->proc_pid;
    int flag = kill(proc_pid, SIGCONT);
    if (flag < 0)
    {
        perror(ERROR "bg ");
        return;
    }
}