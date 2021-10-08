#include "signals.h"
#include "header.h"
#include "history.h"
#include "prompt.h"
#include "jobs.h"
// /*
// void SIGINT_HANDLER(int signum)
// {
//     pid_t pid = getpid();
//     if (pid < 0)
//     {
//         perror(ERROR);
//         return;
//     }
//     else if (pid != SHELL_PID)
//         raise(SIGINT);
//     else
//     {
//         return;
//         /*
//         write_back_history();
//         printf("\n\n\n");
//         printf(PROMPT "EVERY MEETING HAS A PARTING ;(\nSEE YOU AGAIN SOON !!!!!!!" RESET);
//         printf("\n\n\n");
//         exit(EXIT_FAILURE);
//         */
// }

// void SIGTSTP_HANDLER(int signum)
// {
//     pid_t pid = getpid();
//     if (pid < 0)
//     {
//         perror(ERROR);
//         return;
//     }
//     else if (pid == SHELL_PID)
//         return;
//     else
//         raise(SIGTSTP);
// }

// void SIGCHILD_HANDLER(int signum)
// {
//     int child_status;

//     pid_t child_pid = waitpid(-1, &child_status, WNOHANG);
//     // child_pid == 0 , no child process , -1 for error
//     if (child_pid > 0)
//     {
//         jobs *command_name = get_data_by_pid(child_pid);
//         if (command_name != NULL)
//         {

//             if (WEXITSTATUS(child_status) == 0 && WIFEXITED(child_status))
//             {
//                 fprintf(stderr, PROMPT "\nPROCESS %s WITH PID : %d EXITED NORMALLY \n" RESET, command_name->proc_name, child_pid);
//                 remove_node(child_pid);
//             }
//             else if (WIFSTOPPED(child_status))
//             {
//                 fprintf(stderr, ERROR "\nPROCESS %s WITH PID : %d STOPPED AFTER RECIEVING SIGNAL \n" RESET, command_name->proc_name, child_pid);
//             }
//             else if (WIFSIGNALED(child_status))
//             {
//                 fprintf(stderr, ERROR "\nPROCESS %s WITH PID : %d EXITED AFTER RECIEVING SIGNAL\n" RESET, command_name->proc_name, child_pid);
//                 remove_node(child_pid);
//             }
//             else if (WIFEXITED(child_status))
//             {
//                 fprintf(stderr, ERROR "\nPROCESS %s WITH PID : %d EXITED ABNORMALLY \n" RESET, command_name->proc_name, child_pid);
//                 remove_node(child_pid);
//             }
//         }
//         prompt();
//         fflush(stdout);
//         return;
//     }
// }

void SIGINT_HANDLER(int signum)
{
    pid_t pid = getpid();
    if (pid < 0)
    {
        perror(ERROR);
        return;
    }
    else if (is_fg == -1)
        return;
    else if (pid != SHELL_PID)
    {
        remove_node(pid);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("\n");
        fflush(stdout);
        prompt();
        // write_back_history();
        // printf("\n\n\n");
        // printf(PROMPT "EVERY MEETING HAS A PARTING ;(\nSEE YOU AGAIN SOON !!!!!!!" RESET);
        // printf("\n\n\n");
        // prompt();
        // fflush(stdout);
        // //  exit(EXIT_FAILURE);
    }
}
void SIGCHILD_HANDLER(int signum)
{
    int child_status;

    pid_t child_pid = waitpid(-1, &child_status, WNOHANG | WUNTRACED);
    // child_pid == 0 , no child process , -1 for error
    if (child_pid >= 0)
    {
        // TO-DO store the command names in a list structure.
        jobs *proc = get_data_by_pid(child_pid);
        if (proc != NULL)
        {

            if (WEXITSTATUS(child_status) == 0 && WIFEXITED(child_status))
            {
                fprintf(stderr, PROMPT "\nPROCESS %s WITH PID : %d EXITED NORMALLY \n" RESET, proc->proc_name, child_pid);
                remove_node(proc->proc_pid);
            }
            else if (WIFSTOPPED(child_status))
            {
                fprintf(stderr, ERROR "\nPROCESS %s WITH PID : %d STOPPED AFTER RECIEVING SIGNAL \n" RESET, proc->proc_name, child_pid);
            }
            else if (WIFSIGNALED(child_status))
            {
                fprintf(stderr, ERROR "\nPROCESS %s WITH PID : %d EXITED AFTER RECIEVING SIGNAL \n" RESET, proc->proc_name, child_pid);
                remove_node(proc->proc_pid);
            }
            else
            {
                fprintf(stderr, ERROR "\nPROCESS %s WITH PID : %d EXITED ABNORMALLY \n" RESET, proc->proc_name, child_pid);
                remove_node(proc->proc_pid);
            }
            fflush(stderr);
            prompt();
            fflush(stdout);
        }
    }
    return;
}

void SIGTSTP_HANDLER(int signum)
{
    pid_t pid = getpid();
    if (pid < 0)
    {
        perror(ERROR);
        return;
    }
    if (is_fg == -1)
        return;
    if (pid != SHELL_PID)
    {
        int flag = kill(pid, SIGTSTP);
        if (flag < 0)
        {
            perror(ERROR);
        }
    }
    else
        printf("\n");
}
void sig(int job_no, int sig_num)
{
    jobs *tmp = get_data_by_id(job_no);
    if (tmp == NULL)
    {
        fprintf(stderr, ERROR "NO PROCESS EXISTS WITH GIVEN job id!!!\n" RESET);
        return;
    }
    print_YELLOW();
    printf("signaling proc %s : %d\n", tmp->proc_name, tmp->proc_pid);
    int flag = kill(tmp->proc_pid, sig_num);
    if (flag < 0)
    {
        perror(ERROR "sig ");
    }
    // if (strcmp(tmp->proc_name, "vi") == 0)
    // {
    //     printf("%d\n", sig_num);
    //     if (sig_num == SIGINT || sig_num == SIGQUIT || sig_num == SIGABRT || sig_num == SIGKILL || sig_num == SIGTERM)
    //     {

    //         remove_node(tmp->proc_pid);
    //     }
    // }
}
void execute_sig(int arg_count, char *argument[])
{
    if (arg_count != 3)
    {
        fprintf(stderr, ERROR "INVALID NUMBER OF ARGUMENTS , syntax - sig <job_number> <signal_number>\n" RESET);
        return;
    }
    int job_no = atoi(argument[1]);
    if (job_no <= 0)
    {
        fprintf(stderr, ERROR "ENTER A POSITIVE INTEGER FOR JOB NUMBER , syntax - sig <job_number> <signal_number>\n" RESET);
        return;
    }
    int sig_no = atoi(argument[2]);
    if (sig_no <= 0)
    {
        fprintf(stderr, ERROR "ENTER A POSITIVE INTEGER FOR SIGNAL NUMBER , syntax - sig <job_number> <signal_number>\n" RESET);
        return;
    }
    sig(job_no, sig_no);
}