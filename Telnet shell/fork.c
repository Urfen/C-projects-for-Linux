#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LENGTH 5

void iterative(int *array, int size){
	int i;
	pid_t pid;

	for(i = 0; i < size; i++){

		pid = fork();

		if(pid == 0){
			printf("\nChild: %d pid: %d element: %d\n", i, pid,*(array+i));
			break;
		}
	}
}

int main(){

	int array[LENGTH];
	int size;

	//Init array
    //Create LENGTH number of elements. 0..LENGTH-1
	int i ;
	for(i = 0; i < LENGTH; i++){
		*(array+i) = i;
	}

    //Get the length of the array.
	size = sizeof(array) / sizeof(*(array+0));
	
	iterative(array, size);

	return 0;
}