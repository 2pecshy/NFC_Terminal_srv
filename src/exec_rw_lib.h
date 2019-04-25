#include <sys/types.h>

int print_all(int pipe_stdout);
int get_all(int pipe_stdout, char *buf, int buf_size);
pid_t popen_rw(char *args[], int *fd_stdin, int *fd_stdout);
