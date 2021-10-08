#include "header.h"
#include "execute.h"
#include "replay.h"

void print_error()
{
    print_YELLOW();
    printf("Usage :\n\nreplay -command <command_to_be_replayed> -interval <interval> -period <period>\n\n");
    printf("        <interval> and <period> should be positive integers\n\nFlags can be in any order\n\n");
    printf("         In case of same flag occuring multiple times, <replay> command is abborted\n\n");
    fflush(stdout);
}
void execute_replay(int arg_count, char *argument[])
{
    //  replay -command echo "hi" -interval 3 -period 6
    printf("Executing replay command \n");
    bool command_present = false;
    bool interval_present = false;
    bool period_present = false;
    char cpy_command[name_len] = "";
    int period;
    int interval;
    if (arg_count < 7)
    {
        fprintf(stderr, ERROR "Invalid Number of Arguments \n" RESET);
        print_error();
        return;
    }
    for (int i = 1; i < arg_count;)
    {
        if (argument[i] != NULL && strcmp("-command", argument[i]) == 0)
        {
            if (command_present)
            {
                fprintf(stderr, ERROR "MORE THAN ONE ARGUMENT PROVIDED FOR COMMAND FLAG !! - INVALID SYNTAX\n" RESET);
                print_error();
                return;
            }
            else
            {
                i++;
                command_present = true;
                while (1)
                {
                    if (argument[i] == NULL)
                        break;
                    if (argument[i][0] == '-')
                    {
                        break;
                    }
                    strcat(cpy_command, argument[i]);
                    strcat(cpy_command, " ");
                    i++;
                }
            }
        }
        if (argument[i] != NULL && strcmp("-interval", argument[i]) == 0)
        {
            i++;
            while (1)
            {
                if (argument[i] == NULL)
                    break;
                if (argument[i][0] == '-')
                {
                    break;
                }
                if (interval_present)
                {
                    fprintf(stderr, ERROR "MORE THAN ONE ARGUMENT PROVIDED FOR INTERVAL FLAG !! - INVALID SYNTAX\n" RESET);
                    print_error();
                    return;
                }
                interval = atoi(argument[i]);
                if (interval <= 0)
                {
                    fprintf(stderr, ERROR "ENTER A POSITIVE INTEGER AS ARGUMENT FOR INTERVAL FLAG\nSYNTAX : replay -command <command name> - interval <interval> - period < period >\n" RESET);
                    return;
                }
                interval_present = true;
                i++;
            }
        }
        if (argument[i] != NULL && strcmp("-period", argument[i]) == 0)
        {
            i++;
            while (1)
            {
                if (argument[i] == NULL)
                    break;
                if (argument[i][0] == '-')
                {
                    break;
                }
                if (period_present)
                {
                    fprintf(stderr, ERROR "MORE THAN ONE ARGUMENT PROVIDED FOR PERIOD FLAG !! - INVALID SYNTAX\n" RESET);
                    print_error();
                    return;
                }
                period = atoi(argument[i]);
                if (period <= 0)
                {
                    printf(ERROR "ENTER A POSITIVE INTEGER AS ARGUMENT FOR PERIOD FLAG\nSYNTAX : replay -command <command name> - interval <interval> - period < period >\n" RESET);
                    return;
                }
                period_present = true;
                i++;
            }
        }
    }
    if (command_present && interval_present && period_present)
    {
        //   printf("command is : %s\n interval is %d\nperiod is %d\n", cpy_command, interval, period);
        char cpy[name_len];
        strcpy(cpy, cpy_command);
        char sleep_command[50], cpy_sleep[50];
        sprintf(sleep_command, "sleep %d", interval);
        strcpy(cpy_sleep, sleep_command);
        for (int i = interval; i <= period; i += interval)
        {
            execute(sleep_command);
            execute(cpy_command);
            strcpy(cpy_command, cpy);
            strcpy(sleep_command, cpy_sleep);
        }
    }
}
// got the flags
