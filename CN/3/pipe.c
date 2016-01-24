#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define STDIN 0 
#define STDOUT 1

char ** processes;
int num_of_processes;


int execute(int process_index){
	if (process_index >= num_of_processes - 1){
		execl(processes[process_index],processes[process_index], (char*)0);
		exit(0);
	}

	// int cur_process_index = process_index;
	// int next_process_index = process_index+1;

	int pipefd[2];
	if (pipe(pipefd) < 0){
		printf("pipe error");
		exit(1);
	}

	int pid = fork();
	if (pid < 0){
		printf("error in fork\n");
		exit(1);
	}
	else if (pid == 0){ //child
		close(pipefd[1]); //closing the write end of pipe
		if (pipefd[0] != STDIN){
			if (dup2(pipefd[0] , STDIN) != STDIN){
				printf("Dup2 error\n");
				exit(1);
			}
			close(pipefd[0]);
		}// any read from STDIN will be from the pipe

		// recursive call for the next process
		execute(process_index+1);
		exit(0);
	}
	else { //parent
		close(pipefd[0]); //closing the reading end of the pipe in the parent
		if (pipefd[1] != STDOUT){
			if (dup2(pipefd[1] , STDOUT) != STDOUT){
				printf("Dup2 error\n");
				exit(1);
			}
			close(pipefd[1]);
		}// any write to the pipe will now be STDOUT

		//executing process
		printf("Executing process #%d\n",process_index );
		execl(processes[process_index],processes[process_index], (char*)0);
		exit(0);

	}
}



int main(int argc, char *argv[] ){

	num_of_processes = argc -1;
	processes = argv + 1;


	// int i;
	// for(i = 0; i<num_of_processes; i++){
		int pid = fork();
		if (pid < 0){
			printf("error in fork\n");
			exit(1);
		}
		else if (pid == 0){ //child

			execute(0);

		}
		else{
			int returnStatus;    
    		waitpid(pid, &returnStatus, 0);  
    		printf("terminated with %d\n", returnStatus);
		}
	// }
}