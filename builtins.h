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
extern char cwd[];	// can't use char *rel_cwd; i don't know why
extern char rel_cwd[];	// can't use char *rel_cwd; i don't know why
extern struct List dirs_stack;

/* function signatures */
void cd_hdlr(int nargs, char **args);
void echo_hdlr(int nargs, char **args);
void pwd_hdlr();
void pushd_hdlr(int nargs, char **args);
void popd_hdlr(char **args);
void dirs_hdlr(int nargs, char **args);
void history_hdlr(int nargs, char **args);
void list_clean(struct List *list);
#endif
