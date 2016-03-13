#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/param.h>

#define MAXQUEUE      5   /* maximum number of pending connections */
#define PORTNUMBER 2225   /* arbitrary portnumber in range we may use */
#define MAXOUTPUT   2000   /* outout buffer size */

static void print_ip_and_port(void);
static void clean_up(int);
static void set_up_server(void);
static void server_loop(void);
static void server_handler(void);

/* global variables so they are accessible in signalhandler */

static int serverFD = -1, clientFD = -1;  /* fd for server and client socket */


int main(int argc, char *argv[]) {

    /* TODO install signalhandler clean_up for clean termination */

	set_up_server();

    print_ip_and_port();   /* inform user of where we are running */

	server_loop();

}


static void set_up_server(void) {
	struct sockaddr_in serverINETAdress;

    /*	
    AT_INET > IPv4 internet protocoll
    SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams.
   		An out-of-band data transmission mechanism may be supported.
	0 > TCP
	*/
	serverFD = socket(AF_INET, SOCK_STREAM, 0);

    /* initialize adresses of server */
	bzero((void *) &serverINETAdress, sizeof(serverINETAdress));

    /* Internet domain */
	serverINETAdress.sin_family = AF_INET;
    /* Accept connectionsfrom any IP adress */
	serverINETAdress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverINETAdress.sin_port = htons((u_short)PORTNUMBER);
    /* set reuse address so you don't have to wait for the port to be free again */
	int on = 1;
	setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    //Bind socket with adress.
    /* int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); */
	if(0 != bind(serverFD, (struct sockaddr*)&serverINETAdress, sizeof(serverINETAdress))){
		printf("Could not bind socket. Shuting down.");
		return;
	}
	else printf("Bound\n");

    /* int listen(int sockfd, int backlog); */
	if(0 != listen(serverFD,MAXQUEUE)){
		printf("Listen failed. Shuting down.");
		return;
	}
	else printf("Listening\n");
}


static void server_loop(void) {
	for (;;){

		struct sockaddr_in clientINETAdress;
		socklen_t clientLen = sizeof(clientINETAdress);

		//Accept connection
	    /* int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen); */
		clientFD = accept(serverFD, (struct sockaddr*)&clientINETAdress, &clientLen);

		fprintf(stdout, "connected clientFD %d from IPadress %s\n",
			clientFD, inet_ntoa(clientINETAdress.sin_addr));

	    /* Create new child to deal with this connection */
		pid_t cpid = fork();
	    if (cpid == 0) { // child
	        serverFD = -1; // avoid cleaning this up
	        server_handler();
	        exit(0);
	    } else { // parent
	        clientFD = -1; // reset so it doesn't get closed twice
	    }
	}
}


static void server_handler(void) {
	unsigned long bufferlen = 0;
	for (;;) {
		//Allocate buffers
		char *buffer = calloc(1, sizeof(int));
		char *finalMessage = calloc(MAXOUTPUT, sizeof(char));
		int finalMessageLength[1];

        /* Receive message length...remember to check for errors! */
		/*ssize_t recv(int sockfd, void *buf, size_t len, int flags);*/
		recv(clientFD, buffer, sizeof(int), 0);
		bufferlen = buffer[0];

		fprintf(stdout, "Received: %lu Bytes\n", bufferlen);

		if (bufferlen > 0) {
			//Allocate space for the command
			char *cmd = (char*) calloc(bufferlen, sizeof(char));
            /* Receive message contents...remember to check for errors! */
			recv(clientFD,cmd,bufferlen,0);
            /* Deal with command "exit"
            or deal with command "cd"
            or deal with anything else using popen*/

            fprintf(stdout, "Received command: %s", (char*)cmd);

            if (strncmp(cmd, "exit", 4) == 0){
            	clean_up(0);
            	return;
            }

            //If not cd then cont.
            if(strstr(cmd, "cd") == NULL){
            	//Reset buffer
            	buffer[0] = '\0';
            	/* forward responses to the client and terminate with a bufferlength 0 */
            	/*FILE *popen(const char *command, const char *type);*/
            	FILE* file = popen(cmd, "r"); // r = read

            	printf("Sending:");
            	//Continue reading from the file and send MAXOUTPUT size parts to the client
            	while(fgets(finalMessage,MAXOUTPUT,file) != NULL){
            		finalMessageLength[0] = strlen(finalMessage);

            		//printf("%s", finalMessage);

            		//Send the size
            		send(clientFD, (char*)finalMessageLength, sizeof(int), 0);
					//Send the part
            		send(clientFD, finalMessage, strlen(finalMessage), 0);

            		//Reset buffer
            		buffer[0] = '\0';	
            	}
            	//Now send the zero
            	finalMessageLength[0] = 0;
            	send(clientFD, (char*)finalMessageLength, sizeof(int), 0);

            	//Close the file
            	pclose(file);

            	//Reset bufferlen
            	bufferlen = 0;
            }
            else{
            	printf("Cant handle: %s", cmd);

            	finalMessage = "Cant handle cd";
            	finalMessageLength[0] = 14;
        		//Send the size
        		send(clientFD, (char*)finalMessageLength, sizeof(int), 0);
				//Send the part
        		send(clientFD, finalMessage, strlen(finalMessage), 0);

        		//Now send the zero
            	finalMessageLength[0] = 0;
            	send(clientFD, (char*)finalMessageLength, sizeof(int), 0);
            }


            /* Remember to allocate and free the receive buffer */
            bufferlen = 0; // reset bufferlen
            free(cmd);  
            free(buffer);
            free(finalMessage);
        }

    }
}


static void print_ip_and_port(void) {
	char hostname[121];
	struct hostent *hostPtr;

	gethostname(hostname, 120);
	hostPtr = gethostbyname(hostname);

	printf("Server is running on: %s (",
		hostname);
	if (hostPtr != NULL) {
		int i = 0;
		while (hostPtr->h_addr_list[i] != NULL) {
			struct sockaddr_in server;
			memcpy(&server.sin_addr, hostPtr->h_addr_list[0], hostPtr->h_length);
			printf(" %s ", inet_ntoa(server.sin_addr));
			i++;
		}
	}
	printf(") : %d\n", PORTNUMBER);
}


static void clean_up(int sign) {
	fprintf(stderr, "Server is shutting down....\n");
	if (serverFD != -1) {
		fprintf(stderr, "Closing serverFD %d\n", serverFD);
		close(serverFD);
	}
	if (clientFD != -1) {
		fprintf(stderr, "Closing clientFD %d\n", clientFD);
		close(clientFD);
	}
	_exit(0);
}
