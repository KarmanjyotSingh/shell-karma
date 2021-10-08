#include "jobs.h"
#include "header.h"
int err_code = 0;
void init_list()
{
    bg_num_proc = 0;
    proc_list.num_proc = 0;
    proc_list.start = NULL;
}

void print_error_jobs()
{
    print_YELLOW();
    printf("Usage :\n\njobs [options] \n\n");
    printf("        -r for running background processes\n\n        -s for stopped background process\n\nFlags can be in any order\n\n");
    printf("         Multiple flags are handled, in case of illegal flags , apt error is displayed , and jobs is aborted\n\n");
    fflush(stdout);
}

jobs *make_node(char *proc, pid_t pid, int job_num)
{
    jobs *ret_job = (jobs *)malloc(sizeof(jobs));
    ret_job->job_num = job_num;
    ret_job->next = NULL;
    strcpy(ret_job->proc_name, proc);
    ret_job->proc_pid = pid;
    return ret_job;
}
void add_node(char *proc_name, pid_t pid)
{
    proc_list.num_proc++;
    bg_num_proc = 0;
    for (jobs *itr = proc_list.start; itr != NULL; itr = itr->next)
    {
        if (bg_num_proc < itr->job_num)
            bg_num_proc = itr->job_num;
    }
    bg_num_proc++;
    jobs *node = make_node(proc_name, pid, bg_num_proc);
    if (proc_list.start == NULL)
    {
        proc_list.start = node;
    }
    else
    {
        jobs *curr = proc_list.start, *temp = NULL;
        while (curr != NULL && strcmp(curr->proc_name, proc_name) <= 0)
        {
            temp = curr;
            curr = curr->next;
        }
        if (curr == NULL)
        {
            temp->next = node;
        }
        else if (temp == NULL)
        {
            node->next = proc_list.start;
            proc_list.start = node;
        }
        else
        {
            node->next = temp->next;
            temp->next = node;
        }
    }
}
void remove_node(pid_t pid)
{
    if (proc_list.num_proc == 0 || pid <= 0)
        return;
    jobs *temp = NULL;
    for (jobs *itr = proc_list.start; itr != NULL; itr = itr->next)
    {
        if (itr->proc_pid == pid)
        {
            if (temp == NULL) // first node
            {
                proc_list.start = itr->next;
                free(itr);
                proc_list.num_proc--;
            }
            else
            {
                temp->next = itr->next;
                free(itr);
                proc_list.num_proc--;
            }
        }
        temp = itr;
    }
}
jobs *get_data_by_pid(pid_t pid)
{
    jobs *ret_data = NULL;
    for (jobs *itr = proc_list.start; itr != NULL; itr = itr->next)
    {
        if (itr->proc_pid == pid)
        {
            ret_data = itr;
            break;
        }
    }
    return ret_data;
}
jobs *get_data_by_id(int job_id)
{
    jobs *ret_data = NULL;
    for (jobs *itr = proc_list.start; itr != NULL; itr = itr->next)
    {
        if (itr->job_num == job_id)
        {
            ret_data = itr;
            break;
        }
    }
    return ret_data;
}
char *get_job_status(pid_t pid)
{
    char proc_stat_path[name_len];
    sprintf(proc_stat_path, "/proc/%d/status", pid);
    FILE *flag_stat = fopen(proc_stat_path, "r");
    if (flag_stat == NULL)
    {
        return NULL;
    }
    int itr = 0;
    // read through the file line by line ufffff, see documentation on man page
    // using getline to fetch line by line
    char *read_line = NULL;
    size_t sz_read = 0;
    char *ret_val = NULL;
    while (getline(&read_line, &sz_read, flag_stat) != -1)
    {
        itr++;
        read_line[strlen(read_line) - 1] = '\0';
        if (itr == 3)
        {
            char *temp = strtok(read_line, ":");
            temp = strtok(NULL, " \t");
            if (temp != NULL)
            {
                ret_val = (char *)malloc(sizeof(char) * (strlen(temp) + 1));
                strcpy(ret_val, temp);
                free(read_line);
                fclose(flag_stat);
                return ret_val;
            }
            else
            {
                free(read_line);
                fclose(flag_stat);
                err_code = 1;
                return NULL;
            }
            break;
        }
        free(read_line);
        read_line = NULL;
    }
}
void execute_jobs(int arg_count, char *argument[])
{

    if (proc_list.num_proc == 0)
    {
        fprintf(stderr, ERROR "jobs : no background process running currently\n" RESET);
        return;
    }
    bool r_flag = false;
    bool s_flag = false;
    for (int i = 1; i < arg_count && argument[i] != NULL; i++)
    {
        if (argument[i][0] == '-')
        {
            if (argument[i][1] != '\0')
            {
                int itr = 1;
                while (argument[i][itr] != '\0')
                {
                    if (argument[i][itr] == 's')
                        s_flag = true;
                    if (argument[i][itr] == 'r')
                        r_flag = true;
                    else
                    {
                        fprintf(stderr, ERROR "Flag : %s , not an option\n", argument[i]);
                        print_error_jobs();
                        return;
                    }

                    itr++;
                }
            }
            else
            {
                fprintf(stderr, ERROR "Not a valid flag\n");
                print_error_jobs();
                return;
            }
        }
    }
    // got the flags
    for (jobs *itr = proc_list.start; itr != NULL;)
    {
        char *proc_st = get_job_status(itr->proc_pid);
        if (proc_st == NULL)
        {
            if (err_code == 2)
                fprintf(stderr, ERROR "unable to get process status \n");
            jobs *temp = itr;
            itr = itr->next;
            // remove_node(temp->proc_pid);
            err_code = 0;
            continue;
        }
        if (r_flag && (strcmp(proc_st, "R") == 0 || strcmp(proc_st, "S") == 0))
        {
            print_BLUE();
            printf("[%d]    ", itr->job_num);
            print_RESET();
            print_YELLOW();
            printf("Running     ");
            print_WHITE();
            printf("%s      ", itr->proc_name);
            print_YELLOW();
            printf("[%d]\n", itr->proc_pid);
            print_RESET();
        }
        if (s_flag && strcmp(proc_st, "T") == 0)
        {
            print_BLUE();
            printf("[%d]    ", itr->job_num);
            print_RESET();
            print_YELLOW();
            printf("Stopped     ");
            print_WHITE();
            printf("%s      ", itr->proc_name);
            print_YELLOW();
            printf("[%d]\n", itr->proc_pid);
            print_RESET();
        }
        if (!r_flag && !s_flag)
        {
            print_BLUE();
            printf("[%d]    ", itr->job_num);
            print_RESET();
            print_YELLOW();
            if (strcmp(proc_st, "S") == 0 || strcmp(proc_st, "R") == 0)
                printf("Running     ");
            else if (strcmp("T", proc_st) == 0)
                printf("Stopped     ");
            print_WHITE();
            printf("%s      ", itr->proc_name);
            print_YELLOW();
            printf("[%d]\n", itr->proc_pid);
            print_RESET();
        }
        itr = itr->next;
    }
}
