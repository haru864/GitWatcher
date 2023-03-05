#include "include/process.h"

#define R 0
#define W 1

FILE *popen_err(const char *command)
{
	FILE *fp = NULL;
	pid_t ppid, cpid;
	int status;
	int nbytes;
	int fd[2];
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
		close(fd[R]);
		close(fd[W]);
		perror("fork");
		exit(1);
	}
	else if (cpid == 0)
	{
		close(fd[R]);
		dup2(devNull, STDOUT_FILENO);
		dup2(fd[W], STDERR_FILENO);
		execlp("sh", "sh", "-c", command, NULL);
		close(fd[W]);
		close(devNull);
		exit(0);
	}

	if (wait(&status) == (pid_t)-1)
	{
		perror("wait");
		exit(1);
	}

	close(fd[W]);
	close(devNull);
	fp = fdopen(fd[R], "r");

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
