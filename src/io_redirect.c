/**
 * This file is I/O redirection interface for Hank Shell.
 * @author: Henry Huang
 * @date: 09/15/2010
 */

#include "hsh.h"

/* Check for command line syntax errors.
 * @nargs: # of arguments
 * @args: command line argument buffer
 * @return: 0 if no exceptions found otherwise non-zero */
int io_exception_hdlr(int nargs, char **args)
{
    int i, exception = 0;

    for (i = 1; i < nargs-1; i++) {
	//if (!strcmp(args[i], ">") && !strcmp(args[i+2], "<"))
	//    exception = 1;
	//else if (!strcmp(args[i], ">") && !strcmp(args[i+2], "|"))
	//    exception = 1;
	//else if (!strcmp(args[i], "|") && !strcmp(args[i+2], "<"))
	//    exception = 1;
    	if (!strcmp(args[nargs-1], ">") || !strcmp(args[nargs-1], "<")) {
	    exception = 2;
	    break;
	} else if (!strcmp(args[i], ">") && !strcmp(args[i+1], ">")) {
	    exception = 1;
	    break;
	} else if (!strcmp(args[i], ">") && !strcmp(args[i+1], "<")) {
	    exception = 1;
	    break;
	} else if (!strcmp(args[i], "<") && !strcmp(args[i+1], ">")) {
	    exception = 1;
	    break;
	} else if (!strcmp(args[i], "<") && !strcmp(args[i+1], "<")) {
	    exception = 1;
	    break;
	} else if (!strcmp(args[i], "2>") && !strcmp(args[i+1], "2>")) {
	    exception = 1;
	    break;
	}  
    }

    if (exception == 1)
	fprintf(stderr, "-hsh: syntax error near unexpected token %s\n", args[i+1]);
    else if (exception == 2)
	fprintf(stderr, "-hsh: syntax error near unexpected token %s\n", args[nargs-1]);
    return exception;
}

/* Set stdin to file
 * @pfd: pointer to file descriptor of that file
 * @pathname: file pathname which stdin redirects to
 * @return: 0 if no errors otherwise -1 */
int redirect_stdin(int *pfd, char *pathname)
{
    if (-1 == (*pfd = open(pathname, O_RDONLY)))
	perror("open");
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
    if (-1 == (*pfd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC)))
	perror("open");
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
    if (-1 == (*pfd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC)))
	perror("open");
    else if (dup2(*pfd, STDERR_FILENO) != STDERR_FILENO)
	perror("dup2 error for stderr");
    else
	close(*pfd);
    return (errno) ? -1 : 0;
}

/* Redirect command line IO
 * @nargs: # of arguments
 * @args: command line argument buffer
 * @return: 0 if no exceptions otherwise 1 */
int io_redirect(int nargs, char **args)
{
    int i, fd[3], rel = 0;

    if (io_exception_hdlr(nargs, args))
	return 1;

    for (i = 1; i < nargs-1; i++) {
	if (!strcmp(args[i], "<"))
	    rel = redirect_stdin(&fd[0], args[i+1]);
	else if (!strcmp(args[i], ">"))
	    rel = redirect_stdout(&fd[1], args[i+1]);
	else if (!strcmp(args[i], "2>"))
	    rel = redirect_stderr(&fd[2], args[i+1]);
    }
    return (rel == -1);
}
