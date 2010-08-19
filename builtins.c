/**
 * This file implements all the builtin commands for Hank Shell.
 * @author: Henry Huang
 * @date: 08/13/2010
 */

#include "builtins.h"

/* cd builtin exception handling
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @return: exception code; 0 for NO EXCEPTION OCCURS
 */
int cd_exception_hdlr(int nargs, char **args)
{
	int exception = 0;

	/* check return value of chdir */
	if (nargs == 1 && chdir(getenv("HOME")) < 0)
		exception = 1;
	else if (nargs >= 2 && chdir(args[1]) < 0)
		exception = 2;
	
	/* exception handling */
	if (exception == 1)
		perror("-hsh: cd: chdir");
	else if (exception == 2)
		fprintf(stderr, "-hsh: %s: %s: %s\n", args[0], args[1], strerror(errno));

	return exception;
}

/* cd builtin handler: change working directory.
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 */
void cd_hdlr(int nargs, char **args)
{
	if (cd_exception_hdlr(nargs, args));
}

/* echo builtin handler.
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 */
void echo_hdlr(int nargs, char **args)
{
	int i;
	for (i = 1; i < nargs; i++)
		printf("%s%s", args[i], (i < nargs-1) ? " " : "");
	printf("\n");
}

/* pwd builtin handler.
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 */
void pwd_hdlr()
{
	printf("%s\n", cwd);
}

/* History builtin exception handling
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 * @hlist: list of history entries
 * @return: exception code; 0 for NO EXCEPTION OCCURS
 */
int his_exception_hdlr(int nargs, char **args, HIST_ENTRY **hlist)
{
	int exception = 0;

	/* check for NO HISTORY */
	if (hlist == NULL)
		exception = -1;
	/* check for # of arguments */
	else if (nargs > 2)
		exception = -2;
	/* check for numeric argument */
	else if (nargs == 2 && !atoi(args[1]))
		exception = 1;

	/* exception handling */
	if (exception == -1)
		;
	else if (exception == -2)
		fprintf(stderr, "-hsh: %s: too many arguments\nUsage: %s [n]\n", args[0], args[0]);
	else if (exception == 1)
		fprintf(stderr, "-hsh: %s: %s: numeric argument required\n", args[0], args[1]);

	return exception;
}

/* History print function.
 * @n_of_entries: # of history entries to print
 * @hlist: list of history entries
 */
void print_history(int n_of_entries, HIST_ENTRY **hlist)
{
	int i;
	for (i = n_of_entries; i > 0; i--)
		printf(" %d  %s\n", 
			history_base + history_length - i, 
			hlist[history_length - i]->line);
}

/* History builtin handler.
 * @nargs: # of arguments in command line
 * @args: command line argument buffer
 */
void history_hdlr(int nargs, char **args)
{
	HIST_ENTRY **the_list = history_list();

	/* check if exception occurs */
	if (his_exception_hdlr(nargs, args, the_list))
		return;

	if (nargs == 1) 	// Only one command line argument 
		print_history(history_length, the_list);
	else if (nargs == 2) 	// Two command line arguments
		print_history(atoi(args[1]), the_list);
}
