/**
 * hsh.c: Hank Shell 
 * @author: Henry Huang
 * @date: 04/08/2010
 */

#include "list.h"
#include "hsh.h"
#include "builtins.h"

int main(void)
{
	return do_main();
}

int do_main(void)
{
	/* initialize shell */
	init_shell();
	
	/* run shell */
	run_shell();

	/* exit shell */
	exit_shell();

	return 0;
}

/* Initialize environment variables 
 * before entering shell.
 */
void init_shell()
{
	/* initialize environmental variables for shell */
	list_init(&dirs_stack);
	list_init(&paths_list);
	
	/* start using history */
	using_history();

	/* get hostname */
	if (gethostname(hostname, sizeof(hostname)))
		die_with_error("gethostname");
}

/* Primary function to run shell. 
 */ 
void run_shell()
{
	int nargs;			/* # of args */
	//int error;			/* error code for find_cmd() functions */
	//pid_t pid;
	
	//char cmd_path[PATH_SIZE];	/* command search path */
	char *prompt  = (char*)NULL;	/* command line prompt */
	char *args[MAX_NUM_ARGS+1];	/* buffer holding cmd line args */
	//char cmd_path[PATH_SIZE];	/* command search path */

	while (1) {
		/* get current working directory in relative path to home directory */
		path_abs2rel();
	
		/* making command line prompt for user */
		update_prompt(&prompt);
	
		/* display shell prompt and read user inputs */
		if (rl_gets(prompt) == NULL || !*cmd_buf)
			continue;
		/* tokenize command line string */
		nargs = cmd_tokenizer(args);

		/* execute builtins if args[0] found in builtins[] */
		if (nargs == -1) {
			fprintf(stderr, "-hsh: too many arguments\n");
			continue;
		} else if (!strcmp(args[0], builtins[0])) {	// exit
			break;
		} else if (!strcmp(args[0], builtins[1])) {	// cd
			cd_hdlr(nargs, args);
			//continue;
		} else if (!strcmp(args[0], builtins[2])) {	// echo
			echo_hdlr(nargs, args);
		} else if (!strcmp(args[0], builtins[3])) {	// pwd
			pwd_hdlr();
		} else if (!strcmp(args[0], builtins[8])) {	// history
			history_hdlr(nargs, args);
		}/* else if (!strcmp(buf_arg[0], builtins[2]) || 
		    !strcmp(buf_arg[0], builtins[3]) || 
		    !strcmp(buf_arg[0], builtins[4])) {	// pushd, popd, dirs *
		       	access_stack(buf_arg[0], buf_arg[1], &dir, nargs);
			continue;
		}
		if (!strcmp(buf_arg[0], builtins[5])) {	// path *
			path_cmd(nargs, buf_arg, &path);
			continue;
		}

		// check for utilities in path *
		error = find_cmd(&path, buf_arg[0], cmd_path);
		if(error == 0) {	// command found 
			switch (pid = fork()) {
			case -1:
				perror("fork");
				exit(1);
			case 0:	// child process here 
				execv(cmd_path, buf_arg);
				perror("execv");
				exit(1);
			default:
				if (waitpid(pid, NULL, 0) != pid) {
					perror("waitpid");	
					exit(1);
				}	
			}
		} else {
			fprintf(stderr, "-sh: %s: command not found\n", buf_arg[0]);
		}
*/
	}

	/* release memory from control */
	if (prompt)
        	free(prompt);
	if (cmd_buf)
        	free(cmd_buf);
}

/* Function to exit shell program.
 * Cleaning up data structures and
 * release memory from control.
 */ 
void exit_shell()
{
	list_dtor(&dirs_stack);
	list_dtor(&paths_list);
	clear_history();
}

/* Convert absolute pathname into relative 
 * (to home directory) pathname.
 */
void path_abs2rel()
{
	const char *home_dir = getenv("HOME");
	char *path = NULL;

	/* zero out buffers */
	memset(cwd, 0, PATH_SIZE);
	memset(rel_cwd, 0, PATH_SIZE);

	/* get current working directory in abs pathname */
	getcwd(cwd, PATH_SIZE);

	/* make relative working directory path */
	if (strstr(cwd, home_dir)) {  	
		path = cwd + strlen(home_dir); 
		strncat(rel_cwd, "~", 1);	
		strncat(rel_cwd, path, strlen(path));
	} else {			
		strncpy(rel_cwd, cwd, strlen(cwd)+1);
	}
}

/* Update command line prompt.
 * @prompt_buf: buffer to hold the prompt string
 * @return:
 */
void update_prompt(char **prompt_buf)
{
	int len = strlen(getenv("USERNAME")) + strlen(hostname) 
		+ strlen(rel_cwd) + strlen("@:# ");
	
	/* If the buffer has already been allocated, 
	 * return the memory to the free pool. */
	if (*prompt_buf) {
        	free(*prompt_buf);
		*prompt_buf = (char*) NULL;
      	}

	*prompt_buf = (char*) malloc(len*sizeof(char) + 1);
	if (*prompt_buf == NULL)
		die_with_error("malloc");
	else	
		memset(*prompt_buf, 0, sizeof(*prompt_buf));

	/* make command line prompt */
	strcat(strcat(*prompt_buf, getenv("USERNAME")), "@");
	strcat(strcat(*prompt_buf, hostname), ":");
	strcat(strcat(*prompt_buf, rel_cwd), "# ");
}

/* Readline_Gets function: 
 * Read a string, and return a pointer to it.
 * @prompt: prompt string buffer
 * @return: NULL on EOF && emptry string, 
 * otherwise a pointer to the string read. 
 */
char *rl_gets(char *prompt)
{
	/* If the buffer has already been allocated,
	 * return the memory to the free pool. */
	if (cmd_buf) {
        	free(cmd_buf);
		cmd_buf = (char *)NULL;
      	}

	/* Get a line from the user. */
	cmd_buf = readline(prompt);

	/* If the line has any text in it, 
	 * save it on the history. */
	if (cmd_buf && *cmd_buf)
        	add_history(cmd_buf);

	return (cmd_buf);
}

/* Parse command line string into tokens.
 * @args: a buffer to hold tokens
 * @return: # of tokens; -1 when too much tokens
 */
int cmd_tokenizer(char **args)
{
	int 	count = 0;
	char 	*token;

	token = strtok(cmd_buf, " \t");
	while (token) {
		args[count++] = token;
		token = strtok(NULL, " \t");
	}

	if (count > MAX_NUM_ARGS)
		count = -1;
	else
		args[count] = NULL;
	
	return count;
}

/* accessing directory stack 
void access_stack(const char *op, const char *dir, stackT *s, int nargs)
{
	if(!strcmp(op, "pushd")){
		push(s, NULL);
		if (nargs == 1)
			cd(NULL);
		else
			cd(dir);
	}	
	if(!strcmp(op, "popd")){
		if (is_empty(s)) {
			fprintf(stderr, "-sh: popd: directory stack empty\n");
			return;
		}
		if (nargs > 1) {
			fprintf(stderr, "-sh: usage: popd\n");
			return;
		}
		cd(top(s));
		pop(s);
	}	
	if(!strcmp(op, "dirs")) {
		if (nargs > 1) {
			fprintf(stderr, "-sh: usage: dirs\n");
			return;
		}
		show_stack(s, '\n');
	}
}*/

/* parsing path command 
int path_cmd(int nargs, char *args[], stackT *s)
{
	struct stat buf;
	
	if (nargs == 1) {
		show_stack(s, ':');
		if (size(s)) printf("\n");
		return 0;
	}

	if (nargs != 3) {
		fprintf(stderr, "-sh: usage: %s [+|-] [/some/dir]\n", args[0]);
		return 1;	// to indicate error occurs 
	}

	if (!strcmp(args[1], "+")) {
		if (stat(args[2], &buf) < 0) {
			fprintf(stderr, "-sh: path: %s\n", strerror(errno));
			return 1;
		}
		if (find_node(s, args[2]) >= 0) {// detecting duplicated path 
			fprintf(stderr, "-sh: path: %s already in path list\n", args[2]);
			return 1;
		}
		push(s, args[2]);
		return 0;
	}

	if (!strcmp(args[1], "-")) {
		if (is_empty(s)) {
			fprintf(stderr, "-sh: path: path list empty\n");
			return 1;
		}
		if (rm_node(s, find_node(s, args[2])) > 0) {
			fprintf(stderr, "-sh: path: %s is not in path list\n", args[2]);
			return 1;
		}
		return 0;
	}

	fprintf(stderr, "-sh: path: %s: invalid argument\n", args[1]);
	return 1;	
}*/

/* to find the executables 
int find_cmd(const stackT *list, const char *cmd, char *path_buf)
{
	struct stat sb;		// store command file status

	// check cmd size is not too long 
	if (strlen(cmd) + strlen("./") >= PATH_SIZE - 1) {
		fprintf(stderr, "command too long\n");
		return -1;
	}

	memset(path_buf, 0, PATH_SIZE);
	strcat(path_buf, "./");
	strcat(path_buf, cmd);
	
	// first search for current dir 
	if (stat(path_buf, &sb) == 0) {
		if ((sb.st_mode & S_IXOTH) != 00001) // user has no execute permission
			return -1;
		return 0; 	// command found 
	} else {
		if (errno == ENOENT); 
		else {
			perror("stat");
			exit(1);
		}
	}

	// otherwise search for path list 
	struct Node *tmp = list->head;
	while (tmp) {
		if (strlen(cmd) + strlen(tmp->data) + strlen("/") >= PATH_SIZE - 1) {
			fprintf(stderr, "command too long\n");
			return -1;
		}

		memset(path_buf, 0, PATH_SIZE);
		strcat(path_buf, tmp->data);
		if (path_buf[strlen(path_buf)-1] != '/')
			strcat(path_buf, "/");
		strcat(path_buf, cmd);
	
		if (stat(path_buf, &sb) == 0) {
			if ((sb.st_mode & S_IXOTH) != 00001) 
				return -1;
			return 0;	// found 
		} else {
			if (errno == ENOENT) {
				tmp = tmp->next;
				continue;
			} else {
				perror("stat");
				exit(1);
			}
		}
	}
	return -1;	 command not found
}*/

/**
 * show error message and exit shell
 */
void die_with_error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}
