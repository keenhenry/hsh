#ifndef _BUILTINS_H_
#define _BUILTINS_H_

/* header files */
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>	/* The GNU readline library */
#include <readline/history.h>	/* The GNU history library */

/* function signatures */
int  his_exception_hdlr(int nargs, char **args, HIST_ENTRY **hlist);
void print_history(int n_of_entries, HIST_ENTRY **hlist);
void history_hdlr(int nargs, char **args);

#endif
