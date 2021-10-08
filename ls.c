#include "header.h"
#include "ls.h"
#include "helper.h"
#include <assert.h>
#include <time.h>
bool not_hidden(const struct dirent *s)
{
    // checks if a file is hidden or not
    if (strlen(s->d_name) > 0 && s->d_name[0] == '.')
    {
        return false;
    }
    return true;
}
char *get_file_permissions(char *file_path)
{
    // returns the malloced string of permission with given file path
    // permission string is 10 char long
    struct stat st;
    int flag = stat(file_path, &st);
    if (flag < 0)
    {
        perror(ERROR "FAILED TO FETCH FILE PERMISSIONS ");
        fprintf(stderr, RESET);
        return NULL;
    }
    char *permissions = (char *)malloc(sizeof(char) * (10 + 1));
    int i = 0;
    if (flag == 0 && S_ISDIR(st.st_mode))
        permissions[i++] = 'd';
    else
        permissions[i++] = '-';

    // check if user read bit is set
    if (st.st_mode & S_IRUSR)
        permissions[i++] = 'r';
    else
        permissions[i++] = '-';

    // check if user write bit is set
    if (st.st_mode & S_IWUSR)
        permissions[i++] = 'w';
    else
        permissions[i++] = '-';

    //check for user execute permission
    if (st.st_mode & S_IXUSR)
        permissions[i++] = 'x';
    else
        permissions[i++] = '-';

    // check if group read bit is set
    if (st.st_mode & S_IRGRP)
        permissions[i++] = 'r';
    else
        permissions[i++] = '-';

    // check if group write bit is set
    if (st.st_mode & S_IWGRP)
        permissions[i++] = 'w';
    else
        permissions[i++] = '-';

    //check for group execute permission
    if (st.st_mode & S_IXGRP)
        permissions[i++] = 'x';
    else
        permissions[i++] = '-';

    // check if other read bit is set
    if (st.st_mode & S_IROTH)
        permissions[i++] = 'r';
    else
        permissions[i++] = '-';

    // check if other write bit is set
    if (st.st_mode & S_IWOTH)
        permissions[i++] = 'w';
    else
        permissions[i++] = '-';

    //check for other execute permission
    if (st.st_mode & S_IXOTH)
        permissions[i++] = 'x';
    else
        permissions[i++] = '-';

    permissions[10] = '\0';

    return permissions;
}
void execute_ls(int arg_count, char *argument[])
{
    a_flag = false;
    l_flag = false;
    num_dir_ls = 0;
    // set the flags for ls //
    for (int i = 1; i < arg_count; i++)
    {
        if (argument[i] != NULL && argument[i][0] == '-')
        {
            if (argument[i][1] != '\0')
            {
                int itr = 1;
                while (argument[i][itr] != '\0')
                {
                    if (argument[i][itr] == 'a')
                        a_flag = true;
                    if (argument[i][itr] == 'l')
                        l_flag = true;
                    itr++;
                }
            }
        }
    }
    // printf("a_flag is : %d \n l_flag is  : %d\n", a_flag, l_flag); //
    // got list of flags a & l     char list_dir[arg_count][name_len];
    char list_dir[arg_count][name_len];
    for (int i = 1; i < arg_count; i++)
    {
        if (argument[i] != NULL && argument[i][0] != '-')
        {
            strcpy(list_dir[num_dir_ls], argument[i]);
            num_dir_ls++;
        }
        else if (argument[i] != NULL && strcmp(argument[i], "-") == 0)
        {
            strcpy(list_dir[num_dir_ls], argument[i]);
            num_dir_ls++;
        }
    }
    // and directory if any
    // simple ls command with the directory as cwd
    if (num_dir_ls == 0)
    {
        getcwd(list_dir[0], name_len);
        if (list_dir[0] == NULL)
        {
            perror(ERROR "");
            fprintf(stderr, RESET);
            return;
        }
        num_dir_ls++;
    }
    // call the ls main function for each of the directories/files supplied as arguments to ls //
    for (int i = 0; i < num_dir_ls; i++)
    {
        ls(list_dir[i]);
    }
}
void ls(char *file_path)
{
    if (l_flag == false)
    {
        char path[name_len + 10];
        if (file_path[0] != '\0' && file_path[0] == '~')
        {
            sprintf(path, "%s/%s", cwd_path, &file_path[1]);
        }
        else if (file_path[0] != '\0')
        {
            strcpy(path, file_path);
        }
        // printf("path is : %s \n", path);
        struct stat st;
        int flag = stat(path, &st);
        if (flag < 0)
        {
            perror(ERROR "");
            fprintf(stderr, RESET);
            return;
        }
        struct dirent **fs;
        int num_file;
        // check if a directory
        if (S_ISDIR(st.st_mode))
        {
            if (a_flag == true) // show the hidden files
                num_file = scandir(path, &fs, NULL, alphasort);
            else // don't show the hidden files
                num_file = scandir(path, &fs, not_hidden, alphasort);
            if (num_file < 0)
            {
                perror(ERROR "");
                fprintf(stderr, RESET);
                if (fs != NULL)
                    free(fs);
            }
            if (num_dir_ls > 1)
            {
                print_WHITE();
                printf("\n%s :\n", file_path);
                print_RESET();
            }
            for (int i = 0; i < num_file; i++)
            {
                if (fs[i]->d_type == 4)
                    print_BLUE();
                else if (fs[i]->d_type == 8)
                    print_YELLOW();
                printf("%s\n", fs[i]->d_name);

                free(fs[i]);
            }
            free(fs);
        }
        else // a file perhaps
        {
            print_YELLOW();
            printf("%s\n", path);
            print_RESET();
        }
    }
    else
    {
        char path[name_len + 10];
        if (file_path[0] != '\0' && file_path[0] == '~')
        {
            sprintf(path, "%s/%s", cwd_path, &file_path[1]);
        }
        else if (file_path[0] != '\0')
        {
            strcpy(path, file_path);
        }
        // printf("path is : %s \n", path);
        struct stat st;
        int flag = stat(path, &st);
        if (flag < 0)
        {
            perror(ERROR "ls");
            fprintf(stderr, RESET);
            return;
        }
        struct dirent **fs;
        int num_file;
        // check if a directory
        if (S_ISDIR(st.st_mode))
        {
            if (a_flag == true) // show the hidden files
                num_file = scandir(path, &fs, NULL, alphasort);
            else // don't show the hidden files
                num_file = scandir(path, &fs, not_hidden, alphasort);
            if (num_file < 0)
            {
                perror(ERROR "ls");
                fprintf(stderr, RESET);
                if (fs != NULL)
                    free(fs);
            }
            if (num_dir_ls > 1)
            {
                print_WHITE();
                printf("\n%s:\n", file_path);
                print_RESET();
            }
            blkcnt_t total_blocks = 0;
            for (int j = 0; j < num_file; j++)
            {
                char dir[2 * name_len];
                sprintf(dir, "%s/%s", path, fs[j]->d_name);
                struct stat st_tmp;
                int x = stat(dir, &st_tmp);
                if (x == 0)
                {
                    total_blocks += st_tmp.st_blocks;
                }
            }
            // got the total blocks
            print_WHITE();
            printf("\ntotal : %ld\n", total_blocks / 2);
            print_RESET();

            for (int j = 0; j < num_file; j++)
            {
                char dir[2 * name_len];
                sprintf(dir, "%s/%s", path, fs[j]->d_name);
                struct stat st_tmp;
                int x = stat(dir, &st_tmp);
                if (x < 0)
                {
                    perror(ERROR "ls");
                    fprintf(stderr, RESET);
                    continue;
                }
                else
                {
                    char *file_permissions = get_file_permissions(dir);
                    struct group *grp = getgrgid(st_tmp.st_gid);
                    struct passwd *pw = getpwuid(st_tmp.st_uid);
                    struct tm *file_time = localtime(&st_tmp.st_mtime);
                    nlink_t file_hlink = st_tmp.st_nlink;

                    char *time_lst_modified = (char *)malloc(sizeof(char) * 100);
                    bool flag_year = false;

                    if (file_time->tm_year == SHELL_YEAR && SHELL_MONTH - file_time->tm_mon < 6)
                        flag_year = false;
                    else if (file_time->tm_year == SHELL_YEAR && SHELL_MONTH - file_time->tm_mon >= 6)
                        flag_year = true;
                    else
                    {
                        int num_year = SHELL_YEAR - file_time->tm_year;
                        if (num_year > 1)
                            flag_year = true;
                        else
                        {
                            int mon = 11 - file_time->tm_mon + SHELL_MONTH;
                            // say its dec then mon = 1 + shell month , now for may shell month is 4 ryt  mon is 5
                            if (mon > 4)
                                flag_year = true;
                        }
                    }

                    if (flag_year == false)
                        strftime(time_lst_modified, 100, "%b  %d %H:%M", file_time);
                    else
                        strftime(time_lst_modified, 100, "%b  %d  %Y", file_time);

                    printf("%s\t%4ld\t%s\t%s\t%9ld\t%s\t", file_permissions, st_tmp.st_nlink, grp->gr_name, pw->pw_name, st_tmp.st_size, time_lst_modified);
                    if (file_permissions[0] == 'd')
                    {
                        print_BLUE();
                        printf("%s\n", fs[j]->d_name);
                        print_RESET();
                    }
                    else if (file_permissions[3] == 'x')
                    {
                        print_MAGENTA();
                        printf("%s\n", fs[j]->d_name);
                        print_RESET();
                    }
                    else
                    {
                        print_YELLOW();
                        printf("%s\n", fs[j]->d_name);
                        print_RESET();
                    }
                    free(fs[j]);
                    free(file_permissions);
                    free(time_lst_modified);
                }
            }
            free(fs);
        }
        else
        {
            // given path represents a file therefore no total printed only permissions well enough
            char *file_permissions = get_file_permissions(path);
            struct group *grp = getgrgid(st.st_gid);
            struct tm *file_time = localtime(&st.st_mtime);
            struct passwd *pw = getpwuid(st.st_uid);
            nlink_t file_hlink = st.st_nlink;
            char *time_lst_modified = (char *)malloc(sizeof(char) * 100);
            bool flag_year = false;

            if (file_time->tm_year == SHELL_YEAR && SHELL_MONTH - file_time->tm_mon < 6)
                flag_year = false;
            else if (file_time->tm_year == SHELL_YEAR && SHELL_MONTH - file_time->tm_mon >= 6)
                flag_year = true;
            else
            {
                int num_year = SHELL_YEAR - file_time->tm_year;
                if (num_year > 1)
                    flag_year = true;
                else
                {
                    int mon = 11 - file_time->tm_mon + SHELL_MONTH;
                    // say its dec then mon = 1 + shell month , now for may shell month is 4 ryt  mon is 5
                    if (mon > 4)
                        flag_year = true;
                }
            }
            if (flag_year == false)
                strftime(time_lst_modified, 100, "%b  %d %H:%M", file_time);
            else
                strftime(time_lst_modified, 100, "%b  %d  %Y", file_time);

            printf("%s\t%4ld\t%s\t%s\t%9ld\t%s\t", file_permissions, st.st_nlink, grp->gr_name, pw->pw_name, st.st_size, time_lst_modified);
            if (file_permissions[3] == 'x')
            {
                print_MAGENTA();
                printf("%s\n", file_path);
                print_RESET();
            }
            else
            {
                print_YELLOW();
                printf("%s\n", file_path);
                print_RESET();
            }
            free(file_permissions);
            free(time_lst_modified);
        }
    }
}
