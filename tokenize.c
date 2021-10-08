#include "header.h"
#include "tokenize.h"
#include "helper.h"

char **getList(char *input)
{
    // input  contains the string of input
    // assuming a max of command_capacity == 512 number of commands
    char **list = (char **)malloc(100 * sizeof(char *));
    num_command = 0; // initialising the number of commands to 0
    char *temp = strtok(input, ";");
    while (temp != NULL)
    {
        list[num_command] = temp;
        if (temp[0] != '\n')
            num_command++;
        temp = strtok(NULL, ";");
    }
    // we'll get a space filled list of commands
    // printf("%d\n", num_command); check
    // replace tabs with spaces
    replaceTabs(list);
    // replace extra spaces to single places
    // for (int i = 0; i < num_command; i++)
    // remove_char_front_and_back(list[i], ' ');
    RemoveExtraSpaces(list);
    return list;
}
void replaceTabs(char **list)
{
    for (int i = 0; i < num_command; i++)
    {
        for (int j = 0; j < strlen(list[i]); j++)
        {
            if (list[i][j] == '\t')
                list[i][j] = ' ';
        }
    }
}
void RemoveExtraSpaces(char **list)
{
    for (int itr = 0; itr < num_command; itr++)
    {
        char *string = list[itr];
        char cpyCommand[name_len];
        // firstly remove the spaces in the front before the start
        int j = 0;
        while (string[j] == ' ')
            j++;

        bool prevspace = false;
        int i = 0;
        while (1)
        {
            if (string[j] != ' ')
            {
                cpyCommand[i] = string[j];
                j++;
                i++;
                prevspace = false;
            }
            else if (string[j] == ' ' && prevspace == false)
            {
                cpyCommand[i] = string[j];
                j++;
                i++;
                prevspace = true;
            }
            else
            {
                j++;
            }
            if (j == strlen(string))
                break;
        }
        // append the null character , look for last mein space
        if (cpyCommand[i - 1] == ' ')
            cpyCommand[i - 1] = '\0';
        else
            cpyCommand[i] = '\0';
        // copy the string to original store placeholder
        strcpy(list[itr], cpyCommand);
    }
}