/**
*	Arvid Bodin TIELA3
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 


bool compare(char command[], char string[]){
	if (strstr(command, string) != NULL) {
    	return(true);
	}
	return(false);
}

int main(){

	printf("Starting badShell...\n");

	bool commandok = true;
	char command[100];

	while(1){
		commandok = true;
		printf(">");
		//Get the sring from treminal
		fgets(command, sizeof(command), stdin);

		//Check that the command does not contain any from of cd or exit.
		if(compare(command,"cd")){
			printf("An error occured!\n");
			commandok = false;
		}
		if(compare(command,"cd ")){
			printf("An error occured!\n");
			commandok = false;
		}
		if(compare(command," cd")){
			printf("An error occured!\n");
			commandok = false;
		}
		if(compare(command,"exit")){
			printf("Exiting!\n");
			break;
		}

		//If the command is not cd or exit process it
		if(commandok){
			system(command);
		}

	}

	return 0;
}