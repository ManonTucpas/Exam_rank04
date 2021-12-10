#include <libc.h>

typedef struct s_cmd
{
	int             pipe[2];
	char            **cmd_names;
	struct s_cmd    *next;
}               t_cmd;


int ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int getnbpipe_until(char **av, char ***tmp)
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
  if (av[i] && strcmp(av[i], ";")== 0)
  {
	  av[i] = 0;
	  *tmp = &(av[i + 1]);
  }
  return (nb);
}

void getcmd_replacepipe(char **av, t_cmd *cmd, int nb)
{
	int i;
	int a;

	i = 0;
	a = 1;
	cmd[0].cmd_names = &(av[0]);
	while(av[i++])
	{
		if (av[i] && strcmp(av[i], "|") == 0)
		{
			av[i] = 0;
			cmd[a++].cmd_names = &(av[i +1]);
			av = &(av[i+ i]);
			i = 0;
		}
		else
			i++;
	}
	i = 0;
	while(i < nb)
	{
		cmd[i].next = &(cmd[i + 1]);
		i++;
	}
	cmd[i].next = NULL;
}

void	ft_write_err(char *str)
{
	int len = ft_strlen(str);
	write(2, str, len);
	write(2, "\n", 1);
	return ;
}

int ft_size(char **str)
{
	int i = 0;

	while(str[i])
		i++;
	return(i);
}

int launchshell(char **env, t_cmd *cmd)
{
	int status;
	int i;
	pid_t pid;

	i = 0;
	status = 0;
	while(cmd)
	{
		if (strcmp(cmd[i].cmd_names[0], "cd") == 0)
		{
			if (ft_size(cmd[i].cmd_names) != 2)
			{
				ft_write_err("error: cd: bad arguments");
				return (0);
			}
			else if (chdir(cmd[i].cmd_names[1]) != 0)
			{
				int len = ft_strlen("error: cd: cannot change directory to ");
				write(2, "error: cd: cannot change directory to ", len);
				ft_write_err(cmd[i].cmd_names[1]);
				return (0);
			}
			if (!cmd[i].next)
				return (0);
			i++;
		}
		else
		{
			pipe(cmd[i].pipe);
			pid = fork();
			if (pid < 0)
			{
				ft_write_err("error: fatal");
				return (0);
			}
			if (pid == 0)
			{
				if(cmd[i].next)
					dup2(cmd[i].pipe[1], 1);
				if (i != 0)
					dup2(cmd[i - 1].pipe[0], 0);
				if (execve(cmd[i].cmd_names[0], cmd[i].cmd_names, env) < 0)
				{
					int len = ft_strlen("error: cannot execute ");
					write(2, "error: cannot execute ", len);
					ft_write_err(cmd[i].cmd_names[0]);
					exit(1);
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


int main(int argc, char **argv, char **env)
{
	int i;
	int nb;

	i= 0;
	argv++;
	while(argv[i++])
	{
		char **tmp = NULL;
		nb = getnbpipe_until(argv, &tmp);

		t_cmd cmd[nb + 1];
		if (*argv)
		{
			getcmd_replacepipe(argv, cmd, nb);
			launchshell(env, cmd);
		}
		if (tmp)
		{
			i = 0;
			argv = tmp;
		}
		else
			return (1);
	}
	return (0);
}