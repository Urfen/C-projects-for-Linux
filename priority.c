#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

int main(){

	//Get the priority with the pid
	int parent_pid = getpid();
	
	int priority = getpriority(PRIO_PROCESS, parent_pid);
	//Give val the value of the process pid and creats one new child
	pid_t pid = fork();

	//Only the child is true
	if(pid == 0){
		//Set NICE to 50
		setpriority(PRIO_PROCESS, parent_pid, 10);
		//Get priority of child
		priority = getpriority(PRIO_PROCESS, parent_pid);
	}

	//Print the pid and priority forever.
	while(1){
		printf("\nPid: %d  parent_pid: %d Priority: %d \n", pid, parent_pid, priority);
	}
}