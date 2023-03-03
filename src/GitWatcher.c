#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "include/process.h"

static void searchDirectory(const char *);
static bool isDirectory(const struct stat *);
static bool isGitRepository(void);
// static bool hasRemoteRepository(const char *);
// static bool hasNoPushedCommits(const char *);

int main(int argc, char **argv)
{
	char target_directory[PATH_MAX];

	if (argc != 2)
	{
		fprintf(stderr, "usage: GitWatcher <dir-name>\n");
		exit(EXIT_FAILURE);
	}

	strcpy(target_directory, argv[1]);
	// fprintf(stderr, "dest: %s\n", target_directory);

	searchDirectory(target_directory);

	return 0;
}

static void searchDirectory(const char *path)
{
	char cwd_before_moving[PATH_MAX];
	char cwd_after_moving[PATH_MAX];
	DIR *dirp;
	struct dirent *p;
	struct stat sb;

	if (getcwd(cwd_before_moving, sizeof(cwd_before_moving)) == NULL)
	{
		perror("getcwd");
		exit(EXIT_FAILURE);
	}
	// fprintf(stderr, "before: %s\n", cwd_before_moving);

	if (chdir(path) == -1)
	{
		perror("chdir");
		exit(EXIT_FAILURE);
	}

	if (getcwd(cwd_after_moving, sizeof(cwd_after_moving)) == NULL)
	{
		perror("getcwd");
		exit(EXIT_FAILURE);
	}
	// fprintf(stderr, "after: %s\n", cwd_after_moving);

	if ((dirp = opendir(cwd_after_moving)) == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	errno = 0;
	while ((p = readdir(dirp)) != NULL)
	{
		if (isGitRepository() == true)
		{
			char buf[256];
			getcwd(buf, sizeof(buf));
			printf("%s\n", buf);
			break;
		}

		if (strcmp(p->d_name, ".") == 0 || strcmp(p->d_name, "..") == 0)
		{
			continue;
		}

		if (lstat(p->d_name, &sb) != 0)
		{
			perror("lstat");
			exit(EXIT_FAILURE);
		}

		if (isDirectory(&sb) == false)
		{
			continue;
		}

		searchDirectory(p->d_name);
	}

	if (errno != 0)
	{
		perror("readdir");
		exit(EXIT_FAILURE);
	}

	if (closedir(dirp) != 0)
	{
		perror("closedir");
		exit(EXIT_FAILURE);
	}

	if (chdir(cwd_before_moving) == -1)
	{
		perror("chdir");
		exit(EXIT_FAILURE);
	}
}

static bool isDirectory(const struct stat *sb)
{
	return ((sb->st_mode & __S_IFMT) == __S_IFDIR);
}

static bool isGitRepository(void)
{
	FILE *fp;
	bool isGitRepo = true;
	char buf[256];
	char *message = "fatal: not a git repository";

	fp = popen_err("git log");

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (strncmp(buf, message, strlen(message)) == 0)
		{
			isGitRepo = false;
		}
	}

	pclose_err(fp);

	return isGitRepo;
}
