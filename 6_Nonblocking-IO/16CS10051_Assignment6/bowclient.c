// NAME: VAIBHAV PODDAR
// ROLL NO: 16CS10051

/* 
		for bag of words requests
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#define BUFF_SIZE 60

int main()
{
	int					sockfd ;
	struct sockaddr_in	serv_addr;

	// creating socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}

	serv_addr.sin_family	= AF_INET;
	serv_addr.sin_port	= htons(6000);
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	
	// client establishes a connection to the server
	if ((connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	int flag=0;
	char buf[BUFF_SIZE+1];
	memset(buf,'\0',sizeof(buf));
	int number_of_words=0;
	int state=0;
	int x;

	while(1){
		x=recv(sockfd, buf, BUFF_SIZE, 0);
		if(x==0)
			break;
		for (int i = 0; i < BUFF_SIZE; ++i)
		{
			if(state==0){ 
				if(buf[i]=='\0') 
					{flag=1;break;} 	//continous \0\0 implies no nore words 
				else if(buf[i]!='\0') 
					state=3;
			}
			else if(state==3){ 
				if(buf[i]=='\0') 
					{number_of_words++;state=4;}
				else if(buf[i]!='\0') 
					state=3;
			}
			else if(state==4){ 
				if(buf[i]=='\0') 
					{flag=1;break;}
				else if(buf[i]!='\0')
					state=3;
			}
		}
		if(flag==1)
		{
			break;
		}
	}
	// printing total number of words
	printf("Total Words: %d\n",number_of_words);
	close(sockfd);
	return 0;
}