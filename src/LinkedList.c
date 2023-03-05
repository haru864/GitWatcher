#include "include/LinkedList.h"

static struct Node *getNodeHavingSamePath(struct LinkedList *list, char *path);

struct LinkedList *init_linkedlist()
{
	struct LinkedList *list = (struct LinkedList *)malloc(sizeof(struct LinkedList));
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

void delete_linkedlist(struct LinkedList *list)
{
	free(list);
	list = NULL;
}

// success -> return number of nodes
// failure -> return -1
int insert_node(struct LinkedList *list, char *path)
{
	if (list->size > 0 && getNodeHavingSamePath(list, path) != NULL)
	{
		return -1;
	}

	struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
	if (!newNode)
	{
		perror("malloc");
		exit(1);
	}
	strcpy(newNode->path, path);
	newNode->next = newNode->prev = NULL;

	if (list->head == NULL && list->tail == NULL)
	{
		list->head = list->tail = newNode;
	}
	else
	{
		list->tail->next = newNode;
		newNode->prev = list->tail;
		list->tail = newNode;
	}

	list->size++;

	return list->size;
}

// success -> return number of nodes
// failure -> return -1
int delete_node(struct LinkedList *list, char *path)
{
	struct Node *nodeToDelete = getNodeHavingSamePath(list, path);

	if (!nodeToDelete)
	{
		return -1;
	}

	if (list->head == list->tail)
	{
		list->head = list->tail = NULL;
		list->size = 0;
	}
	else
	{
		nodeToDelete->prev->next = nodeToDelete->next;
		nodeToDelete->next->prev = nodeToDelete->prev;
		free(nodeToDelete);
	}
}

void printAllNode(struct LinkedList *list)
{
	struct Node *currentNode;

	if (!list)
	{
		return;
	}

	currentNode = list->head;

	while (currentNode != NULL)
	{
		printf("%s\n", currentNode->path);
		currentNode = currentNode->next;
	}
}

static struct Node *getNodeHavingSamePath(struct LinkedList *list, char *path)
{
	struct Node *nodeHavingSamePath = NULL;
	struct Node *currentNode;

	if (!list || !path)
	{
		return NULL;
	}

	currentNode = list->head;

	while (currentNode != NULL)
	{
		if (strcmp(currentNode->path, path) == 0)
		{
			nodeHavingSamePath = currentNode;
			break;
		}
		currentNode = currentNode->next;
	}

	return nodeHavingSamePath;
}
