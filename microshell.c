#include <libc.h>

typedef struct  s_cmd
{
    char            **cmd_name;
    int             pipe[2];
    struct s_cmd   *next;
}               t_cmd;

int    get_pipenb_until(char **av, char ***tmp)
{
    int i;
    int nb;

    i = 0;
    nb = 0;
    while(av[i] && strcmp(av[i], ";") != 0)
    {
        if (av[i] && strcmp(av[i], "|") == 0)
            nb++;
        i++;
    }
    if(av[i] && strcmp(av[i], ";") == 0)
    {
        av[i] = 0;
        *tmp = &(av[i + 1]);
    }
    return (nb);

}


void printtab(char **str)
{
    int i = 0;
    while (str[i])
    {
        printf("%s\n", str[i]);
        i++;
    }
}

void    get_cmd_setpipezero(char **av, t_cmd *cmd, int nb_pipe)
{
    int i;
    int a;

    i = 0;
    a = 1;

    cmd[0].cmd_name = &(av[0]);
    while(av[i])
    {
        if (av[i] && strcmp(av[i], "|") == 0)
        {
            av[i] = 0;
            cmd[a++].cmd_name = &(av[i + 1]);
           
            av = &(av[i + 1]);
            i = 0;
        }
        else
            i++;
    }
    i = 0;
    while(i < nb_pipe)
    {
        cmd[i].next = &(cmd[i + 1]);
        i++;
    }
    cmd[i].next = NULL;

}

int main(int argc, char **argv, char **env)
{
    
    int i;
    int nb_pipe;

    argv++;

    i = 0;
    while(argv[i++])
    {
        char **tmp = NULL;
        nb_pipe = get_pipenb_until(argv, &tmp);
        printf("nbpipe = %d\n", nb_pipe);
        t_cmd cmd[nb_pipe + 1];

        if (*argv)
        {
            printf("HERE\n");
            get_cmd_setpipezero(argv, cmd, nb_pipe);
          // begin_shell(env, cmd_name);

        }
        if (tmp)
        {
            argv = tmp;
            i = 0;
        }
        else 
            return (1);
    }
    return (0);
}