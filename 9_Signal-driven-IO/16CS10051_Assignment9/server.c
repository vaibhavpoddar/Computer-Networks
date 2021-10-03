/*
If the main is executing inside the critical section 
and a signal occurs here, then if the main's critical section has already 
aquired a lock and there is no more resources(lock) available 
then if the signal handler function also tries to get that lock 
then this will cause the main function and the signal handler 
to get stuck and would result in deadlock.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h> 	// "sockaddr_in" used for server address
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h> 
#include <sys/stat.h>
#include <fcntl.h> 
#include <signal.h>

#define BUFF_SIZE 100

int us_id;
char BUF[100];
struct sockaddr_in uSA,uCA;

void handleSignal()
{
	signal(SIGIO, handleSignal);
	memset(BUF, '\0', sizeof(BUF));
	int len = sizeof(uCA);
    recvfrom(us_id, BUF, BUFF_SIZE, 0, (struct sockaddr*)&uCA, &len);
   	write(1, BUF, sizeof(BUF));
   	sendto(us_id, BUF, strlen(BUF), 0, (struct sockaddr *)&uCA, sizeof(uCA));    
   	write(1, "\n", 1);
}

int main()
{
	signal(SIGIO, handleSignal);

	uSA.sin_family    	= AF_INET;
	uSA.sin_addr.s_addr = INADDR_ANY; 
	uSA.sin_port 		= htons(5000);  
	
	// Creating Socket
	us_id = socket(AF_INET, SOCK_DGRAM, 0);
	if (us_id < 0) { printf("Socket Creation Failed\n"); exit(0); }
	
	// binding the sockets
	int b1=bind(us_id, (struct sockaddr *) &uSA, sizeof(uSA));
	if (b1<0) { printf("Unable to bind\n"); exit(0); }
	
	socklen_t len;
	printf("Server Running...\n");
	if(fcntl(us_id, F_SETOWN, getpid()) < 0)
	{
		perror("fcntl Error (F_SETOWN)");
		exit(1);
	}

	int f = fcntl(us_id, F_GETFL);
	if(fcntl(us_id, F_SETFL, O_ASYNC | f) < 0){
		perror("fcntl Error (F_SETFL)");
		exit(1);	
	}

	while(1){
		sleep(1);
	}

	return 0;
}