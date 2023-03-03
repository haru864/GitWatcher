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
#include <malloc.h>
#include "include/process.h"

static void searchDirectory(const char *path);
static bool isDirectory(const struct stat *sb);
static bool isGitRepository(void);
static bool hasRemoteRepository(void);
static bool isRemoteUpdated(void);
static char *getCurrentBranch(void);
static char *getRemoteBranch(char *localBranchName);

// debug
static void printfHexa(char *);

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
		if (isGitRepository() == true && hasRemoteRepository() == false)
		{
			char buf[256];
			getcwd(buf, sizeof(buf));
			printf("no remotes => %s\n", buf);
			break;
		}

		isRemoteUpdated();

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

static bool hasRemoteRepository()
{
	FILE *fp;
	bool hasRemoteRepo = false;
	char buf[256];
	char *message = "  remotes/";

	fp = popen("git branch -a", "r");

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (strncmp(buf, message, strlen(message)) == 0)
		{
			hasRemoteRepo = true;
		}
	}

	pclose(fp);

	return hasRemoteRepo;
}

static bool isRemoteUpdated(void)
{
	FILE *fp;
	char buf[256];
	char *currentBranchName = getCurrentBranch();
	char *remoteBranchName = getRemoteBranch(currentBranchName);
	char *command_fetch_arg = (char *)malloc(malloc_usable_size(remoteBranchName));
	char *command_fetch = NULL;
	char *command_revlist = NULL;
	printf("%s", currentBranchName);
	printf("%s", remoteBranchName);

	strcpy(command_fetch_arg, &remoteBranchName[8]);
	for (int i = 0; i < strlen(command_fetch_arg); i++)
	{
		if (command_fetch_arg[i] == '/')
		{
			command_fetch_arg[i] = ' ';
		}
	}
	printf("%s", command_fetch_arg);

	command_fetch = (char *)malloc(malloc_usable_size(command_fetch_arg) + sizeof("git fetch "));
	snprintf(command_fetch, malloc_usable_size(command_fetch), "git fetch %s", command_fetch_arg);

	fp = popen(command_fetch, "r");
	// fp = popen(command_revlist, "r");

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		printf(" => %s\n", buf);
	}

	// pclose(fp);
	free(currentBranchName);
	free(remoteBranchName);
	free(command_fetch_arg);
	free(command_fetch);
	// free(command_revlist);

	return false;
}

static char *getCurrentBranch(void)
{
	FILE *fp;
	char buf[256];
	char *markerOfCurrentBranch = "* ";
	char *currentBranchName;

	fp = popen("git branch", "r");

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		if (strncmp(buf, markerOfCurrentBranch, strlen(markerOfCurrentBranch)) == 0)
		{
			int len = strlen(buf);
			currentBranchName = (char *)malloc(sizeof(len));
			snprintf(currentBranchName, len, "/%s\n", &buf[2]);
		}
	}

	pclose(fp);

	return currentBranchName;
}

static char *getRemoteBranch(char *localBranchName)
{
	FILE *fp;
	char buf[256];
	char *remoteBranchName = NULL;

	fp = popen("git branch -a", "r");

	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		bool isSame = true;
		int idx = 0;
		if (strlen(buf) < strlen(localBranchName))
		{
			continue;
		}
		for (int i = strlen(buf) - strlen(localBranchName), j = 0; i < strlen(buf); i++, j++)
		{
			if (buf[i] != localBranchName[j])
			{
				isSame = false;
				idx = strlen(buf) - strlen(localBranchName);
				break;
			}
		}
		if (isSame == true)
		{
			remoteBranchName = (char *)malloc(sizeof(buf) - 1);
			strcpy(remoteBranchName, &buf[2]);
		}
	}

	pclose(fp);

	return remoteBranchName;
}

static void printfHexa(char *s)
{
	for (int i = 0; i < strlen(s); i++)
	{
		printf("%x ", s[i]);
	}
	printf("\n");
}
