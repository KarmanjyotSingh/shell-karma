#ifndef SHELL_HEADER
#define SHELL_HEADER

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/dir.h>
#include <errno.h>
#include <sys/param.h>
#include <dirent.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include "helper.h"

#define bool int
#define true 1
int is_fg;
#define false 0
#define command_capacity 512
#define name_len 1024
#define maxflags 16
#define POSIX_SPACE_DELIMITER " \n\t\r"
#define COMMAND_DELIMITER ";\n"

#define ERROR "\e[1;91m"      // red
#define PROMPT "\e[1;32m"     // green
#define PROMPT_DIR "\e[1;94m" // blue
#define EXECUTABLE "\e[1;95m" // magenta
#define YELLOW "\e[0;93m"
#define WHITE "\e[1;97m"
#define RESET "\e[0;0m"

int copy_stdin_fileno;
int copy_stdout_fileno;
int num_command;
bool is_redir;
bool is_pipe;
char HISTORY_FILE_NAME[name_len + 50];
pid_t SHELL_PID;
char HOSTNAME[name_len];
char USERNAME[name_len];
char last_dir_visited[name_len];
char cwd_path[name_len]; // store  the path to the current working directory from where the shell was run
bool is_cwd;
int SHELL_YEAR;  // current login_year
int SHELL_MONTH; // login_month

#endif