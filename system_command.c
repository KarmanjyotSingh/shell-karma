#include "header.h"
#include "system_command.h"
#include <signal.h>
#include "jobs.h"

void execute_sys_command(int arg_count, char *argument[])
{
    bool is_background_proc = false;
    if (arg_count == 0)
    {
        perror(ERROR "Invalid Argument Called, give atleast one command to execute\n");
        return;
    }
    if (arg_count >= 2 && strcmp(argument[arg_count - 1], "&") == 0)
    {
        is_background_proc = true;
        argument[arg_count - 1] = NULL;
        arg_count--;
    }
    else if (arg_count >= 1 && argument[arg_count - 1][strlen(argument[arg_count - 1]) - 1] == '&')
    {
        is_background_proc = true;
        argument[arg_count - 1][strlen(argument[arg_count - 1]) - 1] = '\0';
        argument[arg_count] = NULL;
    }
    // we execute the system commands in foreground mode
    if (is_background_proc == false)
    {
        // not a background process , wait parent till child finish
        // printf(ERROR "%d \n", getpid());
        // should be NULL terminated
        argument[arg_count] = NULL;
        pid_t proc_fork = fork();
        if (proc_fork < 0)
        {
            perror(ERROR "Failed To Execute Command ");
            return;
        }
        else if (proc_fork == 0)
        {
            //  printf(ERROR "CHILD %d \n", getpid());
            // in child
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            setpgid(0, 0);
            // set the child process group id to child ka pid ( creating a new group )
            int exec_flag = execvp(argument[0], argument);
            if (exec_flag < 0)
            {
                perror(ERROR "Failed To Execute Command ");
                fprintf(stderr, RESET);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // IN PARENT PROCESS , control passed on to the child
            // wait for the child to execute using wait();
            dup2(copy_stdin_fileno, 0);
            dup2(copy_stdout_fileno, 1);
            signal(SIGTTIN, SIG_IGN);
            // stop the terminal input for the parent //
            signal(SIGTTOU, SIG_IGN);
            // stop the terminal output to the terminal for the parent, ignore any input/output interrupts from parent //
            int st_wait;
            // set the group id of parent process as the pid of the parent, the child sets it's group as child ka pid
            setpgid(proc_fork, 0);
            // tcsetpgrp  set the terminal display permission to the parent process
            tcsetpgrp(STDIN_FILENO, proc_fork);

            waitpid(proc_fork, &st_wait, WUNTRACED); // wait till the child proc dies of
            // st_wait contains the error code

            pid_t pgid_parent = getpgrp();
            // printf(WHITE "PROCFORK %d : %d \n", proc_fork, pgid_parent);
            // printf(ERROR "PARENT %d \n", getpid());

            tcsetpgrp(STDIN_FILENO, pgid_parent);

            signal(SIGTTIN, SIG_DFL);
            // can continue for taking input
            signal(SIGTTOU, SIG_DFL);
            // can continue for taking output
            // printf(WHITE "PROCESS EXECUTED\n");
            if (WIFSTOPPED(st_wait))
            {
                add_node(argument[0], proc_fork);
                printf("Process %s with pid %d stopped .... \n", argument[0], proc_fork);
            }
        }
    }
    else
    {
        // background process hoga
        // no need for backgrounfd to give permission for read and write from parent
        pid_t proc_fork = fork();
        if (proc_fork < 0)
        {
            perror(ERROR "Failed To Execute Command");
            fprintf(stderr, RESET);
            return;
        }
        else if (proc_fork == 0)
        {
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            setpgid(0, 0);
            int exec_flag = execvp(argument[0], argument);
            if (exec_flag < 0)
            {
                perror(ERROR "Failed To Execute Command");
                fprintf(stderr, RESET);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            // in parent
            // no need to wait
            dup2(copy_stdin_fileno, 0);
            dup2(copy_stdout_fileno, 1);

            char *command = (char *)malloc(sizeof(char) * name_len);
            strcpy(command, argument[0]);
            for (int i = 1; i < arg_count && argument[i] != NULL; i++)
            {
                strcat(command, " ");
                strcat(command, argument[i]);
            }
            add_node(command, proc_fork);
            free(command);
            pid_t pgid_parent = getpgrp();
            setpgid(proc_fork, 0);
            print_WHITE();
            printf("%d\n", proc_fork);
            print_RESET();
            tcsetpgrp(STDIN_FILENO, pgid_parent);
        }
    }
}