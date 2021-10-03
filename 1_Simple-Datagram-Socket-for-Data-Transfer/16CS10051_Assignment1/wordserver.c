// NAME: VAIBHAV PODDAR
// ROLL: 16CS10051
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
# define WORDSIZE 1024
# define MAXSIZE 1024
int main(){

    // Create socket file descriptor 
	int s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s<0){
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverside, clientside;
	socklen_t len;
	memset(&serverside, 0, sizeof(serverside));
	
	serverside.sin_family = AF_INET;
	serverside.sin_port = htons(8181);
	serverside.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket with the server address 
	if(	bind(s, (struct sockaddr*)&serverside, sizeof(serverside))<0 )
	{
		perror("Binding failed");
		exit(EXIT_FAILURE);
	}

	printf("\nServer running....\n");

	FILE *fp;
	char message[MAXSIZE];
	char word[WORDSIZE];
	int n;

	len=sizeof(clientside);
	n=recvfrom(s, message, MAXSIZE, 0, (struct sockaddr*)&clientside, &len);
	message[n]='\0';

	
	fp = fopen(message, "r");
	
	//if find is not present
	if(fp==NULL){
		char notfound[10] = "NOTFOUND";
		sendto(s, notfound, strlen(notfound), 0, (struct sockaddr*)&clientside, sizeof(clientside));
		printf("Send: NOTFOUND\n");
	}else{
		while(1){
			fscanf(fp, "%s", word);
			// sending the read word from the file to the client
			sendto(s, word, strlen(word), 0, (struct sockaddr*)&clientside, sizeof(clientside));
			printf("Send: %s\n\n", word);
			len=sizeof(clientside);
			memset(message,'\0',sizeof(message));
			// checking the word is END or not
			if(strcmp(word,"END")!=0)
			{	//receiving the message from the client
				recvfrom(s, message, MAXSIZE, 0, (struct sockaddr*)&clientside, &len);
				printf("(client):%s\n",message);
			}
			else{
				break;
			}
		}
	}
	return 0;
}