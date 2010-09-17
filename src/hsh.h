#ifndef _HSH_
#define _HSH_

/* included header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <readline/readline.h>	/* The GNU readline library */
#include <readline/history.h>	/* The GNU history library */
#include "list.h"

/* definition of symbolic constants */
#define MAX_NUM_ARGS 256
#define PATH_SIZE 4096

/*========================= 
 * Global Data Structures *
 =========================*/

/* A structure which contains information on the shell 
 * builtin commands this program can understand */
typedef int hsh_btfunc_t (int, char**);	/* builtin function pointer type */

typedef struct {
    char *name;		/* user printable name */
    char *doc;		/* documentation string for this function */
    hsh_btfunc_t *func;	/* function to call to do the job */
} BUILTIN;

/*====================== 
 * Function Prototypes *
 ======================*/

/* builtin helper function signatures */
void list_clean(struct List *list);

/* hsh helper function signatures */
char *dupstr (char *s);

/* readline interface */
char *readline(const char *prompt);
void initialize_readline (void);
char *command_generator(const char *, int);
char **hsh_completion(const char *, int, int);

/* IO redirection interface */
int io_redirect(int *pnargs, char **args);
void restore_stdio(void);

/* builtin command interface */
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

/* hsh interface */
void init_shell();
void execute_line();
void clean_shell();

#endif
