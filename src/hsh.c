/**
 * hsh.c: Hank Shell 
 * @author: Henry Huang
 * @date: 04/08/2010
 */

#include "list.h"
#include "builtins.h"
#include "hsh.h"

extern BUILTIN builtins[];
extern char *xmalloc PARAMS((size_t));	/* extern function declaration */

int do_main(void)
{
	/* initialize shell */
	init_shell();
	
	/* execute commands from command line */
	execute_cmd();

	/* clean up hsh memory*/
	clean_shell();

	return 0;
}

int main(void)
{
	return do_main();
}

//===================================================================//
// 	     	 						     //
// 	     	 Error Handling Helper Functions	    	     //
// 	     	 						     //
//===================================================================//

/* show error message and exit shell */
static void die_with_error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

//===================================================================//
// 	     	 						     //
// 	     	 	Hsh Helper Functions	    	     	     //
// 	     	 						     //
//===================================================================//

char *dupstr (char *s)
{
    char *r;

    r = xmalloc (strlen (s) + 1);
    strcpy (r, s);
    return (r);
}

/* Convert absolute pathname into relative 
 * (to home directory) pathname. */
static void path_abs2rel()
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
 * @prompt_buf: buffer to hold the prompt string */
static void update_prompt(char **prompt_buf)
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
 * otherwise a pointer to the string read. */
static char *rl_gets(char *prompt)
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
 * @return: # of tokens; -1 when too much tokens */
static int cmd_tokenizer(char **args)
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

/* Look up the name of a command.
 * @name: the name of the command
 * @return: a pointer to that BUILTIN entry;  
 * return NULL if name isn't a builtin name. */
BUILTIN *find_builtins(char *name)
{
	register int i;
	
	for (i = 0; builtins[i].name; i++)
	    if (strcmp(name, builtins[i].name) == 0)
		return (&builtins[i]);
	return (BUILTIN*)NULL;
}

/* Execute builtin command.
 * @nargs: # of command line arguments
 * @args: a buffer to hold tokens
 * @return: -2 if command is not builtin cmd;
 * return 1 to break out loop;
 * return otherwise to continue loop */
static int execute_builtin(int nargs, char **args)
{
    BUILTIN *builtin = (BUILTIN*) NULL; 
	
    if (!(builtin = find_builtins(args[0])))	// command is not builtin
		return -2;
    return (*(builtin->func))(nargs, args); 
}

//===================================================================//
// 	     	 						     //
//                 Interface to Readline Completion                  //
// 	     	 						     //
//===================================================================//

/* Tell the GNU Readline library how to complete. We want to try to complete
 * on builtin names if this is the first word in the line, or on filenames
 * if not. */
void initialize_readline (void)
{
    /* Allow conditional parsing of the ~/.inputrc file. */
    rl_readline_name = "hsh";

    /* Tell the completer that we want a crack first. */
    rl_attempted_completion_function = hsh_completion;
}

/* Attempt to complete on the contents of TEXT.  START and END bound the
 * region of rl_line_buffer that contains the word to complete.  TEXT is
 * the word to complete.  We can use the entire contents of rl_line_buffer
 * in case we want to do some simple parsing.  Return the array of matches,
 * or NULL if there aren't any. */
char **hsh_completion (const char *text, int start, int end)
{
    char **matches = (char **)NULL;

    /* If this word is at the start of the line, then it is a command
     * to complete.  Otherwise it is the name of a file in the current
     * directory. */
    if (start == 0)
        matches = rl_completion_matches (text, command_generator);

    return (matches);
}

/* Generator function for command completion.  STATE lets us know whether
 * to start from scratch; without any state (i.e. STATE == 0), then we
 * start at the top of the list. */
char *command_generator (const char *text, int state)
{
    static int list_index, len;
    char *name;

    /* If this is a new word to complete, initialize now.  This includes
     * saving the length of TEXT for efficiency, and initializing the index
     * variable to 0. */
    if (!state) {
	list_index = 0;
	len = strlen (text);
    }

    /* Return the next name which partially matches from the command list. */
    while ((name = builtins[list_index].name)) {
        list_index++;
        if (strncmp (name, text, len) == 0)
            return (dupstr(name));
    }

    /* If no names matched, then return NULL. */
    return ((char *)NULL);
}

//===================================================================//
// 	     	 						     //
// 	     	 	Hsh Primary Functions	    	     	     //
// 	     	 						     //
//===================================================================//

/* Initialize environment variables 
 * before entering shell */
void init_shell()
{
	/* initialize environmental variables for shell */
	list_init(&dirs_stack);
	list_init(&paths_list);

	/* Bind our completer. */	
	initialize_readline();
	
	/* start using history */
	using_history();

	/* get hostname */
	if (gethostname(hostname, sizeof(hostname)))
		die_with_error("gethostname");
}


/* Execute command line */ 
void execute_cmd()
{
	int nargs;			/* # of args */
	int rel_blt;			/* return value of execute_builtin() */
	//int error;			/* error code for find_cmd() functions */
	//pid_t pid;
	
	//char cmd_path[PATH_SIZE];	/* command search path */
	char *prompt  = (char*)NULL;	/* command line prompt */
	char *args[MAX_NUM_ARGS+1];	/* buffer holding cmd line args */
	//char cmd_path[PATH_SIZE];	/* command search path */

	while (1) {
		/* get current working directory in relative path 
		 * to home directory */
		path_abs2rel();
	
		/* making command line prompt for user */
		update_prompt(&prompt);
	
		/* display shell prompt and read user inputs */
		if (rl_gets(prompt) == NULL || !*cmd_buf)
			continue;
		
		/* tokenize command line string */
		if (-1 == (nargs = cmd_tokenizer(args))) {
			fprintf(stderr, "-hsh: too many arguments\n");
			continue;
		}

		/* execute commands */
		rel_blt = execute_builtin(nargs, args);
		
		if (rel_blt == -1)
			break;
		else if (rel_blt >= 0)
			continue;
		else
			;	// do something
		/*
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

/* Clean up data structures and 
 * release memory from control */ 
void clean_shell()
{
	list_clean(&dirs_stack);
	list_clean(&paths_list);
	clear_history();
}

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
