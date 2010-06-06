#ifndef _HSH_
#define _HSH_

#define MAX_NUM_ARGS 256
#define CMD_BUFSIZE 512
#define PATH_SIZE 4096

const char *builtins[] = {"exit", "cd", "echo", "pwd", "pushd", "popd", "dirs", "path", "history", "kill"};

int input_clean(char *buf);
int read_cmd(char *buf, int *bf_sz);
int tokenize_cmd(char *args[], char *cmd);
void cd(const char *dir);
void access_stack(const char *op, const char *dir, stackT *s, int nargs);
int path_cmd(int nargs, char *args[], stackT *s);
int find_cmd(const stackT *list, const char *cmd, char *path_buf);

#endif
