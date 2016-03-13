#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define TRIES         5   /* maximum number of attempts to conect to server */
#define MAXCMD      2000   /* max command length */


static int connect_client(in_addr_t, int);
static void client_handler(void);
static void handle_reply(void);

static char *SIP, *SPORT;
static int socketFD = -1;

int main(int argc, char *argv[]) {
    in_addr_t serverIP;
    int portNumber;

    if (argc != 3) {
        printf("Usage: client serverIPAdr portNumber\n");
        exit(0);
    }

    if (-1 == (serverIP = inet_addr(argv[1]))) {
        perror("First argument was not an IP adress");
        exit(0);
    }
    SIP = argv[1];

    if (1 != sscanf(argv[2], "%d", &portNumber)) {
        perror("Second argument was not a port number");
        exit(0);
    }
    SPORT = argv[2];

    /* connect client to server */
    if (-1 != (socketFD = connect_client(serverIP, portNumber))) {
        /* handle the connection */
        client_handler();
        /* clean up after us */
        close(socketFD);
    }
}


static void client_handler() {
    char cmd[MAXCMD];
    for (;;){
        cmd[0] = '\0'; // reset the buffer to be an empty string
        fprintf(stdout, "%s:%s> ", SIP, SPORT);

        /* Get command for the commandline and length */
        fgets (cmd, MAXCMD, stdin);
        int length[1];
        length[0] = strlen(cmd);
        
        /* Send command length to the server */
        /* size_t send(int sockfd, const void *buf, size_t len, int flags); */
        send(socketFD, (char*)length, sizeof(int),0);

        /* Send command to the server */
        send(socketFD, cmd, strlen(cmd),0);
        
        /* If command was "exit" we should also terminate on this side */
        if (strncmp(cmd, "exit", 4) == 0) {
            return;
        }
        /* If not we wait for the server's reply */
        handle_reply();
    }
}


static void handle_reply() {
    unsigned long bufferlen = 0;
    //Allocate bufffer and returnmsg
    char *buffer = (char*) calloc(MAXCMD, sizeof(char));
    char *returnmsg = (char*) calloc(MAXCMD, sizeof(char));
    int messageNr = 0;
    for(;;) {
        /* Receive message length...remember to check for errors! */
        //Reset buffer and len
        buffer[0] = '\0';
        messageNr = messageNr + bufferlen;
        bufferlen = 0;

        //Receive the length fomr the server.
        recv(socketFD, buffer, sizeof(int), 0);
        bufferlen = buffer[0];
        //printf("Received len: %lu\n", bufferlen);

        //If len = 0 the message is done
        if (bufferlen == 0) {
            /* Print message to stdout */
            printf("%s\n", returnmsg);

            /* Allocate and free the receive buffer */  
            //bufferlen = 0; // reset bufferlen
            //free(buffer);
            //free(returnmsg);

            return; // finish on 0 length message
        }
        
        /* Receive incoming message into buffer...remember to check for errors! */
        recv(socketFD, returnmsg+messageNr, bufferlen, 0); //Receive result form server
        //printf("%s", buffer);
        

    }
}


static int connect_client(in_addr_t serverIP, int portNumber) {
    int attempts = 0, result = -1;
    struct sockaddr_in serverINETAdress;

    /* Create a socket to server in socketFD, with default protocol for stream (TCP) */
    /*  
    AT_INET > IPv4 internet protocoll
    SOCK_STREAM Provides sequenced, reliable, two-way, connection-based byte streams.
        An out-of-band data transmission mechanism may be supported.
    0 > TCP
    */
    socketFD = socket(AF_INET, SOCK_STREAM, 0);

    bzero((void *) &serverINETAdress, sizeof(serverINETAdress));

    serverINETAdress.sin_family = AF_INET;       /* Internet domain */
    serverINETAdress.sin_addr.s_addr = serverIP; /* IP adress of server */
    serverINETAdress.sin_port = htons((short)portNumber);

    fprintf(stderr, "Connecting to: %s %d\n", inet_ntoa(serverINETAdress.sin_addr), portNumber);

    /* Try to connect until number of TRIES is exceeded */
    bool connected = false;
    while(!connected){

        /* int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);*/
        if(connect(socketFD, (struct sockaddr *)&serverINETAdress, sizeof(serverINETAdress)) < 0){
            printf("ERROR, could not connect to server\n");

            //Increase atmepts and check that not TRIES amount failed.
            attempts++;
            if(attempts >= TRIES){
                return 1; //Failed
            }            

        }

        else{
            return socketFD; // only if successful
        }
    }
    return 1;
}

