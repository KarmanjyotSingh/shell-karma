#include "header.h"
#include "history.h"

void init_history()
{
    FILE *hist_file = fopen(HISTORY_FILE_NAME, "r");
    hist_count = 0;
    last_hist_entry_added = 0;
    if (hist_file == NULL)
    {
        hist_file = fopen(HISTORY_FILE_NAME, "w"); // create the file
        fclose(hist_file);
        return;
    }
    char *input = NULL;
    size_t sz_line = 0;
    while (getline(&input, &sz_line, hist_file) != -1)
    {
        if (input[strlen(input) - 1] == '\n')
            input[strlen(input) - 1] = '\0';
        strcpy(history_list[hist_count].command, input);
        hist_count++;
    }
    last_hist_entry_added = hist_count - 1;
    int x = fclose(hist_file);
    if (x < 0)
    {
        fprintf(stderr, ERROR "UNABLE TO LOAD HISTORY\n" RESET);
    }
}
void add_to_history(char *command)
{
    if (command == NULL)
        return;
    if (hist_count == 0)
    {
        strcpy(history_list[hist_count].command, command);
        last_hist_entry_added++;
        hist_count++;
        return;
    }
    int new_idx = hist_count % 20;

    if (strcmp(history_list[last_hist_entry_added].command, command) != 0)
    {
        strcpy(history_list[new_idx].command, command);
        last_hist_entry_added = new_idx;
        hist_count++;
        return;
    }
}
void write_back_history()
{
    FILE *hist_file = fopen(HISTORY_FILE_NAME, "w");
    if (hist_file == NULL)
    {
        fprintf(stderr, ERROR);
        fprintf(stderr, RESET);
        return;
    }
    int sz_history = 20;
    if (hist_count < 20)
        sz_history = hist_count;

    if (hist_count >= 20)
    {
        int start_idx = hist_count % 20;
        for (int i = start_idx; i < sz_history; i++)
            fprintf(hist_file, "%s\n", history_list[i].command);

        for (int i = 0; i < start_idx; i++)
            fprintf(hist_file, "%s\n", history_list[i].command);
    }
    else
    {
        for (int i = 0; i < sz_history; i++)
            fprintf(hist_file, "%s\n", history_list[i].command);
    }
    int x = fclose(hist_file);
    if (x < 0)
    {
        fprintf(stderr, ERROR "UNABLE TO WRITE_BACK HISTORY\n" RESET);
    }
}
void history(int arg_count, char *argument[])
{
    if (arg_count > 2)
    {
        fprintf(stderr, ERROR "history : TOO MANY ARGUMENTS\n" RESET);
        return;
    }
    if (arg_count == 1)
    {
        int sz_history = 10, cnt = 0;
        if (hist_count < 10)
        {
            sz_history = hist_count;
            if (hist_count == 0)
            {
                fprintf(stderr, ERROR "EMPTY HISTORY NOTHING TO SHOW.\n" RESET);
                return;
            }
        }
        for (int i = last_hist_entry_added;; i--)
        {
            if (i < 0)
                i = sz_history - 1;
            print_WHITE();
            printf("[%d] ", cnt + 1);
            print_YELLOW();
            printf(": %s\n", history_list[i].command);
            cnt++;
            if (cnt == sz_history)
                break;
        }
    }
    else
    {
        int idx = atoi(argument[1]);
        if (idx <= 0)
        {
            fprintf(stderr, ERROR "history : PROVIDE A POSITIVE INTEGER AS ARGUMENT\n" RESET);
            return;
        }
        int cnt = 0;
        int sz_history = 20;
        if (hist_count < 20)
        {
            sz_history = hist_count;
            if (hist_count == 0)
            {
                fprintf(stderr, ERROR "EMPTY HISTORY NOTHING TO SHOW\n" RESET);
                return;
            }
            if (idx > hist_count)
            {
                fprintf(stderr, ERROR "HISTORY ONLY HAS %d ENTRIES TO SHOW, DISPLAYING THEM\n" RESET, hist_count);
                idx = hist_count;
            }
        }
        if (idx > 20)
        {
            fprintf(stderr, ERROR "HISTORY ONLY HAS 20 ENTRIES TO SHOW, DISPLAYING THEM\n" RESET);
            idx = 20;
        }
        // last_hist contains place for
        int start_idx = hist_count % 20;
        for (int i = last_hist_entry_added;; i--)
        {
            if (i < 0)
                i = sz_history - 1;
            print_WHITE();
            printf("[%d] ", cnt + 1);
            print_YELLOW();
            printf(": %s\n", history_list[i].command);
            cnt++;
            if (cnt == idx)
                break;
        }
    }
}