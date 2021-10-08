#include "header.h"
#include "echo.h"

void execute_echo(int arg_count, char **argument)
{
    int i = 1;
    while (argument[i] != NULL)
    {
        printf("%s", argument[i]);
        i++;
        if (argument[i] != NULL)
            printf(" ");
    }
    printf("\n");
}