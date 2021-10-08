#include "header.h"
#include "helper.h"

char *remove_char_front_and_back(char *input, char ch)
{
    char *ret = (char *)malloc(sizeof(input));
    int i = 0, len = strlen(input);

    while (input[i] == ch)
        i++;

    strcpy(ret, input);
    ret[len] = '\0';
    for (int itr = strlen(ret) - 1; itr >= 0; itr--)
    {
        if (ret[itr] == ch)
            ret[itr] = '\0';
    }

    return ret;
}
char *get_file_name_from_path(char *file_path)
{
    if (file_path == NULL) // if an invalid file path is provided
        return NULL;
    char *x = (char *)malloc((strlen(file_path) + 1) * sizeof(char));
    strcpy(x, file_path);
    char *cpy = strstr(x, "/");
    if (cpy == NULL) // file name itself provided ==> it is in the current working directory
        return x;
    else
    {
        int lft = 0;
        do
        {
            lft = strlen(cpy) + 1;
            // printf("lft is : %d\n", lft); //
            x = &x[strlen(x) - lft + 2]; // rearranging the pointer to the string (pointer de-referencing) //
            // printf("x is : %s\n", x); //
            cpy = strstr(x, "/");
            // printf("cpy is : %s\n", cpy); //
        } while (cpy); // until cpy doesnt become null //
    }
    return x;
}
char *get_file_path(char *file_path)
{
    if (file_path == NULL)
        return NULL;
    char *x = (char *)malloc((strlen(file_path) + 1) * sizeof(char));
    strcpy(x, file_path);
    char *fileName = get_file_name_from_path(x);
    char *chk = strstr(x, fileName);
    if (chk)
    {
        strcpy(chk, "");
        return x;
    }
    else
        return NULL;
}
char *get_home_dir_path(char *path)
{
    char *ret_val = (char *)malloc(strlen(cwd_path) + strlen(path) + 7);
    sprintf(ret_val, "%s/%s", cwd_path, &path[1]);
    return ret_val;
}

void print_RED()
{
    if (!is_redir && !is_pipe)
        printf(ERROR);
}
void print_GREEN()
{
    if (!is_redir && !is_pipe)
        printf(PROMPT);
}
void print_YELLOW()
{
    if (!is_redir && !is_pipe)
        printf(YELLOW);
}
void print_WHITE()
{
    if (!is_redir && !is_pipe)
        printf(WHITE);
}
void print_MAGENTA()
{
    if (!is_redir && !is_pipe)
        printf(EXECUTABLE);
}
void print_RESET()
{
    if (!is_redir && !is_pipe)
        printf(RESET);
}
void print_BLUE()
{
    if (!is_redir && !is_pipe)
        printf(PROMPT_DIR);
}
