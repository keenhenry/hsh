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

/* definition of symbolic constants */
#define MAX_NUM_ARGS 256
#define CMD_BUF_SIZE 512
#define PATH_SIZE 4096

/**
 * Global Data Structures
 **/

/* builtins for shell */
static const char *builtins[] = {"exit", "cd", "echo", "pwd", "pushd", "popd", "dirs", "path", "history", "kill"};

/* directory stack */
static struct List dirs_stack;

/* pathname list */
static struct List paths_list;

/* command history queue */
static struct List cmd_queue;

/* current working directory */
static char *pwd = NULL;

/* function prototypes */
/*
int input_clean(char *buf);
int read_cmd(char *buf, int *bf_sz);
int tokenize_cmd(char *args[], char *cmd);
void cd(const char *dir);
void access_stack(const char *op, const char *dir, stackT *s, int nargs);
int path_cmd(int nargs, char *args[], stackT *s);
int find_cmd(const stackT *list, const char *cmd, char *path_buf);
*/
#endif
