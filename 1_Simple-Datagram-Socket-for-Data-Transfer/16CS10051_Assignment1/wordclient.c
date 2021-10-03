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
# define LIMIT 100
int main(){
	// Creating socket file descriptor 
	int s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s<0){
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serverside;
	memset(&serverside, 0, sizeof(serverside));

	// Server information
	serverside.sin_family = AF_INET;
	serverside.sin_port = htons(8181);
	serverside.sin_addr.s_addr = INADDR_ANY;

	char message[LIMIT];
	socklen_t len;

	//Accepting the file Name
	char fname[50];
	printf("Enter the file name:\n");
	scanf("%s",fname);
	sendto(s, fname, strlen(fname), 0, (struct sockaddr*)&serverside, sizeof(serverside));
	
	len = sizeof(serverside);
	int n;
	n=recvfrom(s, message, LIMIT, 0, (struct sockaddr*)&serverside, &len);
	message[n]='\0';
	if(strcmp(message,"NOTFOUND")==0){	//when file is not present in local directory
		printf("File Not Found\n");		//client prints an error message
		close(s);
		return 0;
	}

	// when file is present
	// HELLO is returned from server
	if(strcmp(message,"HELLO")==0){		
		printf("(server): %s\n", message);
		FILE * newfile;
		newfile = fopen("Newfile.txt","a+");	//creating new local file 
		char input[LIMIT];
		
		while(1){
			printf("\n(client): ");
			scanf("%s",input);
			sendto(s, input, strlen(input), 0, (struct sockaddr*)&serverside, sizeof(serverside)); // sending the word number to the server
			memset(message,'\0',sizeof(message));
			n=recvfrom(s, message, LIMIT, 0, (struct sockaddr*)&serverside, &len); //receiving the message from the server
			message[n]='\0';
			printf("(server): %s\n", message);
			if(strcmp(message,"END")!=0)		//clint does not received the keyword END
			{	
				fprintf(newfile, "%s\n", message);	//adding content to new file
				printf("****Word added!!****\n");	
				fflush(newfile);
				continue;
			}		
			else{								//clint received the keyword END
				fclose(newfile);				//closing the new file
				break;
			}
		}

	}

	close(s);
	return 0;
}