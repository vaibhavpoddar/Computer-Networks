#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
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
#include <sys/wait.h>
typedef int ERROR_CODE;
#define FILE_SIZE 20
#define BUFF_SIZE 20
int main()
{
	int C1, C2, opt, S2, a2, size_of_packet, flag=0;
	if( (C1 = socket(AF_INET, SOCK_STREAM, 0) ) <0 )
	{	perror("Cant create socket.\n"); exit(0); }

	opt = 1;
	if( setsockopt(C1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt Error......");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in SA , SA2, CA2;
	SA.sin_family 	   = AF_INET;
	SA.sin_port        = htons(20001);
	inet_aton("127.0.0.1", &SA.sin_addr);

	if( connect(C1, (struct sockaddr*)&SA, sizeof(SA)) < 0 ){
		perror("Unable to connect");
		exit(0);
	}


	// Reading File name from user
	char buf[105], file_name[FILE_SIZE], ch;
	printf("Enter the file name:\n");
	read(0, file_name, FILE_SIZE);
	file_name[strlen(file_name)-1]='\0';

	// Sending File name to Server
	send(C1, file_name, strlen(file_name)+1, 0);
	
	// Receiving E/L
	int TOTAL_PACKETS=0, LAST_BLOCK_SIZE=0;
	recv(C1, &ch, 1 , MSG_WAITALL);
	if(ch == 'E'){
		printf("Error: File does not exist.\n");
		close(C1);
		return 0;
	}
	else if(ch=='L'){

		// Receiving Total File Size
		int No_of_packets=0,size=0;
		recv(C1, &size, sizeof(int), MSG_WAITALL);
		if(size!=0){
			if(size % BUFF_SIZE == 0)
			{
				No_of_packets = (size/BUFF_SIZE);
				LAST_BLOCK_SIZE = BUFF_SIZE;
				TOTAL_PACKETS = No_of_packets;
			}
			else
			{
				No_of_packets = (size/BUFF_SIZE)+1; 
				LAST_BLOCK_SIZE = size%BUFF_SIZE;
				TOTAL_PACKETS = No_of_packets;
			}
		}

		// Receiving and Writing to file
		int F = open(file_name, O_WRONLY | O_CREAT, 0644);
		while(No_of_packets > 1){
			recv(C1, buf, BUFF_SIZE, MSG_WAITALL);
			write(F, buf, BUFF_SIZE);
			No_of_packets--;
		}
		if(No_of_packets == 1)
		{	recv(C1, buf, size%BUFF_SIZE, MSG_WAITALL);
			write(F, buf, size%BUFF_SIZE);
		}		
		printf("Total number of blocks received = %d, Last block size = %d\n", TOTAL_PACKETS, LAST_BLOCK_SIZE);
		close(F);
		close(C1);	
		return 0;
	}
}