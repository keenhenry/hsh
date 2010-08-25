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
#define PATH_SIZE 4096

/*========================= 
 * Global Data Structures
 =========================*/

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

/* a static variable as a pointer to command line buffer */
static char *cmd_buf = (char*)NULL;	

/*====================== 
 * Function Prototypes
 ======================*/

void init_shell();
void execute_cmd();
void clean_shell();

char *readline(const char *prompt);
void initialize_readline (void);
char *command_generator(const char *, int);
char **hsh_completion(const char *, int, int);

/*
int path_cmd(int nargs, char *args[], stackT *s);
int find_cmd(const stackT *list, const char *cmd, char *path_buf);
*/
#endif
