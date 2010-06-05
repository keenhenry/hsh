/* list.c: generic implementation of a list */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/**
 * a 'private' function used exclusively inside this file
 * for output error messages 
 **/
static void die(const char *msg)
{
	perror(msg);
	exit(1);
} 

void list_init(struct List *list)
{
	struct Node *dum_head = (struct Node*) malloc(sizeof(struct Node));
	struct Node *dum_tail = (struct Node*) malloc(sizeof(struct Node));

	if (dum_head == NULL || dum_tail == NULL) die("malloc");

	dum_head->next = dum_head->prev = dum_tail;
	dum_tail->prev = dum_tail->next = dum_head;

	list->head = dum_head;
	list->tail = dum_tail;
	list->back = list->front = NULL;
	list->size = 0;
}

inline void clear(struct List *list)
{
	while (list->size) pop(list);
}

void list_dtor(struct List *list)
{
	clear(list);
	free(list->head);
	free(list->tail);
	list->back = list->front = NULL;
	list->size = -1;
}

void push(struct List *list, DATATYPE *data)
{
	struct Node *tmp = (struct Node*) malloc(sizeof(struct Node));
	
	if (tmp == NULL) 
		die("malloc");

	tmp->data = data;
	tmp->next = list->tail;
	tmp->prev = list->tail->prev; 
	tmp->prev->next = tmp;
	tmp->next->prev = tmp;

	list->back = tmp;
	if (list->size == 0) list->front = tmp;
	list->size++;	
}

void pop(struct List *list)
{
	struct Node *curr = list->back;
	
	if (list->size <= 0) {
		fprintf(stderr, "Stack empty\n");
		return;
	}

	curr->prev->next = curr->next;
	curr->next->prev = curr->prev;
	if (list->size == 1) 
		list->front = list->back = NULL;
	list->back = curr->prev;
	free(curr);

	list->size--;
}

DATATYPE* top(const struct List *list)
{
	if (list->size <= 0) {
		fprintf(stderr, "Stack empty\n");
		return NULL;
	}

	return (DATATYPE*)(list->back->data);
}

void push_front(struct List *list, DATATYPE *data)
{
	struct Node *tmp = (struct Node*) malloc(sizeof(struct Node));
	
	if (tmp == NULL) 
		die("malloc");

	tmp->data = data;
	tmp->next = list->head->next;
	tmp->prev = list->head; 
	tmp->prev->next = tmp;
	tmp->next->prev = tmp;

	list->front = tmp;
	if (list->size == 0) list->back = tmp;
	list->size++;	
}

void pop_front(struct List *list)
{
	struct Node *curr = list->front;
	
	if (list->size <= 0) {
		fprintf(stderr, "Stack empty\n");
		return;
	}

	curr->prev->next = curr->next;
	curr->next->prev = curr->prev;
	if (list->size == 1) 
		list->front = list->back = NULL;
	list->front = curr->next;
	free(curr);

	list->size--;
}

void push_back(struct List *list, DATATYPE *data)
{
	push(list, data);
}

void pop_back(struct List *list)
{
	pop(list);
}

DATATYPE* front(const struct List *list)
{
	if (list->size <= 0) {
		fprintf(stderr, "Stack empty\n");
		return NULL;
	}

	return (DATATYPE*)(list->front->data);
}

DATATYPE* back(const struct List *list)
{
	return top(list);
}

int find_index(const struct List *list, int (*compare)(const void *e, const void *v), DATATYPE *value)
{
	struct 	Node *tmp 	= list->front;
	int 	index 		= -1;		
	
	if (is_empty(list))
		return index;

	while (tmp != list->tail) {
		++index;
		if (!compare(tmp->data, value)) 
			return index;
		tmp = tmp->next;
	}

	return -1;	/* no matched element found */	
}

struct Node *find_node(const struct List *list, int (*compare)(const void *e, const void *v), DATATYPE *value)
{
	struct 	Node *tmp 	= list->front;
	
	if (is_empty(list))
		return NULL;

	while (tmp != list->tail) {
		if (!compare(tmp->data, value)) 
			return tmp;
		tmp = tmp->next;
	}

	return NULL;	/* no matched element found */	
}

int remove_at_idx(struct List *list, int index)
{
	struct 	Node *tmp = list->front;
	int 	i = 0;
	
	if (is_empty(list) || index >= list_size(list) || index < 0) 
		return 1;	/* Not found */

	while (i++ < index) 
		tmp = tmp->next;

	if (index == 0) {	/* the element to be removed is the first element */
		pop_front(list);
	} else	if (index == list_size(list) - 1) {	/* last element is to be removed */
		pop(list);
	} else {		/* element to be removed is in the middle */
		tmp->next->prev = tmp->prev;
		tmp->prev->next = tmp->next;
		list->size--;
		free(tmp);
	}

	return 0;	/* remove successfully */
}

void list_traversal(struct List *list, void (*f)(void *a))
{
	struct Node *tmp = list->front;
	
	while(tmp != NULL && tmp != list->tail) {
		f(tmp->data);
		tmp = tmp->next;
	}
}	
