/**
 * This is a list API that supports both 
 * stack and deque operations. The underlining
 * implementation is a cicular doubly linked list.
 * @author: Henry Huang
 * @date: 02/02/2010
 **/

#ifndef __LIST_H__
#define __LIST_H__

typedef double DATATYPE;		/* the type of data a node will hold */

/**
 * A List consists of three pointers:
 * @head: point to the head of a list
 * @tail: point to the tail of a list
 **/
struct List {
	struct Node *head;
	struct Node *tail;
	struct Node *back;	/* point to the last element in the list */
	struct Node *front;	/* point to the first element in the list */
	int size;
};

/**
 * The structure of a Node
 **/
struct Node {
	void *data;		/* this list does not manage user data */
	struct Node *next;
	struct Node *prev;
};

/*--- COMMON OPERATIONS ---*/

/**
 * list_init(): Initialization of List structure
 * @list: a pointer to a List struct
 * @return: void
 **/
void list_init(struct List* list);

/**
 * is_empty(): is this list empty? 
 * @list: a pointer to a List struct
 * @return: non-zero if the list is empty 
 **/
static inline int is_empty(const struct List *list)
{
	return (list->size == 0);
}

/**
 * list_size(): return the size of list 
 * @list: a pointer to a List struct
 * @return: # of nodes in a list  
 **/
static inline int list_size(const struct List *list)
{
	return list->size;
}

/**
 * clear(): remove all the nodes except head and tail
 * @list: a pointer to a List struct
 **/
void clear(struct List *list);

/**
 * list_dtor(): remove the whole list
 * @list: a pointer to a List struct
 **/
void list_dtor(struct List *list);

/*--- STACK OPERATIONS ---*/
void push(struct List *list, DATATYPE *data);
void pop(struct List *list);	

/**
 * top(): peek at the content of top of stack 
 * @list: a pointer to a List struct
 * @return: the reference to the data of top element  
 **/
DATATYPE* top(const struct List *list);

/*--- DEQUE OPERATIONS ---*/
void push_front(struct List *list, DATATYPE *data);
void pop_front(struct List *list);
void push_back(struct List *list, DATATYPE *data);
void pop_back(struct List *list);
DATATYPE* front(const struct List *list);
DATATYPE* back(const struct List *list);


/*--- LIST OPERATIONS ---*/
/**
 * find_idex() finds the index of a list element that stores data with value 'value'
 * the usage is similar to qsort in standard C library
 * @list: the list to be sought
 * @compare: a user-defined comparison function; 
 * - return -1 if element is less than value
 * - return 0 if element is equal to value
 * - return +1 if element is bigger thatn value
 * @return: return the index of matched node; -1 if not found
 * index is zero-based!    
 **/
int find_index(const struct List *list, int (*compare)(const void *element, const void *value), DATATYPE *value);

/**
 * find_node() finds the reference to a list element that stores data with value 'value'
 * the usage is similar to qsort in standard C library
 * @list: the list to be sought
 * @compare: a user-defined comparison function; 
 * - return -1 if element is less than value
 * - return 0 if element is equal to value
 * - return +1 if element is bigger thatn value
 * @return: return the reference of that matched node; NULL if not found
 **/
struct Node *find_node(const struct List *list, int (*compare)(const void *element, const void *value), DATATYPE *value);

/**
 * remove_at_idx(): remove a node whose index is 'index'
 * @list: the list to be removed
 * @index: the index of the node to be removed; index is zero-based
 * @return: 0 if removed successfully; 1 if not found or not a valid index
 **/
int remove_at_idx(struct List *list, int index);

/**
 * list_traversal(): traverse the list and apply a function f to each element
 * @list: the list to be traversed
 * @(*f): the function to be applied on a single element
 **/
void list_traversal(struct List *list, void (*f)(void *a));
#endif
