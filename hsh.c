/**
 * hsh.c: Hank Shell 
 * @author: Henry Huang
 * @date: 04/08/2010
 */

#include "list.h"
#include "hsh.h"

/**
 * show error message and exit shell
 */
void die_with_error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

/**
 * initialize some environs before entering shell
 */
void init_shell()
{
	/* initialize environmental variables for shell */
	list_init(&dirs_stack);
	list_init(&paths_list);
	list_init(&cmd_queue);

	/* get hostname */
	if (gethostname(hostname, sizeof(hostname)))
		die_with_error("gethostname");
}

/**
 * convert absolute pathname into relative (to home directory) pathname
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

	/* get the working directory in relative path to home directory */
	if (strstr(cwd, home_dir)) {  	// pathname contains home directory
		path = cwd + strlen(home_dir); 
		strncat(rel_cwd, "~", 1);	
		strncat(rel_cwd, path, strlen(path));
	} else {			// pathname does not contain home directory
		strncpy(rel_cwd, cwd, strlen(cwd)+1);
	}
}

/**
 * update command line prompt 
 */
void update_prompt(char **prompt_buf)
{
	int len = 0;
	len = strlen(getpwuid(getuid())->pw_name) + 
	      strlen(hostname) 			  + 
	      strlen(rel_cwd) 			  +
	      strlen("@:# ");
	
	/* If the buffer has already been allocated, 
	 * return the memory to the free pool. */
	if (*prompt_buf) {
        	free(*prompt_buf);
		*prompt_buf = (char *)NULL;
      	}

	*prompt_buf = (char*) malloc(len*sizeof(char) + 1);
	if (*prompt_buf == NULL)
		die_with_error("malloc");

	/* make command line prompt */
	strcat(strcat(*prompt_buf, getpwuid(getuid())->pw_name), "@");
	strcat(strcat(*prompt_buf, hostname), ":");
	strcat(strcat(*prompt_buf, rel_cwd), "# ");
}

/** 
 * Read a string, and return a pointer to it.
 * Returns NULL on EOF. 
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

	/* If the line has any text in it, save it on the history. */
	if (cmd_buf && *cmd_buf)
        	printf("add_history\n");//add_history(cmd_buf);

	return (cmd_buf);
}

/**
 * function to run shell 
 */ 
void run_shell()
{
	//int nargs;			/* # of args */
	//int error;			/* error code for find_cmd() functions */
	//pid_t pid;
	
	//char cmd_path[PATH_SIZE];	/* command search path */
	char *prompt  = (char*)NULL;	/* command line prompt */
	//char *buf_arg[MAX_NUM_ARGS];	/* buffer holding cmdline words */
	//char cmd_path[PATH_SIZE];	/* command search path */
	
	while (1) {
		/* get current working directory in relative path to home directory */
		path_abs2rel();
	
		/* making command line prompt for user */
		update_prompt(&prompt);
	
		/* display shell prompt and read user inputs */
		if (rl_gets(prompt) == NULL)
			continue;

		// next task: adding history functionality - 
		// check GNU history library documentation

		/*	
		nargs = tokenize_cmd(buf_arg, buf_cmd);		
		if (nargs >= MAX_NUM_ARGS) {
			fprintf(stderr, "-sh: too many arguments\n");
			continue;
		}

		* check for builtins *
		if (nargs == 0)
			continue;
		if (!strcmp(buf_arg[0], builtins[0])) 	// exit *
			break;
		if (!strcmp(buf_arg[0], builtins[1])) {	// cd *
			if (nargs == 1)
				cd(NULL);
			else
				cd(buf_arg[1]);
			continue;
		}	
		if (!strcmp(buf_arg[0], builtins[2]) || 
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
}

void exit_shell()
{
	list_dtor(&dirs_stack);
	list_dtor(&paths_list);
	list_dtor(&cmd_queue);
}


/* parse user command into tokens /
int tokenize_cmd(char *args[], char *cmd)
{
	int count = 0;
	char *token;
	const char delim[] = " \t";

	token = strtok(cmd, delim);
	while (token){
		args[count++] = token;
		token = strtok(NULL, delim);
	}
	args[count] = NULL;
	
	return count;
}*/

/* change current working directory /
void cd(const char *dir)
{
	if (dir == NULL) {
		if (chdir(getpwuid(getuid())->pw_dir) < 0)
			perror("-sh: cd: chdir");
	} else if (chdir(dir) < 0) {  
		fprintf(stderr, "-sh: cd: %s: %s\n", dir, strerror(errno));
	}
}
*/
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

int do_main()
{
	/* initialize shell */
	init_shell();
	
	/* run shell */
	run_shell();

	/* exit shell */
	exit_shell();

	return 0;
}

int main(void)
{
	return do_main();
}
