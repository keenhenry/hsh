/**
 * This file implements all the builtin commands for Hank Shell.
 * @author: Henry Huang
 * @date: 08/13/2010
 */

#include "builtins.h"

extern char cwd[];	// can't use char *rel_cwd; i don't know why
extern char rel_cwd[];	// can't use char *rel_cwd; i don't know why
extern struct List dirs_stack;
extern struct List paths_list;

BUILTIN builtins[] = {
    { "exit", "Exit hsh"			   , builtin_exit },
    { "cd", "Change directory"		   	   , builtin_cd },
    { "echo", "Echo command line arguments"	   , builtin_echo },
    { "pwd", "Print current working directory"     , builtin_pwd },
    { "pushd", "Push directory onto stack"	   , builtin_pushd },
    { "popd", "Pop directory out of stack"	   , builtin_popd },
    { "dirs", "Print directories on stack"	   , builtin_dirs },
    { "path", "Modify hsh search directory list"   , builtin_path },
    { "history", "Show command line history"	   , builtin_history },
//    { "kill", "Kill processes"		   	   , builtin_kill },
//    { "jobs", "Show current running jobs under hsh", builtin_jobs },
    { (char*)NULL, (char*)NULL, (hsh_btfunc_t*)NULL }
};

//===================================================================//
// 	     	 						     //
// 	     	 Exception Handling Helper Functions	    	     //
// 	     	 						     //
//===================================================================//

/* cd builtin exception handling
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @return: exception code; 0 for NO EXCEPTION OCCURS */
static int cd_exception_hdlr(int nargs, char **args)
{
	int exception = 0;

	/* check return value of chdir */
	if (nargs == 1 && chdir(getenv("HOME")) < 0)
		exception = 1;
	else if (nargs >= 2 && chdir(args[1]) < 0)
		exception = 2;
	
	/* exception handling */
	if (exception == 1)
		fprintf(stderr, "-hsh: %s: %s\n", 
				args[0], strerror(errno));
	else if (exception == 2)
		fprintf(stderr, "-hsh: %s: %s: %s\n", 
				args[0], args[1], strerror(errno));
	return exception;
}

/* popd builtin exception handling.
 * @args: command line argument buffer
 * @return: exception code; 0 for NO EXCEPTION OCCURS */
static int popd_exception_hdlr(int nargs, char **args)
{
	int exception = 0;

	if (nargs > 2)
		exception = 1;
	/* a hack for atoi() funcion; this case is not an exception! */
	else if (nargs==2 && strcmp(args[1],"-0")==0)
		;
	else if (nargs==2 && atoi(args[1])>=0)
		exception = 2;
	else if (is_empty(&dirs_stack))
		exception = 3;
	else if (nargs==2 && -atoi(args[1])>list_size(&dirs_stack))
		exception = 4;
	
	/* exception handling */
	if (exception == 1)
		fprintf(stderr, "-hsh: %s: too many arguments\nUsage: %s [-n]\n", 
				args[0], args[0]);
	else if (exception == 2)
		fprintf(stderr, "-hsh: %s: %s: invalid option\nUsage: %s [-n]\n", 
				args[0], args[1], args[0]);
	else if (exception == 3)
		fprintf(stderr, "-hsh: %s: directory stack empty\n", args[0]);
	else if (exception == 4)
		fprintf(stderr, "-hsh: %s: %s: number too big\n", args[0], args[1]);

	return exception;
}

/* dirs builtin exception handling
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @return: exception code; 0 for NO EXCEPTION OCCURS */
static int dirs_exception_hdlr(int nargs, char **args)
{
	int exception = 0;

	if (nargs > 1)
		exception = 1;
	
	/* exception handling */
	if (exception == 1)
		fprintf(stderr, "-hsh: %s: too many arguments\nUsage: %s\n",
				args[0], args[0]);
	return exception;
}

/* History builtin exception handling
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @hlist: list of history entries
 * @return: exception code; 0 for NO EXCEPTION OCCURS */
static int his_exception_hdlr(int nargs, char **args, HIST_ENTRY **hlist)
{
	int exception = 0;

	/* check for NO HISTORY */
	if (hlist == NULL)
		exception = -1;
	/* check for # of arguments */
	else if (nargs > 2)
		exception = -2;
	/* a hack for atoi() funcion; this case is not an exception! */
	else if (nargs == 2 && strcmp(args[1], "0")==0)
		;
	/* check for numeric argument */
	else if (nargs == 2 && !atoi(args[1]))
		exception = 1;
	/* check validity of numeric argument */
	else if (nargs == 2 && (atoi(args[1]) > history_length || 
				atoi(args[1]) < 0))
		exception = 2;

	/* exception handling */
	if (exception == -1)
		;
	else if (exception == -2)
		fprintf(stderr, "-hsh: %s: too many arguments\nUsage: %s [n]\n", 
				args[0], args[0]);
	else if (exception == 1)
		fprintf(stderr, "-hsh: %s: %s: numeric argument required\n", 
				args[0], args[1]);
	else if (exception == 2)
		fprintf(stderr, "-hsh: %s: %s: invalid option\n", 
				args[0], args[1]);
	return exception;
}

/* path builtin exception handling
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @return: exception code; 0 for NO EXCEPTION OCCURS */
static int path_exception_hdlr(int nargs, char **args) 
{
	return 0;
}

//===================================================================//
// 	     	 						     //
// 	     	       Builtin Helper Functions	    	     	     //
// 	     	 						     //
//===================================================================//

/* Print a single element on directory stack.
 * @data: the data the element points to */
static void print_stack_element(void *data) 
{
	char *rel_path = (char *) NULL;

	if (strstr((char*)data, getenv("HOME")))
		rel_path = (char*) data + strlen(getenv("HOME"));
	
	if (rel_path)
		printf("~%s ", rel_path);
	else
		printf("%s ", (char *) data);
}

/* History printing function.
 * @n_of_entries: # of history entries to print
 * @hlist: list of history entries */
static void print_history(int n_of_entries, HIST_ENTRY **hlist)
{
	int i;
	for (i = n_of_entries; i > 0; i--)
		printf(" %d  %s\n", history_base + history_length - i,
			hlist[history_length - i]->line);
}

/* Pop directory stack helper funcion. */
static void pop_dirs_stack()
{
	if (chdir(top(&dirs_stack))<0) {
		perror("chdir");
	} else {
		free(top(&dirs_stack));
		pop(&dirs_stack);
	}
}

//===================================================================//
// 	     	 						     //
// 	     	 	Builtin Command Functions	    	     //
// 	     	 						     //
//===================================================================//

/* exit builtin function: exit Hsh program
 * @nargs: # of arguments in command line
 * @args: command line argument buffer 
 * @return: 0 to continue loop; -1 to break */
inline int builtin_exit(int nargs, char **args) { return -1; }

/* cd builtin function: change working directory.
 * @nargs: # of arguments in command line
 * @args: command line argument buffer 
 * @return: -1 to break; otherwise continue loop */
int builtin_cd(int nargs, char **args)
{
	return cd_exception_hdlr(nargs, args);
}

/* echo builtin function: print command line args
 * @nargs: # of arguments in command line
 * @args: command line argument buffer 
 * @return: 0 to continue loop; -1 to break */
int builtin_echo(int nargs, char **args)
{
	int i;
	for (i = 1; i < nargs; i++)
		printf("%s%s", args[i], (i < nargs-1) ? " " : "");
	printf("\n");
	return 0;
}

/* pwd builtin function: print current working directory
 * @nargs: # of arguments in command line
 * @args: command line argument buffer 
 * @return: -1 to break; otherwise continue loop */
inline int builtin_pwd(int nargs, char **args) { return printf("%s\n", cwd); }

/* pushd builtin function: change working directory
 * and then push previous working directory onto stack
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @return: 0 to continue loop; -1 to break */
int builtin_pushd(int nargs, char **args)
{
	char *dir = NULL;

	/* check chdir exceptions */
	if (cd_exception_hdlr(nargs, args))
		return 0;
	
	/* allocate memory for directory name;
	 * the list API does not manage user data! */
	if (!(dir = (char*) malloc(strlen(cwd)+1))) {
		perror("malloc");
		return 0;
	}
	
	memset(dir, 0, 1);
	
	/* push directory to stack; but never push directory
	 * that is identical to top element on the stack */
	if (is_empty(&dirs_stack) || strcmp(top(&dirs_stack), cwd))
		push(&dirs_stack, strcpy(dir, cwd));

	/* print out directory stack */
	list_traversal(&dirs_stack, print_stack_element); 
	printf("<\n");
	return 0;
}

/* popd builtin function: popd directory stack
 * @stack: directory stack holding directory names
 * @args: command line argument buffer
 * @return: 0 to continue loop; -1 to break */
int builtin_popd(int nargs, char **args)
{
	int i;

	/* check exception */
	if (popd_exception_hdlr(nargs, args))
		return 0;

	/* pop directory stack */
	if (nargs == 1) {
		pop_dirs_stack();
	} else {	/* # of args is 2 */
		for (i=0; i<-atoi(args[1]); i++)
			pop_dirs_stack();
	}
	
	/* print out directory stack */
	list_traversal(&dirs_stack, print_stack_element); 
	printf("<\n");
	return 0;
}

/* dirs builtin function: display directory names on stack
 * @nargs: # of arguments in command line
 * @args: command line argument buffer 
 * @return: 0 to continue loop; -1 to break */
int builtin_dirs(int nargs, char **args)
{
	if (dirs_exception_hdlr(nargs, args))
		return 0;
	
	list_traversal(&dirs_stack, print_stack_element);
	printf("<\n");		      // stack top symbol
	return 0;
}

/* history builtin function: show command line history
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @return: 0 to continue loop; -1 to break */
int builtin_history(int nargs, char **args)
{
	HIST_ENTRY **the_list = history_list();

	/* check if exception occurs */
	if (his_exception_hdlr(nargs, args, the_list))
		return 0;

	if (nargs == 1) 	// Only one command line argument 
		print_history(history_length, the_list);
	else if (nargs == 2) 	// Two command line arguments
		print_history(atoi(args[1]), the_list);
	return 0;
}

/* path builtin function: modify hsh search directory list
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @return: 0 to continue loop; -1 to break */
int builtin_path(int nargs, char **args)
{
	return 0;
	/*
	struct stat buf;
	
	if (nargs == 1) {
		show_stack(s, ':');
		if (size(s)) printf("\n");
		return 0;
	}

	if (nargs != 3) {
		fprintf(stderr, "-sh: usage: %s [+|-] [/some/dir]\n", args[0]);
		return 1;	// to indicate error occurs 
	}

	if (!strcmp(args[1], "+")) {
		if (stat(args[2], &buf) < 0) {
			fprintf(stderr, "-sh: path: %s\n", strerror(errno));
			return 1;
		}
		if (find_node(s, args[2]) >= 0) {// detecting duplicated path 
			fprintf(stderr, "-sh: path: %s already in path list\n", args[2]);
			return 1;
		}
		push(s, args[2]);
		return 0;
	}

	if (!strcmp(args[1], "-")) {
		if (is_empty(s)) {
			fprintf(stderr, "-sh: path: path list empty\n");
			return 1;
		}
		if (rm_node(s, find_node(s, args[2])) > 0) {
			fprintf(stderr, "-sh: path: %s is not in path list\n", args[2]);
			return 1;
		}
		return 0;
	}

	fprintf(stderr, "-sh: path: %s: invalid argument\n", args[1]);
	return 1; */	
}

//===================================================================//
// 	     	 						     //
// 	          Hank Shell List Clean-Up Functions	       	     //
// 	     	 						     //
//===================================================================//

/* Application specific list memory reclaim function 
 * @list: the list to be cleaned up */
void list_clean(struct List *list)
{
	while (list->size) {
		free(top(list));	// free node data
		pop(list);		// free node
	}
	list_dtor(list);
}
