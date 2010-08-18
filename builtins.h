#ifndef _BUILTINS_H_
#define _BUILTINS_H_

/* header files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <readline/readline.h>	/* The GNU readline library */
#include <readline/history.h>	/* The GNU history library */

/* external variables */
extern char cwd[];	// can't use char *cwd; i don't know why

/* function signatures */
int cd_exception_hdlr(int nargs, char **args);
void cd_hdlr(int nargs, char **args);
void pwd_hdlr();
int  his_exception_hdlr(int nargs, char **args, HIST_ENTRY **hlist);
void print_history(int n_of_entries, HIST_ENTRY **hlist);
void history_hdlr(int nargs, char **args);

#endif
