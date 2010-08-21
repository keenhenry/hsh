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
#include <readline/readline.h>	/* The GNU readline library */
#include <readline/history.h>	/* The GNU history library */

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
struct List dirs_stack;

/* pathname list */
struct List paths_list;

/* current working directory: in absolute path name */
char cwd[PATH_SIZE] = {0};

/* current working directory: in relative path name */
char rel_cwd[PATH_SIZE] = {0};

/* system hostname hsh currently running on */
static char hostname[30] = {0};

/* a static variable as a command line buffer */
static char *cmd_buf = (char*)NULL;	

/* function prototypes */
int do_main(void);

void init_shell();
void run_shell();
void exit_shell();

void path_abs2rel();
void update_prompt(char **prompt_buf);
char *readline(const char *prompt);
char *rl_gets(char *prompt);

int cmd_tokenizer(char *args[]);

void die_with_error(char *msg);

/*
void cd(const char *dir);
void access_stack(const char *op, const char *dir, stackT *s, int nargs);
int path_cmd(int nargs, char *args[], stackT *s);
int find_cmd(const stackT *list, const char *cmd, char *path_buf);
*/
#endif
