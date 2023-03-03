#include "include/process.h"

FILE *popen_err(const char *command)
{
	FILE *fp = NULL;
	pid_t ppid, cpid;
	int status;
	int fd[2];
	int nbytes;
	char buf[256];
	int devNull = open("/dev/null", O_WRONLY);

	if (pipe(fd) == -1)
	{
		perror("pipe");
		exit(1);
	}

	ppid = getpid();
	fflush(NULL);

	if ((cpid = fork()) == -1)
	{
		perror("fork");
		exit(1);
	}
	else if (cpid == 0)
	{
		close(fd[0]);
		dup2(devNull, STDOUT_FILENO);
		dup2(fd[1], STDERR_FILENO);
		execlp("sh", "sh", "-c", command, NULL);
		close(fd[1]);
		exit(0);
	}

	if (wait(&status) == (pid_t)-1)
	{
		perror("wait");
		exit(1);
	}

	close(fd[1]);
	fp = fdopen(fd[0], "r");

	return fp;
}

void pclose_err(FILE *stream)
{
	if (fclose(stream) == EOF)
	{
		perror("fclose");
		exit(1);
	}
}
