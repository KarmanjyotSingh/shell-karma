#include "header.h"
#include "input.h"
#include "execute.h"

char *getInput()
{
    char *input = NULL;
    unsigned long int sz_input = 0;
    ssize_t r = getline(&input, &sz_input, stdin);
    if (r > 0)
    {
        if (input[strlen(input) - 1] == '\n')
            input[strlen(input) - 1] = '\0';
    }
    else
        execute("exit");
    return input;
}