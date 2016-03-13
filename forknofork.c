#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LENGTH 5


void iterative(int *array, int size){
	for(int i = 0; i < size; i++){
		printf("Ite: %d\n", *(array+i));
	}
}

void recursive(int *array, int size){
	printf("Rec: %d\n", *array);
	if(--size > 0){
		recursive((array+1), size); 
	}
}

int main(){
	int array[LENGTH];
	int size;

	//Create LENGTH number of elements. 0..LENGTH-1
	for(int i = 0; i < LENGTH; i++){
		*(array+i) = i;
	}

	//Get the length of the array.
	size = sizeof(array) / sizeof(*(array+0));

	iterative(array, size);
	recursive(array, size);
}