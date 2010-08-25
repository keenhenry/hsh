#ifndef _BUILTINS_H_
#define _BUILTINS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <readline/readline.h>	/* The GNU readline library */
#include <readline/history.h>	/* The GNU history library */
#include "list.h"

/* A structure which contains information on the shell 
 * builtin commands this program can understand */
typedef int hsh_btfunc_t (int, char**);	/* builtin function pointer type */

typedef struct {
    char *name;		/* user printable name */
    char *doc;		/* documentation string for this function */
    hsh_btfunc_t *func;	/* function to call to do the job */
} BUILTIN;

/* builtin function signatures */
int builtin_exit(int nargs, char **args);
int builtin_cd(int nargs, char **args);
int builtin_echo(int nargs, char **args);
int builtin_pwd(int nargs, char **args);
int builtin_pushd(int nargs, char **args);
int builtin_popd(int nargs, char **args);
int builtin_dirs(int nargs, char **args);
int builtin_path(int nargs, char **args);
int builtin_history(int nargs, char **args);
int builtin_kill(int nargs, char **args);
int builtin_jobs(int nargs, char **args);

/* other function signatures */
void list_clean(struct List *list);
#endif
