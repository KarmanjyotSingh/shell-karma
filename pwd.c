#include "header.h"
#include "pwd.h"

void execute_pwd()
{
    char *cwd = (char *)malloc(sizeof(char) * name_len);
    getcwd(cwd, name_len);
    if (cwd != NULL)
    {
        printf("%s\n", cwd);
        free(cwd);
    }
    else
    {
        perror(ERROR " ERROR GETTING CWD ");
        fprintf(stderr, RESET);
    }
}