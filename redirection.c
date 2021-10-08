#include "header.h"
#include "redirection.h"
#include "execute.h"
#include "header.h"
#include "bg.h"
#include "signals.h"
#include "jobs.h"
#include "ls.h"
#include "echo.h"
#include "pwd.h"
#include "pinfo.h"
#include "system_command.h"
#include "cd.h"
#include "redirection.h"
#include "history.h"

int check_redirection(char *comm)
{
    char *input_redirection = strstr(comm, "<");
    char *output_redirection = strstr(comm, ">");
    return (input_redirection != NULL || output_redirection != NULL);
}
void free_ptrs(int arg_count, char *arguments[])
{
    for (int i = 0; i < arg_count; i++)
        if (arguments[i] != NULL)
            free(arguments[i]);
}
void execute_redirection(char *comm)
{
    // flag for not printing color codes into the file
    bool in = false, in_file = false;
    // flag for checking the input '<' redirection
    bool out = false, out_file = false;
    // flag for checking the output '>' or '>>' redirection
    bool append = false;
    // for checking the append redirection '>>'
    char input_file[name_len];
    // store the input file ka path name
    int fd_in; // file descriptor for input file
    char output_file[name_len];
    // path for output file
    int fd_out; // file descriptor for output file
    input_file[0] = '\0', output_file[0] = '\0';
    size_t arg_len = strlen(comm) + 1;
    char *str = strtok(comm, " ");
    if (str == NULL)
        return;
    char *arguments[arg_len];
    int arg_count = 0;
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
    while (str != NULL)
    {
        if (strcmp(str, "<") == 0)
        {
            // input redirection
            if (in || out) // in case of consecutive redirection commands , no file provided
            {
                fprintf(stderr, ERROR "shell : invalid redirection syntax unexpected token near <\n" RESET);
                // error to exit the process , exiting .... free the pointers ...
                free_ptrs(arg_count, arguments);
                close(stdin_copy);
                close(stdout_copy);
                return;
            }
            in = true;
            in_file = true;
            str = strtok(NULL, " ");
            continue;
        }
        else if (strcmp(">", str) == 0)
        {
            // output redirection
            if (in || out)
            {
                fprintf(stderr, ERROR "shell : invalid redirection syntax unexpected token near >\n" RESET);
                // error to exit the process , exiting .... free the pointers ...
                free_ptrs(arg_count, arguments);
                close(stdin_copy);
                close(stdout_copy);
                return;
            }
            if (append) // in case of multiple output redirections , final one is taken to hold
                append = false;
            out = true;
            out_file = true;
            str = strtok(NULL, " ");
            continue;
        }
        else if (strcmp(">>", str) == 0)
        {
            // append output
            if (in || out)
            {
                fprintf(stderr, ERROR "shell : invalid redirection syntax unexpected token near >>\n" RESET);
                // error to exit the process , exiting .... free the pointers ...
                free_ptrs(arg_count, arguments);
                close(stdin_copy);
                close(stdout_copy);
                return;
            }
            out_file = true;
            out = true;
            append = true;
            str = strtok(NULL, " ");
            continue;
        }
        if (in == true)
        {
            strcpy(input_file, str);
            in = false;
            str = strtok(NULL, " ");
            continue;
        }
        else if (out == true)
        {
            strcpy(output_file, str);
            out = false;
            str = strtok(NULL, " ");
            continue;
        }
        arguments[arg_count] = (char *)malloc((strlen(str) + 1) * sizeof(char));
        strcpy(arguments[arg_count], str);
        arg_count++;
        str = strtok(NULL, " ");
    }
    arguments[arg_count] = NULL;
    if (in || out)
    {
        fprintf(stderr, ERROR "shell : invalid redirection syntax\n" RESET);
        // error to exit the process , exiting .... free the pointers ...
        free_ptrs(arg_count, arguments);
        close(stdin_copy);
        close(stdout_copy);
        return;
    }
    if (in_file == true)
    {
        if (input_file[0] == '\0')
        {
            fprintf(stderr, ERROR "shell : no file given for input redirection\n" RESET);
            // error to exit the process , exiting .... free the pointers ...
            free_ptrs(arg_count, arguments);
            close(stdin_copy);
            close(stdout_copy);
            return;
        }
        fd_in = open(input_file, O_RDONLY);
        if (fd_in <= 0)
        {
            perror(ERROR "shell : couldn't redirect, invalid input file\n");
            free_ptrs(arg_count, arguments);
            close(stdin_copy);
            close(stdout_copy);
            return;
        }
        int flag = dup2(fd_in, STDIN_FILENO);
        if (flag < 0)
        {
            perror(ERROR "shell : couldn't redirect, invalid input file\n");
            free_ptrs(arg_count, arguments);
            close(fd_in);
            close(stdin_copy);
            close(stdout_copy);
            return;
        }
    }
    if (out_file == true)
    {
        if (output_file[0] == '\0')
        {
            fprintf(stderr, ERROR "shell : no file given for output redirection\n" RESET);
            // error to exit the process , exiting .... free the pointers ...
            free_ptrs(arg_count, arguments);
            if (input_file)
                close(fd_in);
            int flag = dup2(stdin_copy, STDIN_FILENO); // restore the file descriptor
            if (flag < 0)
            {
                fprintf(stderr, ERROR "I/O error , Exiting shell ...............\n" RESET);
                execute("exit");
                close(stdin_copy);
                close(stdout_copy);
                free_ptrs(arg_count, arguments);
            }
            free_ptrs(arg_count, arguments);
            close(stdin_copy);
            close(stdout_copy);
            return;
        }
        if (append == true)
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        else
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd_out <= 0)
        {
            perror(ERROR "shell : couldn't redirect, invalid output file ");
            if (input_file)
                close(fd_in);
            int flag = dup2(stdin_copy, STDIN_FILENO); // restore the file descriptor
            if (flag < 0)
            {
                fprintf(stderr, ERROR "I/O error , Exiting shell ...............\n" RESET);
                execute("exit");
                close(stdin_copy);
                free_ptrs(arg_count, arguments);
                close(stdout_copy);
            }
            close(stdin_copy);
            close(stdout_copy);
            free_ptrs(arg_count, arguments);
            return;
        }
        int flag = dup2(fd_out, STDOUT_FILENO);
        if (flag < 0)
        {
            perror(ERROR "shell : couldn't redirect, invalid input file ");
            int flag = dup2(stdin_copy, STDIN_FILENO); // restore the file descriptor
            if (input_file)
                close(fd_in);
            if (flag < 0)
            {
                fprintf(stderr, ERROR "I/O error , Exiting shell ...............\n" RESET);
                execute("exit");
                close(stdin_copy);
                close(stdout_copy);
                free_ptrs(arg_count, arguments);
            }
            free_ptrs(arg_count, arguments);
            close(stdin_copy);
            close(stdout_copy);
            return;
        }
    }
    if (strcmp(arguments[0], "ls") == 0)
    {
        execute_ls(arg_count, arguments);
    }
    else if (strcmp("cd", arguments[0]) == 0)
    {
        execute_cd(arg_count, arguments);
    }
    else if (strcmp("echo", arguments[0]) == 0)
    {
        execute_echo(arg_count, arguments);
    }
    else if (strcmp("pwd", arguments[0]) == 0)
    {
        execute_pwd();
    }
    else if (strcmp("history", arguments[0]) == 0)
    {
        history(arg_count, arguments);
    }
    else if (strcmp("pinfo", arguments[0]) == 0)
    {
        execute_pinfo(arg_count, arguments);
    }
    else if (strcmp("fg", arguments[0]) == 0)
    {
        is_fg = 1;
        execute_fg(arg_count, arguments);
        is_fg = -1;
    }
    else if (strcmp("repeat", arguments[0]) == 0)
    {
        if (arg_count == 1)
        {
            fprintf(stderr, ERROR "NO COMMAND PROVIDED TO REPEAT , SYNTAX : repeat <number_of_repetitions> < command_to_be_repeated >\n" RESET);
            return;
        }
        int x = atoi(arguments[1]);
        if (x <= 0)
        {
            fprintf(stderr, ERROR "ENTER A POSITIVE INTEGER FOR repeat TO EXECUTE, SYNTAX : repeat <number_of_repetitions> < command_to_be_repeated >\n" RESET);
            return;
        }
        // else correct flags
        // make a string now with apt flags
        if (arguments[2] == NULL)
        {
            fprintf(stderr, ERROR "INVALID COMMAND , SYNTAX : repeat <number_of_repetitions> < command_to_be_repeated >\n" RESET);
            return;
        }
        char cpy_command[name_len];
        char cpy2[name_len];
        strcpy(cpy_command, arguments[2]);
        for (int i = 3; i < arg_count && arguments[i] != NULL; i++)
        {
            strcat(cpy_command, " ");
            strcat(cpy_command, arguments[i]);
        }
        strcpy(cpy2, cpy_command);
        for (int i = 0; i < x; i++)
        {
            execute(cpy2);
            strcpy(cpy2, cpy_command);
        }
    }
    else if (strcmp("bg", arguments[0]) == 0)
    {
        execute_bg(arg_count, arguments);
    }
    else if (strcmp("exit", arguments[0]) == 0)
    {
        write_back_history();
        printf("\n\n\n");
        fprintf(stderr, PROMPT "EVERY MEETING HAS A PARTING ;(\nSEE YOU AGAIN SOON !!!!!!!" RESET);
        printf("\n\n\n");
        exit(0);
    }
    else if (strcmp("replay", arguments[0]) == 0)
    {
        execute_replay(arg_count, arguments);
    }
    else if (strcmp("jobs", arguments[0]) == 0)
    {
        execute_jobs(arg_count, arguments);
    }
    else if (strcmp("sig", arguments[0]) == 0)
    {
        execute_sig(arg_count, arguments);
    }
    else
    {
        execute_sys_command(arg_count, arguments);
    }

end:
    if (in_file)
    {
        int flag = dup2(stdin_copy, STDIN_FILENO);
        if (flag < 0)
        {
            perror(ERROR);
            fprintf(stderr, ERROR "EXITING ......\n");
            free_ptrs(arg_count, arguments);
            close(stdin_copy);
            close(stdout_copy);
            execute("exit");
        }
        close(fd_in);
    }
    if (out_file)
    {
        int flag = dup2(stdout_copy, STDOUT_FILENO);
        if (flag < 0)
        {
            perror(ERROR);
            fprintf(stderr, ERROR "EXITING ......\n");
            free_ptrs(arg_count, arguments);
            close(stdin_copy);
            close(stdout_copy);
            execute("exit");
        }
        close(fd_out);
    }
    close(stdin_copy);
    close(stdout_copy);
    print_RESET();
}