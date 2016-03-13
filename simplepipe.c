#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ 0 //Read end of pipe
#define WRITE 1 //Write end of pipe
#define CHILD 0 //Child pid = 0


int main(int argc, char **argv){

    //Crete pipe
	int dataPipe[2];
	pipe(dataPipe);
	printf("Pipe created\n");

	//Declare pids and create first child
	pid_t pid1, pid2;
	pid1 = fork();

	//Only let the child1 in.
	if(pid1 == CHILD){
		printf("child1, created. Running ls -la\n");

    	//Set Child_1's stdout to the WRITE side of the pipe.
		dup2(dataPipe[WRITE], WRITE);

		//Close childs pipe
		close(dataPipe[READ]);
		close(dataPipe[WRITE]);

		//Run "ls -la"
		system("ls -la");

		//Terminate the child
		exit(0);
	}

	//Make the parent create the secound child
	else{
		pid2 = fork();

		//Only let child2 in
		if(pid2 == CHILD){
			printf("child2, created. Running '| less'\n");

    		//Set Child_2's stdoin to the READ side of the pipe.
			dup2(dataPipe[READ], READ);

			//Close childs pipe
			close(dataPipe[READ]);
			close(dataPipe[WRITE]);

			//Run "less"
			system("less");

			//Terminate the child
			exit(0);
		}
	}

	//Close parent pipe
	close(dataPipe[READ]);
	close(dataPipe[WRITE]);

	wait(&pid1);
	wait(&pid2);

	exit(0);
}


