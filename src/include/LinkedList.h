#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <linux/limits.h>

struct Node
{
	struct Node *next;
	struct Node *prev;
	char path[PATH_MAX];
};

struct LinkedList
{
	struct Node *head;
	struct Node *tail;
	int size;
};

struct LinkedList *init_linkedlist();
void delete_linkedlist(struct LinkedList *list);
int insert_node(struct LinkedList *list, char *path);
int delete_node(struct LinkedList *list, char *path);
void printAllNode(struct LinkedList *list);

#endif
