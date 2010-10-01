/**
 * This file is the pipe interface for Hank Shell.
 * @author: Henry Huang
 * @date: 09/22/2010
 */

#include "hsh.h"

extern PS_INFO *arr_ps_infos;

//===================================================================//
// 	     	 						     //
// 	     	    	Pipeline Helper Functions		     //
// 	     	 						     //
//===================================================================//

/* Check for command line syntax errors.
 * @nargs: # of arguments
 * @args: command line argument buffer
 * @return: 0 if no exceptions found otherwise non-zero */
int pipe_exception_hdlr(int nargs, char **args)
{
    int i, exception = 0;

    for (i = 0; i < nargs-1; i++) {
    	if ((i == 0 && !strcmp("|", args[i])) || (i==nargs-2 && !strcmp("|", args[i+1]))) {
	    exception = 1;
	    break;
	} else if ((!strcmp(args[i], "|") && !strcmp(args[i+1], "|")) ||
	    (!strcmp(args[i], "|") && !strcmp(args[i+1], "<")) ||
	    (!strcmp(args[i], "|") && !strcmp(args[i+1], ">")) ||
	    (!strcmp(args[i], ">") && !strcmp(args[i+1], "|")) ||
	    (!strcmp(args[i], "<") && !strcmp(args[i+1], "|"))) {
	    exception = 2;
	    break;
	}
    }

    /* exception handling */
    if (exception == 1 || exception == 2)
	fprintf(stderr, "-hsh: syntax error near unexpected token '%s'\n", args[i+1]);
	
    return exception;
}

//===================================================================//
// 	     	 						     //
// 	     	    	   Pipeline Interface		     	     //
// 	     	 						     //
//===================================================================//

/* Count the number of processes needed to be forked
 * @args: command line tokens including pipe symbols
 * @return: the # of processes; 1 if only one process */
int count_processes(char **args) 
{
    int i, count = 1;

    /* count the # of '|' symbols */
    for (i = 0; args[i]; i++)
	if (!strcmp(args[i], "|")) ++count;

    /* return # of processes */
    return count;
}

/* Allocate memory for processes' argument lists.
 * @num_of_ps: number of processes */
void prepare_arg_lists(int num_of_ps)
{
    int i;

    if (arr_ps_infos) {
	free(arr_ps_infos);
	arr_ps_infos = (PS_INFO *) NULL;
    }

    if (num_of_ps) {
    	arr_ps_infos = (PS_INFO *) malloc(num_of_ps*sizeof(PS_INFO));
    	if (!arr_ps_infos) 
	    die_with_error("malloc");

	/* initialize arr_ps_infos */
	for (i = 0; i < num_of_ps; i++) {
	    arr_ps_infos[i].argc = 0;
	    arr_ps_infos[i].argv = (char **) NULL;
	}
    }
}

/* Setting up PS_INFO for each process.
 * @num_of_ps: # of processes
 * @args: command line argument list */
void set_ps_infos(int num_of_ps, char **args)
{
    int i , j, head;

    for (i = 0, j = 0, head = 0; args[i]; i++) {
	if (!strcmp(args[i], "|")) {
	    arr_ps_infos[j].argc = i - head;
	    arr_ps_infos[j].argv = &args[head];
	    args[i] = (char *) NULL;
	    head = i + 1;
    	    j++;
	}
    }

    /* the last process arg list info */
    arr_ps_infos[j].argc = i - head;
    arr_ps_infos[j].argv = &args[head];
}

/* Clear memory allocated for arr_ps_infos.
 * @array: array of ps_info structures */
void clear_ps_infos(PS_INFO *array)
{
    if (array) free(array);
}
