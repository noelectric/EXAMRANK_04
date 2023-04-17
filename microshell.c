#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int i_error(char *str, char *arg)
{
	while (*str)
		write(2, str++, 1);
	if (arg)
		while (*arg)
		 write(2, arg++, 1);
	write(2, "\n", 1);
	return 1;
}

int i_exec(char **av, int tmp, char **env)
{
	av[0] = NULL;
	dup2(tmp, 0);
	close(tmp);
	execve(av[0], av, env);
	return(i_error("error : cannot excute", av[0]));
}

int main(int ac, char **av, char **env)
{
	int i = 0;
	int tmp_fd;
	int fd[2];
	(void)ac;

	while(av[i] && av[i + 1])
	{
		av = &av[i + 1];
		i = 0;
		while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
			i++;
		if(!strcmp(av[0], "cd"))
		{
			if (i != 2)
				i_error("error: cd: bad arguments", NULL);
			else if (chdir(av[1]) != 0)
				i_error("error: cd: cannot change directory to ", av[1]);
		}
		else if (i != 0 && (av[i] != 0 || strcmp(av[i], ";") == 0))
		{
			if (fork() == 0)
			{
				if (i_exec(av, tmp_fd, env))
					return 1;
			}
			else
			{
				close(tmp_fd);
				while (waitpid(-1, NULL, 0))
					;
				tmp_fd = dup(0);
			}
		}
		else if(i != 0 && strcmp(av[i], "|"))
		{
			pipe(fd);
			if(fork() == 0)
			{
				dup2(fd[1], 1);
				close(fd[1]);
				close(tmp_fd);
				if (i_exec(av, tmp_fd, env))
					return 1;				
			}
			else
			{
				close(fd[1]);
				close (tmp_fd);
				tmp_fd = fd[0];
			}
		}
	}
	close(tmp_fd);
	return 0;
}