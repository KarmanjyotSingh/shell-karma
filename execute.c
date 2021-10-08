#include "header.h"
#include "execute.h"
#include "ls.h"
#include "echo.h"
#include "pwd.h"
#include "replay.h"
#include "pinfo.h"
#include "bg.h"
#include "system_command.h"
#include "cd.h"
#include "pipe.h"
#include "jobs.h"
#include "redirection.h"
#include "history.h"
#include "fg.h"
#include "signals.h"
void execute(char *command)
{
    int len = strlen(command) + 1;
    // in the case of a null command string
    if (command == NULL)
        return;

    if (count_pipes(command) != 0)
    {
        is_pipe = true;
        execute_piping(command);
        is_pipe = false;
        return;
    }
    if (check_redirection(command) == true)
    {
        is_redir = true;
        execute_redirection(command);
        is_redir = false;
        return;
    }
    char *str = strtok(command, " ");
    // printf("command is : %s \nstr is : %s\n", command, str);
    //str contains the main command now
    // note : strtok does allocates any memory to str, only a part of str, donot free str
    if (str == NULL) // in case there exist no main command
        return;
    int arg_count = 0;
    // char **arguments = (char **)malloc(len * sizeof(char *));
    char *arguments[len];

    while (str != NULL)
    {
        arguments[arg_count] = (char *)malloc((strlen(str) + 1) * sizeof(char));
        strcpy(arguments[arg_count], str);
        arg_count++;
        str = strtok(NULL, " ");
    }
    arguments[arg_count] = NULL;
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
    for (int i = 0; i < arg_count; i++)
        if (arguments[i] != NULL)
            free(arguments[i]);
}