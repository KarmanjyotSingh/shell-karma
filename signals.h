#ifndef SHELL_SIGNALS
#define SHELL_SIGNALS

void SIGCHILD_HANDLER(int signum);
void SIGINT_HANDLER(int signum);
void SIGTSTP_HANDLER(int signum);
void execute_sig(int arg_count, char *argument[]);
#endif