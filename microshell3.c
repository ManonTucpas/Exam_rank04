#include <libc.h>

typedef struct s_cmd
{
    int pipe[2];
    char    **cmd_names;
    struct s_cmd *next;
}               t_cmd;

int ft_strlen(char *s)
{
    int i = 0;

    while(s[i])
        i++;
        return (i);
}

int ft_size(char **s)
{
    int i = 0;
    while (s[i])
        i++;
    return (i);
}

void    write_err(char *s)
{
    int i;
    i = ft_strlen(s);
    write(2, s, i);
    write(2, "\n", 1);
    return ;
}

int     getpipenb_until(char **av, char ***tmp)
{
    int i;
    int nb;

    nb= 0;
    i = 0;
    while (av[i] && strcmp(av[i], ";") != 0)
    {
        if (av[i] && strcmp(av[i], "|") == 0)
            nb++;
        i++;
    }
    if (av[i] && strcmp(av[i], ";") == 0)
    {
         av[i] = 0;
        *tmp =  &(av[i +1]);
    }
    return (nb);
}

void    replace_pipe_getcmd(char **av, t_cmd *cmd, int nb)
{
    int i;
    int a;

    a = 1;
    i = 0;
    cmd[0].cmd_names = &(av[0]);
    while(av[i])
    {
        if (av[i] && strcmp(av[i], "|") == 0)
        {
            av[i] = 0;
            cmd[a++].cmd_names = &(av[i + 1]);
            av =  &(av[i + i]);
            i = 0;
        }
        else
            i++;
    }
    i = 0;
    while (i < nb)
    {
        cmd[i].next = &(cmd[i + 1]);
        i++;
    }
    cmd[i].next = NULL;
}


int launch(t_cmd *cmd, char **env)
{
    int status = 0;
    int i = 0;
    pid_t pid;

    while (cmd)
    {
        if (strcmp(cmd[i].cmd_names[0], "cd") == 0)
        {
            if (ft_size(cmd[i].cmd_names) != 2)
            {
                write_err("error: cd: bad arguments");
                return (0);
            }
           
            if (chdir(cmd[i].cmd_names[1]) != 0)
            {
                
                int len = ft_strlen("error: cd: cannot change directory to ");
                write(2, "error: cd: cannot change directory to ", len);
                write_err(cmd[i].cmd_names[1]);
                return (0);
            }
            if (!cmd[i].next)
                return (0);
            i++;
        }
        else
        {
            pipe(cmd[i].pipe);
            pid  = fork();
            if (pid < 0)
            {
                write_err("error: fatal");
                return (0);
            }
            if (pid == 0)
            {
                if (cmd[i].next)
                    dup2(cmd[i].pipe[1], 1);
                if (i != 0)
                    dup2(cmd[i - 1].pipe[0], 0);
                if (execve(cmd[i].cmd_names[0], cmd[i].cmd_names, env) < 0)
                {
                    int len = ft_strlen("error: cannot execute ");
                    write(2, "error: cannot execute ", len);
                    write_err(cmd[i].cmd_names[0]);
                    exit (1);
                }
            }
            else
            {
                waitpid(pid, &status, 0);
                if (cmd[i].next)
                    close(cmd[i].pipe[1]);
                if (i != 0)
                    close(cmd[i - 1].pipe[0]);
                if (!cmd[i].next)
                    return (0);
                i++;
            }
        }
    }
    return (1);
}

int main(int argc, char **av, char **env)
{
    int i;
    int nb;

    i = 0;
    av++;
    while(av[i++])
    {
        char **tmp = NULL;
        nb = getpipenb_until(av, &tmp);
        t_cmd cmd[nb + 1];

        if (*av)
        {
            replace_pipe_getcmd(av, cmd, nb);
            launch(cmd, env);
        }
        if (tmp)
        {
            av = tmp;
            i = 0;
        }
        else
            return (1);
    }
    return (0);
}