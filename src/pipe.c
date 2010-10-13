/**
 * This file is the pipe interface for Hank Shell.
 * @author: Henry Huang
 * @date: 09/22/2010
 */

#include "hsh.h"

extern PS_INFO *arr_ps_infos;
extern int single_threaded_cmd(int *pnargs, char **args, int mode);

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

    /* check for exceptions */
    exception = (!strcmp("|", args[0]))? 1 : 0;
    exception = (!exception && !strcmp("|", args[nargs-1]))? 2 : 0;
    
    for (i = 0; !exception && i<(nargs-1); i++)
	if ((!strcmp(args[i], "|") && !strcmp(args[i+1], "|"))
	 || (!strcmp(args[i], "|") && !strcmp(args[i+1], ">"))
	 || (!strcmp(args[i], ">") && !strcmp(args[i+1], "|"))
	 || (!strcmp(args[i], "|") && !strcmp(args[i+1], "<"))
	 || (!strcmp(args[i], "<") && !strcmp(args[i+1], "|")))
	    exception = 3;

    /* exception handling */
    if (exception == 1)
	fprintf(stderr, "-hsh: syntax error near unexpected token '%s'\n", args[0]);
    else if (exception == 2)
	fprintf(stderr, "-hsh: syntax error near unexpected token '%s'\n", args[nargs-1]);
    else if (exception == 3)
	fprintf(stderr, "-hsh: syntax error near unexpected token '%s'\n", args[i]);
    else
	exception = io_exception_hdlr(nargs, args);
    return exception;
}

//===================================================================//
// 	     	 						     //
// 	     	    Command Line Parsing Interface		     //
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

//===================================================================//
// 	     	 						     //
// 	     	    	   Pipeline Interface		     	     //
// 	     	 						     //
//===================================================================//

/* Closing all pipes open for IPC.
 * @pipes: array of pipes 
 * @n_of_th: number of processes/threads created
 * @return: 0 if creation of pipes succeeded; 
 * 	    otherwise return -1 */
int set_pipes(int (*pipes)[2], int n_of_th)
{
    int i, rel = 0;

    /* n_of_th processes need (n_of_th - 1) pipes */
    for (i = 0; i < n_of_th-1; ++i) {
	if (-1 == (rel = pipe(pipes[i]))) {
	    perror("pipe");
	    break;
	}
    }
    return rel;
}

/* Closing all pipes open for IPC.
 * @pipes: array of pipes 
 * @n_of_th: number of threads/processes in the pipeline */
void close_pipes(int (*pipes)[2], int n_of_th)
{
    int i;
    for (i = 0; i < n_of_th-1; ++i) {
	close(pipes[i][0]);	/* close read end of ith pipe */
	close(pipes[i][1]); 	/* close write end of ith pipe */
    }
}

/* Wait for the first child process in the processes chain.
 * @pid: process id of the first child process
 * @return: the return value of waitpid() function */
int wait_first_child(pid_t pid)
{
    int rel;
    if (-1 == (rel = waitpid(pid, NULL, 0)))
    	perror("waitpid");
    return rel;
}

/* Connect pipe read end to stdin.
 * @pipes: array of pipe file descriptors
 * @idx: the pipe index we are dup-ing
 * @n_of_th: number of threads/processes in the pipeline
 * @return: the return value of dup2 system call */
int dup_pipe_read(int (*pipes)[2], int idx, int n_of_th)
{
    int rel; 
    if (-1 == (rel = dup2(pipes[idx][0], STDIN_FILENO)))
    	perror("dup2");
    close_pipes(pipes, n_of_th);
    return rel;
}

/* Connect pipe write end to stdout.
 * @pipes: array of pipe file descriptors 
 * @idx: the pipe index we are dup-ing
 * @n_of_th: number of threads/processes in the pipeline
 * @return: the return value of dup2 system call */
int dup_pipe_write(int (*pipes)[2], int idx, int n_of_th)
{
    int rel; 
    if (-1 == (rel = dup2(pipes[idx][1], STDOUT_FILENO)))
    	perror("dup2");
    close_pipes(pipes, n_of_th);
    return rel;
}

/* Connect pipe write end to stdout and pipe read end to stdin.
 * @pipes: array of pipe file descriptors
 * @idx: the process index
 * @n_of_th: number of threads/processes in the pipeline
 * @return: the return value of dup2 system call */
int dup_pipe_read_write(int (*pipes)[2], int idx, int n_of_th)
{
    int rel; 
    if (-1 == (rel = dup2(pipes[n_of_th-1-idx][0], STDIN_FILENO)) ||
	-1 == (rel = dup2(pipes[n_of_th-2-idx][1], STDOUT_FILENO)))
    	perror("dup2 read write");
    close_pipes(pipes, n_of_th);
    return rel;
}

/* Setup pipes for a single process run the process in the pipeline.
 * @n_of_th: number of threads in the line
 * @i: index of loop; index of threads in arr_ps_infos */
void pipe_process(int n_of_th, int i, int (*pipes)[], pid_t pid)  
{
    if (i == n_of_th) {	/* parent of all processes */
	close_pipes(pipes, n_of_th);
    	usleep(50000);
	wait_first_child(pid); 
    } else if (i == n_of_th - 1) {   /* first child in the chain */
	if (-1 == dup_pipe_read(pipes, 0, n_of_th)) _exit(EXIT_FAILURE);
    	single_threaded_cmd(&arr_ps_infos[i].argc, arr_ps_infos[i].argv, TRUE);
    } else if (i == 0) {    /* last child in the chain gets here */
	if (-1 == dup_pipe_write(pipes, n_of_th-2, n_of_th)) _exit(EXIT_FAILURE);
    	single_threaded_cmd(&arr_ps_infos[i].argc, arr_ps_infos[i].argv, TRUE);
    } else {    /* processes in the middle of the chain get here */
	if (-1 == dup_pipe_read_write(pipes, i, n_of_th)) _exit(EXIT_FAILURE);
    	single_threaded_cmd(&arr_ps_infos[i].argc, arr_ps_infos[i].argv, TRUE);
    }	
}
