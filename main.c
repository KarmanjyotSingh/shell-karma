#include "header.h"
#include "prompt.h"
#include "input.h"
#include "execute.h"
#include "tokenize.h"
#include "history.h"
#include "system_command.h"
#include "cd.h"
#include "signals.h"

int main()
{
    is_fg = -1;
    init_shell();
    //signal();
    init_history();
    signal(SIGCHLD, SIGCHILD_HANDLER);
    signal(SIGINT, SIGINT_HANDLER);
    signal(SIGTSTP, SIGTSTP_HANDLER);
    char *in;
    char **list;
    prompt();
    copy_stdin_fileno = dup(STDIN_FILENO);
    copy_stdout_fileno = dup(STDOUT_FILENO);
    while (1)
    {

        in = getInput();
        char command[name_len];
        strcpy(command, in);
        add_to_history(command);
        list = getList(command);
        list[num_command] = NULL;
        for (int i = 0; list != NULL && list[i] != NULL && i < num_command; i++)
        {
            execute(list[i]);
        }
        free(in);
        free(list);
        prompt();
    }
    execute("exit");
}