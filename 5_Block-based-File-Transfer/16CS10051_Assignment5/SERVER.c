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
#include <dirent.h>
#include <errno.h>
#define BUFF_SIZE 20
#define FILE_SIZE 20
int	main()
{

	int S1,C1;
	int a,opt;
	if( (S1 = socket(AF_INET, SOCK_STREAM, 0) ) <0 )
	{	perror("Cant create socket.\n"); exit(0); }

	struct sockaddr_in SA,CA;
	SA.sin_family 	   = AF_INET;
	SA.sin_addr.s_addr = INADDR_ANY;
	SA.sin_port        = htons(20001);

	opt = 1;
	if( setsockopt(S1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt Error......");
		exit(EXIT_FAILURE);
	}
	

	if( bind(S1, (struct sockaddr *)&SA, sizeof(SA)) < 0 )
	{	perror("Unable to bind local address\n"); exit(0); }
	a = sizeof(CA);
	listen( S1, 5);
	printf("Waiting for connection....\n");
	C1 = accept(S1, (struct sockaddr *)&CA, &a);
	if(C1<0)
		printf("Not accepted.\n");

	// Receiving file name
	char ch, file_name[FILE_SIZE];
	int i=0;
	while(recv(C1, &ch, 1, 0)){
		if(ch!='\0')
		{
			file_name[i]=ch;
			i++;
			continue;
		}else{
			file_name[i]='\0';
			break;
		}
	}
	printf("File name received form client: %s\n",file_name );

	// Opening that file
	int f = open(file_name, O_RDONLY);
	if(f<0){
		// If file not present
		ch = 'E';
		send(C1, &ch , 1, 0);
		printf("File not present.\n");
	}else{
		// If file is present
		// Sending L
		ch = 'L';
		send(C1, &ch , 1 , 0);
	
		// Calculating and Sending File size
		char buf[BUFF_SIZE];
		int size = 0,x = 0, No_of_packets = 0;
		while(x = read(f, buf, BUFF_SIZE)) { size += x; }
		close(f);
		send(C1, &size, sizeof(int), 0);

		// Sending Blocks
		printf("Sending Blocks..\n");
		f = open(file_name , O_RDONLY);
		if(size % BUFF_SIZE == 0)
			No_of_packets = (size/BUFF_SIZE);
		else
			No_of_packets = (size/BUFF_SIZE)+1;
		while(No_of_packets){
			read(f, buf, BUFF_SIZE);
			send(C1, buf, BUFF_SIZE, 0);
			No_of_packets--;
		}
		close(f);
		printf("File Transfered Successfully.\n");
	}
	close(C1);
	close(S1);
	return 0;	
}


		// Socket:
			// send(C1, buf, buf_len, 0);
			// recv(C1, &ch, 1, 0)
		// File manipulation:
			// f = open(file_name , O_RDONLY);
			// x = read(f, buf, BUFF_SIZE)
			// 