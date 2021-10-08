#ifndef SHELL_TOKENIZE
#define SHELL_TOKENIZE

char **getList(char *input); // returns the tokenized array of strings with all the commands
void replaceTabs(char **list);
void RemoveExtraSpaces(char **list);

#endif