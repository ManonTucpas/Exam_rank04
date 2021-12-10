#include <libc.h>



typedef struct cmd {

    int pipe[2];
    char **cmd;
    struct cmd *next;

}   cmd;

int c(char **av)
{
    int i= 0;

    while(av[i])
        i++;
    return i;
}

int count(char **av, char ***tmp)
{
    int i = 0;
    int nb = 0;

    for (; av[i] && strcmp(av[i], ";") != 0; i++)
    {
        if (av[i] && strcmp(av[i], "|") == 0)
            nb++;
    }

    if (av[i] && strcmp(av[i], ";") == 0)
    {
        av[i] = 0;
        *tmp = &(av[i+1]);
    }
    return nb;
}


void sett(char **av, cmd *cmd, int nb)
{
    int i = 0;
    int a = 1;

    cmd[0].cmd = &(av[0]);

    while ( av[i] )
    {
        if (av[i] && strcmp(av[i], "|") == 0)
        {
            av[i] = 0;
            cmd[a++].cmd = &(av[i+1]);
            av  = &(av[i+i]);
            i = 0;
        }
        else    
            i++;

    }

    i = 0;

    for (; i < nb; i++)
        cmd[i].next = &(cmd[i+1]);
    cmd[i].next = NULL;
    
    
}

void printt(char **av)
{
    for (int i = 0; av[i]; i++)
        printf("%s\n", av[i]);
    printf("----------------\n");
}


int start(char **env, cmd *cmd)
{
    int status = 0, i = 0;
    pid_t pid;



    while (cmd)
    {
        if (strcmp(cmd[i].cmd[0], "cd") == 0)
        {
            if (c(cmd[i].cmd) != 2)
            {
                printf("error cd : not enought argvs\n");
                return 0;
            }
            else if (chdir(cmd[i].cmd[1]) != 0)
            {
                printf("error cd : cannot redirect\n");
                return 0;
            }
            if (!cmd[i].next)
                    return 0;
            i++;
        }

        else
        {
            pipe(cmd[i].pipe );
            pid = fork();

            if (pid < 0)
            {
                printf("error fork");
                return 0;
            }
            if (pid == 0)
            {
                    if (cmd[i].next)
                        dup2(cmd[i].pipe[1], 1);
                    if (i != 0)
                        dup2(cmd[i-1].pipe[0], 0);

                    if (execve(cmd[i].cmd[0], cmd[i].cmd, env) < 0)
                    {
                        printf("unknow commands\n");
                        exit(1);
                    }
            }
            else
            {
                waitpid(pid, &status, 0);
                if (cmd[i].next)
                    close(cmd[i].pipe[1]);
                if (i != 0)
                    close(cmd[i-1].pipe[0]);
                if (!cmd[i].next)
                    return 0;
                i++;
            }
        }
    }
    return 1;
}

int main(int ac, char **av, char **env)
{
    av++;
    int i = 0;
    while (av[i++])
    {   
        char **tmp = NULL;
        int nb = count(av, &tmp);

        cmd cmd[nb+1];


        if (*av)
        {
            sett(av, cmd, nb);
            start(env, cmd);
        }

        if (tmp)
        {
            av = tmp;
            i = 0;
        }
        else
            return 1;
    }

    return 0;
}