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

static void search_directory(const char *);
static void print_file_information(struct stat *, char *);

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

	search_directory(target_directory);

	return 0;
}

static void search_directory(const char *path)
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
	fprintf(stderr, "before: %s\n", cwd_before_moving);

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
	fprintf(stderr, "after: %s\n", cwd_after_moving);

	if ((dirp = opendir(cwd_after_moving)) == NULL)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}

	errno = 0;
	while ((p = readdir(dirp)) != NULL)
	{
		fprintf(stderr, "%s\n", p->d_name);
		if (lstat(p->d_name, &sb) != 0)
		{
			perror("lstat");
			exit(EXIT_FAILURE);
		}
		print_file_information(&sb, p->d_name);
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
}

static void print_file_information(struct stat *sb, char *path)
{
	fprintf(stderr, "Information for %s:\n", path);
	fprintf(stderr, "  st_ino   = %d\n", (int)sb->st_ino);
	fprintf(stderr, "  st_mode  = %d\n", (int)sb->st_mode);
}
