#ifndef SHELL_HELPER
#define SHELL_HELPER

char *remove_char_front_and_back(char *input, char ch);
// returns the modified string
char *get_file_name_from_path(char *file_path);
// returns the file name from given file path
char *get_file_path(char *file_path);
// removes file name and returns the path
char *get_home_dir_path(char *path);

void print_RED();
void print_GREEN();
void print_YELLOW();
void print_WHITE();
void print_MAGENTA();
void print_RESET();
void print_BLUE();
#endif