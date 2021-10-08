#include "pipe.h"
#include "header.h"
#include "execute.h"

int count_pipes(char *command)
{
    int len = strlen(command);
    int num_pipes = 0;
    for (int i = 0; i < len; i++)
    {
        if (command[i] == '|')
            num_pipes++;
    }
    return num_pipes;
}
void free_pointer(int arg_count, char *arguments[])
{
    for (int i = 0; i < arg_count; i++)
        if (arguments[i] != NULL)
            free(arguments[i]);
}

void execute_piping(char *command)
{
    if (command == NULL)
        return;
    int num_pipes = count_pipes(command);
    int num_pipe_commands = num_pipes + 1;
    int stdin_copy = dup(STDIN_FILENO);
    if (stdin_copy < 0)
    {
        perror(ERROR "could not redirect input ");
        return;
    }
    int stdout_copy = dup(STDOUT_FILENO);
    if (stdout_copy < 0)
    {
        perror(ERROR "could not redirect output ");
        close(stdin_copy);
        return;
    }
    char *pipe_arguments[num_pipe_commands + 1];
    int arg_count = 0;
    char *str = strtok(command, "|");
    while (str != NULL)
    {
        pipe_arguments[arg_count] = (char *)malloc(sizeof(char) * ((strlen(str) + 1)));
        strcpy(pipe_arguments[arg_count], str);
        str = strtok(NULL, "|");
        arg_count++;
    }
    if (arg_count != num_pipe_commands)
    {
        fprintf(stderr, ERROR "Invalid syntax near '|' \n" RESET);
        free_pointer(arg_count, pipe_arguments);
        close(stdin_copy);
        close(stdout_copy);
        return;
    }
    pipe_arguments[arg_count] = NULL;
    int proc_pipe[num_pipes][2];
    // change the stdinfileno and stdoutfileno
    int flag = pipe(proc_pipe[0]);
    if (flag < 0)
    {
        perror(ERROR "COULDNOT PIPE THE COMMANDS :");
        fprintf(stderr, "\nEXITING...\n");
        free_pointer(arg_count, pipe_arguments);
        close(stdin_copy);
        close(stdout_copy);
        return;
    }
    // 1 -> read end
    // 0 write end
    // for first proc copy pipe[1] to stdout
    int flag2 = dup2(proc_pipe[0][1], STDOUT_FILENO);
    if (flag2 < 0)
    {
        perror(ERROR "COULDNOT PIPE THE COMMANDS :");
        fprintf(stderr, "\nEXITING...\n");
        close(proc_pipe[0][1]);
        close(proc_pipe[0][0]);
        free_pointer(arg_count, pipe_arguments);
        close(stdin_copy);
        close(stdout_copy);
        return;
    }
    execute(pipe_arguments[0]);
    close(proc_pipe[0][1]);
    int itr = 1;
    for (; itr < arg_count - 1 && pipe_arguments[itr] != NULL; itr++)
    {
        // intitate the next pipe
        int flag_pipe = pipe(proc_pipe[itr]);
        if (flag_pipe < 0)
        {
            perror(ERROR "COULDNOT PIPE THE COMMANDS :");
            fprintf(stderr, "\nEXITING...\n");
            execute("exit");
        }
        // pipe established
        // duplicate last proc ka write end to stdin
        int flag_dup = dup2(proc_pipe[itr - 1][0], STDIN_FILENO);
        if (flag_dup < 0)
        {
            perror(ERROR "COULDNOT PIPE THE COMMANDS :");
            fprintf(stderr, "\nEXITING...\n");
            execute("exit");
        }
        int flag_dup2 = dup2(proc_pipe[itr][1], STDOUT_FILENO);
        if (flag_dup2 < 0)
        {
            perror(ERROR "COULDNOT PIPE THE COMMANDS :");
            fprintf(stderr, "\nEXITING...\n");
            execute("exit");
        }
        execute(pipe_arguments[itr]);
        close(proc_pipe[itr - 1][0]);
        close(proc_pipe[itr][1]);
    }

    int flag_dup = dup2(proc_pipe[itr - 1][0], STDIN_FILENO);
    if (flag_dup < 0)
    {
        perror(ERROR "COULDNOT PIPE THE COMMANDS :");
        fprintf(stderr, "\nEXITING...\n");
        execute("exit");
    }
    int flag_dup2 = dup2(stdout_copy, STDOUT_FILENO);
    if (flag_dup2 < 0)
    {
        perror(ERROR "COULDNOT PIPE THE COMMANDS :");
        fprintf(stderr, "\nEXITING...\n");
        execute("exit");
    }
    is_pipe = false;
    execute(pipe_arguments[itr]);

    int fg = dup2(stdin_copy, STDIN_FILENO);
    if (fg < 0)
    {
        perror(ERROR "COULDNOT PIPE THE COMMANDS :");
        fprintf(stderr, "\nEXITING...\n");
        execute("exit");
    }

    close(proc_pipe[itr - 1][0]);
    close(stdin_copy);
    close(stdout_copy);
    free_pointer(arg_count, pipe_arguments);
}
/*
void execute_piping(char *command)
{
    if (command == NULL)
        return;
    int num_pipes = count_pipes(command);
    int num_pipe_commands = num_pipes + 1;
    int stdin_copy = dup(STDIN_FILENO);
    if (stdin_copy < 0)
    {
        perror(ERROR "could not redirect input ");
        return;
    }
    int stdout_copy = dup(STDOUT_FILENO);
    if (stdout_copy < 0)
    {
        perror(ERROR "could not redirect output ");
        close(stdin_copy);
        return;
    }
    char *pipe_arguments[num_pipe_commands + 1];
    int arg_count = 0;
    char *str = strtok(command, "|");
    while (str != NULL)
    {
        pipe_arguments[arg_count] = (char *)malloc(sizeof(char) * ((strlen(str) + 1)));
        strcpy(pipe_arguments[arg_count], str);
        str = strtok(NULL, "|");
        arg_count++;
    }
    if (arg_count != num_pipe_commands)
    {
        fprintf(stderr, ERROR "Invalid syntax near '|' \n" RESET);
        free_pointer(arg_count, pipe_arguments);
        close(stdin_copy);
        close(stdout_copy);
        return;
    }
    pipe_arguments[arg_count] = NULL;
    for (int i = 0; i < arg_count - 1; i++)
    {
        int pipe_fd[2];
        if (pipe(pipe_fd) < 0)
        {
            perror("Error in constructing pipe");
            break;
        }
        dup2(pipe_fd[1], 1);
        execute(pipe_arguments[i]);
        dup2(pipe_fd[0], 0);
        close(pipe_fd[1]);
    }
    dup2(stdout_copy, STDOUT_FILENO);
    execute(pipe_arguments[arg_count - 1]);
    dup2(stdin_copy, STDIN_FILENO);
    free_pointer(arg_count, pipe_arguments);
}*/