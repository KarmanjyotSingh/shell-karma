#include "pinfo.h"
#include "helper.h"

void execute_pinfo(int arg_count, char *argument[])
{
    pid_t pid_process;
    if (arg_count > 2)
    {
        fprintf(stderr, ERROR "INVALID NUMBER OF ARGUMENTS , GIVE SYNTAX OF THE TYPE PINFO <job_number>\n" RESET);
        return;
    }
    else if (arg_count == 1)
    {
        pid_process = SHELL_PID;
    }
    else
    {
        pid_process = strtol(argument[1], NULL, 10);
        if (pid_process < 0)
        {
            fprintf(stderr, ERROR "JOB NUMBER CAN'T BE A NEGATIVE NUMBER\n" RESET);
            return;
        }
    }
    char proc_stat_path[name_len], proc_exe_path[name_len];
    sprintf(proc_stat_path, "/proc/%d/status", pid_process);
    sprintf(proc_exe_path, "/proc/%d/exe", pid_process);
    FILE *flag_stat = fopen(proc_stat_path, "r");
    if (flag_stat == NULL)
    {
        perror(ERROR "pinfo ");
        fprintf(stderr, RESET);
        return;
    }
    int itr = 0;
    // read through the file line by line ufffff, see documentation on man page
    // using getline to fetch line by line
    char *read_line = NULL;
    size_t sz_read = 0;
    proc_info st_proc;
    st_proc.proc_pid = pid_process;
    while (getline(&read_line, &sz_read, flag_stat) != -1)
    {
        itr++;
        read_line[strlen(read_line) - 1] = '\0';
        if (itr == 3)
        {
            char *temp = strtok(read_line, ":");
            temp = strtok(NULL, " \t");
            if (temp != NULL)
                strcpy(st_proc.proc_status, temp);
            else
                st_proc.proc_status[0] = '\0';
        }
        else if (itr == 18)
        {
            char *temp = strtok(read_line, ":");
            temp = strtok(NULL, " \t");
            if (temp != NULL)
            {
                strcpy(st_proc.virtual_mem_size, temp); // got the size
                temp = strtok(NULL, " \t");
                if (temp != NULL)
                {
                    char tmp[name_len];
                    sprintf(tmp, " %s { Virtual Memory }", temp);
                    strcat(st_proc.virtual_mem_size, tmp);
                }
            }
            break;
        }
        free(read_line);
        read_line = NULL;
    }
    fclose(flag_stat);
    // now get the executables
    size_t flag_exe = readlink(proc_exe_path, read_line, name_len);
    if (flag_exe < 0)
    {
        perror(ERROR "COULD NOT FETCH THE EXECUTABLE ");
        fprintf(stderr, RESET);
        return;
    }
    read_line[flag_exe] = '\0';
    if (strcmp(read_line, cwd_path) == 0)
    {
        strcpy(st_proc.exec_path, "~");
    }
    else if (strlen(read_line) < strlen(cwd_path))
    {
        strcpy(st_proc.exec_path, read_line);
    }
    else
    {
        if (strstr(read_line, cwd_path) != NULL)
        {
            // path will be of the form cwdpath/some_path/
            int len = strlen(cwd_path);
            char *rel_dir = (char *)malloc((strlen(read_line) - len + 1) * sizeof(char));
            strcpy(rel_dir, &read_line[len]);
            char str[name_len];
            sprintf(str, "~%s", rel_dir);
            strcpy(st_proc.exec_path, str);
            free(rel_dir);
        }
        else
        {
            strcpy(st_proc.exec_path, read_line);
        }
    }
    if (pid_process == SHELL_PID)
        strcat(st_proc.proc_status, "+");
    // Print the data ..
    print_WHITE();
    printf("PID-- %d\nPROCESS STATUS -- %s\nMEMORY -- %s\nEXECUTABLE PATH -- %s\n", st_proc.proc_pid, st_proc.proc_status, st_proc.virtual_mem_size, st_proc.exec_path);
    print_RESET();
}