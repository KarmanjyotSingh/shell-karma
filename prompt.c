#include "header.h"
#include "prompt.h"
#include <time.h>
#include "jobs.h"

void init_shell()
{
    gethostname(HOSTNAME, name_len);      // get host name
    strcpy(USERNAME, getenv("USERNAME")); // get user name
    getcwd(cwd_path, name_len);
    SHELL_PID = getpid();
    strcpy(last_dir_visited, "");
    time_t temp;
    time(&temp);
    init_list();
    struct tm *login_time = localtime(&temp);
    SHELL_MONTH = login_time->tm_mon;
    sprintf(HISTORY_FILE_NAME, "%s/history.txt", cwd_path);
    SHELL_YEAR = login_time->tm_year;
    is_cwd = 1;
}
void prompt()
{
    char dir[name_len];
    getcwd(dir, name_len);

    int path = strcmp(dir, cwd_path);
    if (path == 0)
        is_cwd = 1;
    else
        is_cwd = 0;
    print_GREEN();
    printf("<%s@%s:", USERNAME, HOSTNAME);
    print_RESET();
    if (is_cwd == 1)
    {
        print_BLUE();
        printf("~");
        print_RESET();
        print_GREEN();
        printf("> ");
        print_RESET();
    }
    else if (strlen(dir) < strlen(cwd_path))
    {
        print_BLUE();
        printf("%s", dir);
        print_RESET();
        print_GREEN();
        printf("> ");
        print_RESET();
    }
    else
    {
        if (strstr(dir, cwd_path) != NULL)
        {
            // path will be of the form cwdpath/some_path/
            int len = strlen(cwd_path);
            char *rel_dir = (char *)malloc((strlen(dir) - len + 1) * sizeof(char));
            strcpy(rel_dir, &dir[len]);
            print_BLUE();
            printf("~%s", rel_dir);
            print_RESET();
            print_GREEN();
            printf("> ");
            print_RESET();

            free(rel_dir);
        }
        else
        {
            print_BLUE();
            printf("%s", dir);
            print_RESET();
            print_GREEN();
            printf("> ");
            print_RESET();
        }
    }
}