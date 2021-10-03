// NAME: VAIBHAV PODDAR
// ROLL NO: 16CS10051
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h> 

#define BUFF_SIZE 15
int main()
{
	int	sockfd, newsockfd ; /* Socket descriptors */
	int	clilen;
	struct sockaddr_in	cli_addr, serv_addr;

	int i;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Cannot create socket\n");
		exit(0);
	}

	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(20000);


	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, 5); 

	int r_bytes;
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen) ;
	if (newsockfd < 0) {
		perror("Accept error\n");
		exit(0);
	}


	char filename[105];

	// server receives the file name from the client
	recv(newsockfd, filename, 105,0);
	int f = open(filename, O_RDONLY);
	if(f<0)
	{	// File not present
		perror("FILE DOES NOT EXIST\n");
		// closing the connection
		close(newsockfd);
		close(f);
		return 0;
	}
	else
	{	// if file is present
		int read_size;
		char buf[BUFF_SIZE];
		memset(buf,'\0',sizeof(buf));
		int count=0;
		while(1)
		{
			memset(buf,'\0',sizeof(buf));
			// readind the content of the file
			//_ _ _ _ \0  if BUFF_SIZE=5
			read_size = read(f, buf, BUFF_SIZE-1);      
			if(read_size>0)
			{
				// sending to client
				// will be 4 generally
				send(newsockfd, buf, strlen(buf), 0);   
				count=1;
			}
			else if(read_size==0 && count==0)			// if file is present but file is empty
			{
				send(newsockfd, buf, strlen(buf), 0);
				break;	
			}
			else
				break;
		}
		// whole file is now sent.
		// closing the connection
		close(newsockfd);
		close(f);
		return 0;
	}
}