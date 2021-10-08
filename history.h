#ifndef SHELL_HISTORY
#define SHELL_HISTORY
#include "header.h"

// using a double linked list data structure for storing the history for 20 elements
struct Node
{
    char command[name_len];
};

int hist_count;
int last_hist_entry_added;

struct Node history_list[20];

void init_history();

void add_to_history(char *command);

void write_back_history();

void history(int arg_count, char *argument[]);

#endif