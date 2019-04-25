#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int print_all(int pipe_stdout){
	char c;
	while(read(pipe_stdout, &c, 1) > 0){
		printf("%c", c);
	}
	return 0;
}

int get_all(int pipe_stdout, char *buf, int buf_size){
	char c;
	int i = 0;
	while(read(pipe_stdout, &c, 1) > 0 && i < buf_size){
		buf[i++] = c;
		printf("%c", c);
	}
	return i;
}

static int main_child(char *args[], int pipe_stdin[2], int pipe_stdout[2]){
	printf("main child\n");
	close(pipe_stdin[1]);
	close(pipe_stdout[0]);
	dup2(pipe_stdin[0], 0);
	dup2(pipe_stdout[1],1);
	close(pipe_stdin[0]);
	close(pipe_stdout[1]);
	execvp(args[0],args);
	return 0;
}

static int main_parent(int pipe_stdin[2], int pipe_stdout[2]){
	int flags;
       	printf("main parent\n");
	close(pipe_stdin[0]);
	close(pipe_stdout[1]);
	flags = fcntl(pipe_stdout[0], F_GETFL, 0);
	fcntl(pipe_stdout[0], F_SETFL, flags | O_NONBLOCK);
	return 0;
}

pid_t popen_rw(char *args[], int *fd_stdin, int *fd_stdout){
	pid_t pid_f1;
	int pipe_stdin[2], pipe_stdout[2];
	
	pid_f1 = pipe(pipe_stdin);
	if(pid_f1){
		printf("can't open pipe\n");
		return -1;
	}
	if(pipe(pipe_stdout)){
		printf("can't open pipe\n");
		return -1;
	}
	switch(fork()){

	case -1:
		printf("fail to fork!\n");
		return -1;
		break;
	case 0:
		main_child(args, pipe_stdin, pipe_stdout);
		break;
	default:
		main_parent(pipe_stdin, pipe_stdout);
		break;
	}
	*fd_stdin = pipe_stdin[1];
	*fd_stdout = pipe_stdout[0];
	return pid_f1;
}
