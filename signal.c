#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void sig_handler(int signo);
static void usr1b(int signo);
static void usr1a(int signo);
static void another_handler(int sig, siginfo_t *si, void *context);


int main(void){

	struct sigaction sa;

	//When SIGINT it received, execute sig_handler
	if(signal(SIGINT, sig_handler) == SIG_ERR){ // subscribe sig_handler to SIGINT
		//printf("\ncan’t catch SIGINT\n");
	}

	//When SIGINT it received, execute usr1a the first time.
	if(signal(SIGUSR1, usr1a) == SIG_ERR){ // subscribe sig_handler to SIGINT
		//printf("\ncan’t catch SIGINT\n");
	}

	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = &another_handler ;

	//When SIGHUP it received, execute sa -> another_handler
	if(sigaction(SIGHUP, &sa, NULL) == -1){ // same thing in more complicated for SIGHUP
		//printf("\ncan’t catch SIGHUP\n" );
	}

	printf("Pid: %d\n" ,getpid());

	// A long long wait so that we can easily issue a signal to this process
	while (1) {
		sleep (1) ;
	}

	return 0;
}


static void another_handler(int sig, siginfo_t *si, void *context){
	printf("received signal %d\n" , sig ) ;
}


static void sig_handler(int signo){
	if(signo == SIGINT){
		printf("received SIGINT\n");
	}
}


//Exectuted the scound time you run SIGUSR1
static void usr1b(int signo){
	if(signo == SIGUSR1){
		printf("received SIGUSR1\n");
	}
}


//Exectuted the first time you run SIGUSR1
static void usr1a(int signo){
	if(signo == SIGUSR1){
		printf("Subscribing to usr1b\n");
		signal(SIGUSR1, usr1b); 
	}
}
