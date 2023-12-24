#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include<sys/wait.h>

enum type
{
    PIPE,
    EXCEC,
    NUL,

};
int ft_strlen(char *str)
{
    int i = 0;
    while(str[i])
        i++;
    return (i);
}

void excec_cd(char **argv)
{
    if (argv[1] == NULL  || argv[2] != NULL)
        write(2, "error: cd: bad arguments\n", 26);
    else
    {
        if (chdir(argv[1]) == -1)
        {
            write(2, "error: cd: cannot change directory to ", 38);
            write(2, argv[1], ft_strlen(argv[1]));
            write(2, "\n", 2);
        }
    }
}
void print_error(char **argv)
{
    write(2, "error: cannot execute ", 22);
    write(2, argv[0], ft_strlen(argv[0]));
    write(2, "\n", 1);
}
void execute_cmd(char** argv, enum type typ, char **env)
{
    int fd[2];
    if (typ == PIPE)
    {
    if (pipe(fd) == -1)
    {
        write(2, "error: fatal\n", 14);
        exit(1);
    }
    }
    int f = fork();
    if (f == 0)
    {
        if(typ == PIPE)
        {
            if (dup2(fd[1], 1)== -1)
            {
                write(2, "error: fatal\n", 14);
                exit(1);
            }
            close(fd[1]);
            close(fd[0]);
        }
        execve(argv[0], argv, env);
        print_error(argv);
        exit(1);
    }
    waitpid(f, NULL, 0);
    if (typ == PIPE)
    {
        if(dup2(fd[0], 0) == -1)
        {
            write(2, "error: fatal\n", 14);
            exit(1);
        }
        close(fd[0]);
        close(fd[1]);
    }
}
int main(int argc, char **argv, char **env)
{

    if (argc > 1)
    {
        enum type typ;
        int i = 0;
        argv++;
        while (argv[i] && argv[i + 1])
        {   
            if (strcmp(argv[i], "|") == 0)
            {
                typ = PIPE;
                argv[i] = NULL;
                if (strcmp(argv[0], "cd") == 0)
                    excec_cd(argv);
                else
                    execute_cmd(argv, typ, env);
                argv = &argv[i + 1];
                i = 0;
            }
            else if (strcmp(argv[i], ";") == 0)
            {
                typ = EXCEC;
                argv[i] = NULL;
                if (strcmp(argv[0], "cd") == 0)
                    excec_cd(argv);
                else
                    execute_cmd(argv, typ, env);
                argv = &argv[i + 1];
                i = 0;
            }
            i++;
        }
        typ = NUL;
        if (strcmp(argv[0], "cd") == 0)
            excec_cd(argv);
        else
            execute_cmd(argv, typ,  env);
    }
}