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
 * convert absolute pathname into relative (to home directory) pathname
 */
void path_abs2rel(char *rel_path)
{
	const char *home_dir = getenv("HOME");
	char *path = NULL;
	
	/* get current working directory in abs pathname */
	getcwd(cwd, PATH_SIZE);

	/* get the working directory in relative path to home directory */
	if (strstr(cwd, home_dir)) {  	// pathname contains home directory
		path = cwd + strlen(home_dir); 
		strncat(rel_path, "~", 1);	
		strncat(rel_path, path, strlen(path));
	} else {			// pathname does not contain home directory
		strncpy(rel_path, cwd, strlen(cwd)+1);
	}
}

/**
 * Print data of an entry in history queue
 */
void print_cmd(void *cmd)
{
	printf("%s\n", (char*)cmd);
}

/**
 * The routine to handle 'history' builtin:
 * traverse history queue and print every entry 
 * from list front to list tail.
 */
void history()
{
	if (is_empty(&cmd_queue))
		printf("No commands buffered\n");
	else	
		list_traversal(&cmd_queue, print_cmd);
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
	
	if (gethostname(hostname, sizeof(hostname)))
		die_with_error("gethostname");
}

/* sanitize user input */
int sanitize_user_input(char *buf)
{
	int i, ch;
	for (i=0; buf[i] != '\0'; i++) {
		ch = buf[i];
		if(ch == '\t' || ch == '\n' || (ch > 31 && ch < 127)) 
			continue;
		else
			break;
	}

	if (i >= strlen(buf)) return 1;
	else 	return 0;
}

/**
 * To read user command into buffer
 * @buf:
 * @bf_sz:
 * @return: 0 when success, otherwise 1 
 */
int read_cmd(char *buf, int *bf_sz)
{
	int count = 0;
	char *buf_tmp;
	while (fgets(buf + count, *bf_sz, stdin)) {
		count = strlen(buf);
		if (count >= (*bf_sz) - 1) { 	// cmd buffer too short
			buf[(*bf_sz)-1] = '\0';
			buf_tmp = (char*) malloc(2 * (*bf_sz));
			strcpy(buf_tmp, buf);
			free(buf);
			buf = buf_tmp;
			(*bf_sz) *= 2;
		} 
	}
	
	if (!sanitize_user_input(buf)) {   // sanitize user input
		fprintf(stderr, "-hsh: bad command\n");
		return 1;
	} else if (buf[count-1] == '\n') {
		buf[count-1] = '\0';
	} 	

	if (ferror(stdin)) { 
		perror("stdin");
		return 1;
	}

	return 0;
}

/**
 * function to run shell 
 */ 
void run_shell()
{
	//int nargs;			/* # of args */
	//int error;			/* error code for find_cmd() functions */
	//pid_t pid;
	
	//char wd[PATH_SIZE];		/* working dir path name */
	//char cmd_path[PATH_SIZE];	/* command search path */
	char *buf_cmd = NULL;		/* user command buffer */
	//char *buf_arg[MAX_NUM_ARGS];	/* buffer holding cmdline words */
	//char cmd_path[PATH_SIZE];	/* command search path */
	
	/* initialize command buffer for user inputs */
	buf_cmd = (char*) malloc(CMD_BUF_SIZE * sizeof(char));
	if (buf_cmd == NULL)
		die_with_error("malloc");
	
	//while (1) {
		/* get current working directory in relative path to home directory */
		path_abs2rel(rel_cwd);
	
		/* display shell prompt */
		printf("%s@%s:%s# ", getpwuid(getuid())->pw_name, hostname, rel_cwd);

		/* zero out command buffer */
		memset(buf_cmd, 0, CMD_BUF_SIZE);
/*	
		if(read_cmd(buf_cmd, &length) > 0)
			continue;
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
	//}
	free(buf_cmd);
}

void exit_shell()
{
	list_dtor(&dirs_stack);
	list_dtor(&paths_list);
	list_dtor(&cmd_queue);
}


/* parse user command into tokens */
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
}

/* change current working directory */
void cd(const char *dir)
{
	if (dir == NULL) {
		if (chdir(getpwuid(getuid())->pw_dir) < 0)
			perror("-sh: cd: chdir");
	} else if (chdir(dir) < 0) {  
		fprintf(stderr, "-sh: cd: %s: %s\n", dir, strerror(errno));
	}
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
