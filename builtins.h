#ifndef _BUILTINS_H_
#define _BUILTINS_H_

/* header files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <readline/readline.h>	/* The GNU readline library */
#include <readline/history.h>	/* The GNU history library */
#include "list.h"

/* external variables */
extern char cwd[];	// can't use char *cwd; i don't know why
extern struct List dirs_stack;

/* function signatures */
int cd_exception_hdlr(int nargs, char **args);
void cd_hdlr(int nargs, char **args);

void echo_hdlr(int nargs, char **args);

void pwd_hdlr();

void pushd_hdlr(int nargs, char **args);

int popd_exception_hdlr(char **args);
void popd_hdlr(char **args);

int dirs_exception_hdlr(int nargs, char **args);
void print_stack_element(void *data);
void dirs_hdlr(int nargs, char **args);

int  his_exception_hdlr(int nargs, char **args, HIST_ENTRY **hlist);
void print_history(int n_of_entries, HIST_ENTRY **hlist);
void history_hdlr(int nargs, char **args);

#endif
