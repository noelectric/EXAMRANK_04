#include <libc.h>

int	_error(char *str, char *arg)
{
	while(*str)
		write (2, str++, 1);
	if (arg)
		while (*arg)
			write(2, arg++, 1);
	write (2, "\n", 1);
	return (1);
}

int	ft_execve(char **av, int i, int tmp, char **env)
{
	av[i] = NULL;
	dup2(tmp, 0);
	close(tmp);
	execve(av[0], av, env);
	return (_error("error: cannot execute "), av[0]);
}

int main(int ac, char **av, char **env)
{
	int i = 0;
	int tmp_fd = dup(0);
	int fd[2];
	(void)ac;

	while (av[i] && av[i + 1])
	{
		av = &av[i + 1];
		i = 0;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		if (strcmp(av[0], "cd") == 0)
		{
			if (i != 2)
				_error("error: cd: bad arguments ", NULL);
			else if (chdir(var[1]) != 0)
				_error("error: cd: cannot change directory to ", av[1]);
		}	
		else if (i != 0 && (av[i] == NULL || strcmp(av[i], ";") == 0))
		{
			if (fork() == 0)
				if (ft_execve(av, i, tmp_fd, env))
					return 1;
			else
			{
				close(tmp_fd);
				while (waitpid(-1, NULL, 0) != -1)
					;
				tmp_fd = dup(0);
			}
		}
		else if (i != 0 && strcmp(av[i], "|") == 0)
		{
			pipe(fd);
			if (fork() == 0)
			{
				dup2(fd[1], 1);
				close(fd[0]);
				close(fd[1]);
				if (ft_execve(av, i, tmp_fd, env))
					return 1;
			}
			else
			{
				close(fd[1]);
				close(tmp_fd);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return 0;
}