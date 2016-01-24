//http://stackoverflow.com/questions/8082932/connecting-n-commands-with-pipes-in-a-shell
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int in = 0;


char ** processes;
int num_of_processes;


int spawn(int in, int out, char *cmd){
	int pid = fork();
	if(pid < 0){
		printf("Fork error\n");
		exit(1);
	}
	//ataching the in to STDIN
	else if (pid == 0){
		if (in != 0){
			dup2(in, 0);
			close(in);
		} // child will read from the pipe of the previous process (int in)

		if (out != 1){
			dup2(out,1);
			close(out);
		} // child will write to the pipe

		execl(cmd, cmd, (char*)0);

	}
	return pid;
}


int main(int argc, char *argv[] ){

	num_of_processes = argc -1;
	processes = argv + 1;

	int i;
	int fd[2];
	for(i = 0; i<num_of_processes-1; i++){
		pipe(fd);
		spawn (in, fd[1],processes[i]);
		close(fd[1]); // no need. child process will write into it.
		in = fd[0]; // for the next process to be able to read from the pipe which the current process is writing into(via the child process)
	}	


	// last process needs to read from the the pipe of the previous process
	if(in != 0){
		dup2(in, 0);
	}

	execl(processes[i], processes[i], (char *)0);

}