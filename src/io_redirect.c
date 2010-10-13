/**
 * This file is I/O redirection interface for Hank Shell.
 * @author: Henry Huang
 * @date: 09/15/2010
 */

#include "hsh.h"

//===================================================================//
// 	     	 						     //
// 	     	 	Global Data Structures			     //
// 	     	 						     //
//===================================================================//

int stdin_save = -1, stdout_save = -1, stderr_save = -1;

//===================================================================//
// 	     	 						     //
// 	     	    IO Redirection Helper Functions		     //
// 	     	 						     //
//===================================================================//

/* Check for command line syntax errors.
 * @nargs: # of arguments
 * @args: command line argument buffer
 * @return: 0 if no exceptions found otherwise non-zero */
int io_exception_hdlr(int nargs, char **args)
{
    int i, exception = 0;

    /* check for exceptions */
    if (nargs==1 && (!strcmp(args[0], ">")  || !strcmp(args[0], "<") || 
	    	     !strcmp(args[0], "1>") || !strcmp(args[0], "2>")))
	    exception = 1;
    if (!strcmp(args[nargs-1], ">") || !strcmp(args[nargs-1], "<"))
	    exception = 2;
    for (i = 0; !exception && i<(nargs-1); i++) // i must start with 0
	if ((!strcmp(args[i], ">") && !strcmp(args[i+1], ">")) ||
	    (!strcmp(args[i], ">") && !strcmp(args[i+1], "<")) ||	
	    (!strcmp(args[i], "<") && !strcmp(args[i+1], ">")) ||
	    (!strcmp(args[i], "<") && !strcmp(args[i+1], "<")) ||
	    (!strcmp(args[i], "2>") && !strcmp(args[i+1], "2>")))
	    exception = 3;

    /* exception handling */
    if (exception == 1)
	fprintf(stderr, "-hsh: syntax error near unexpected token 'newline'\n");
    else if (exception == 2)
	fprintf(stderr, "-hsh: syntax error near unexpected token '%s'\n", args[nargs-1]);
    else if (exception == 3)
	fprintf(stderr, "-hsh: syntax error near unexpected token '%s'\n", args[i]);
    return exception;
}

/* Set stdin to file
 * @pfd: pointer to file descriptor of that file
 * @pathname: file pathname which stdin redirects to
 * @return: 0 if no errors otherwise -1 */
int redirect_stdin(int *pfd, char *pathname)
{
    if (-1 == (*pfd = open(pathname, O_RDONLY)))
	fprintf(stderr, "-hsh: %s: %s\n", pathname, strerror(errno));
    else if (dup2(*pfd, STDIN_FILENO) != STDIN_FILENO)
	perror("dup2 error for stdin");
    else
	close(*pfd);
    return (errno) ? -1 : 0;
}

/* Set stdout to file
 * @pfd: pointer to file descriptor of that file
 * @pathname: file pathname which stdout redirects to
 * @return: 0 if no errors otherwise -1 */
int redirect_stdout(int *pfd, char *pathname)
{
    if (-1 == (*pfd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0666)))
	fprintf(stderr, "-hsh: %s: %s\n", pathname, strerror(errno));
    else if (dup2(*pfd, STDOUT_FILENO) != STDOUT_FILENO)
	perror("dup2 error for stdout");
    else
	close(*pfd);
    return (errno) ? -1 : 0;
}

/* Redirect stdout to file for append
 * @pfd: pointer to file descriptor of that file
 * @pathname: file pathname which stdout redirects to
 * @return: 0 if no errors otherwise -1 */
int redirect_stdout_append(int *pfd, char *pathname)
{
    if (-1 == (*pfd = open(pathname, O_WRONLY | O_CREAT | O_APPEND, 0666)))
	fprintf(stderr, "-hsh: %s: %s\n", pathname, strerror(errno));
    else if (dup2(*pfd, STDOUT_FILENO) != STDOUT_FILENO)
	perror("dup2 error for stdout");
    else
	close(*pfd);
    return (errno) ? -1 : 0;
}

/* Set stderr to file
 * @pfd: pointer to file descriptor of that file
 * @pathname: file pathname which stderr redirects to
 * @return: 0 if no errors otherwise -1 */
int redirect_stderr(int *pfd, char *pathname)
{
    if (-1 == (*pfd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0666)))
	fprintf(stderr, "-hsh: %s: %s\n", pathname, strerror(errno));
    else if (dup2(*pfd, STDERR_FILENO) != STDERR_FILENO)
	perror("dup2 error for stderr");
    else
	close(*pfd);
    return (errno) ? -1 : 0;
}

/* Remove arguments from argument list
 * @idx: starting index for processing
 * @pnargs: pointer to nargs variable
 * @args: command line argument buffer */
void del_args(int idx, int *pnargs, char **args)
{
    int j;
    for (j = idx; j < *pnargs-2; j++)
        args[j] = args[j+2];
    args[j] = NULL;
    *pnargs -= 2;
}

//===================================================================//
// 	     	 						     //
// 	     	    	IO Redirection Interface		     //
// 	     	 						     //
//===================================================================//

/* Redirect command line IO
 * @pnargs: pointer to nargs variable
 * @args: command line argument buffer
 * @return: 0 if no exceptions otherwise 1 */
int io_redirect(int *pnargs, char **args)
{
    int i = 0, rel = 0, fd[3];	/* i needs to be started with 0 */

    //if (io_exception_hdlr(*pnargs, args))
	//return 1; // checked inside pipe_exception_hdlr()

    /* IMPORTANT! MUST remove io operators and associated 
     * file names in args; using del_args() to do that */
    while (i < *pnargs-1) {
	if (!strcmp(args[i], "<")) {
	    if(stdin_save == -1) stdin_save = dup(STDIN_FILENO);
	    if (-1 == (rel = redirect_stdin(&fd[0], args[i+1]))) break;
	    else del_args(i, pnargs, args);
	} else if (!strcmp(args[i], ">") || !strcmp(args[i], "1>")) {
	    if(stdout_save == -1) stdout_save = dup(STDOUT_FILENO);
	    if (-1 == (rel = redirect_stdout(&fd[1], args[i+1]))) break;
	    else del_args(i, pnargs, args);
	} else if (!strcmp(args[i], ">>")) {
	    if(stdout_save == -1) stdout_save = dup(STDOUT_FILENO);
	    if (-1 == (rel = redirect_stdout_append(&fd[1], args[i+1]))) break;
	    else del_args(i, pnargs, args);
	} else if (!strcmp(args[i], "2>")) {
	    if(stderr_save == -1) stderr_save = dup(STDERR_FILENO);
	    if (-1 == (rel = redirect_stderr(&fd[2], args[i+1]))) break;
	    else del_args(i, pnargs, args);
	} else {
	    ++i;
	}
    }
    return (rel == -1);
}

/* Retore stdin, stdout and/or stderr 
 * after redirection is done. */
void restore_stdio(void)
{
    if (stdin_save != -1)
	dup2(stdin_save, STDIN_FILENO);
    if (stdout_save != -1)
	dup2(stdout_save, STDOUT_FILENO);
    if (stderr_save != -1)
	dup2(stderr_save, STDERR_FILENO);
}
