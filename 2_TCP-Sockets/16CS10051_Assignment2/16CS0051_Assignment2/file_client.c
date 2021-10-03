// NAME: VAIBHAV PODDAR
// ROLL NO: 16CS10051
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> 

#define BUFF_SIZE 6

int count_words(char * A, int n, int prev){ 
// prev=1 means current is in continuation with previous word.
// prev=0 means current will give new word.
	int state=0;
	int counter=0;
	for (int i = 0; i < n; ++i)
	{
		if(state==0)
		{	
			if(A[i]==' ' || A[i]=='\t' || A[i]=='\n' || A[i]=='.' || A[i]==';' || A[i]==',')
				state=1;
			else
				{
					if(prev && state==0)
						{prev=0;state=2;}
					else
						{state=2;counter++;}
				}
		}
		else if(state==1)
		{
			if(A[i]==' ' || A[i]=='\t' || A[i]=='\n' || A[i]=='.' || A[i]==';' || A[i]==',')
				state=1;
			else
				{state=2;counter++;}
		}else if(state==2)
		{
			if(A[i]==' ' || A[i]=='\t' || A[i]=='\n' || A[i]=='.' || A[i]==';' || A[i]==',')
				state=1;
			else
				state=2;
		}
	}
	return counter;
}

int main()
{
	int			sockfd ;
	struct sockaddr_in	serv_addr;

	int i;
	

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Unable to create socket\n");
		exit(0);
	}

	serv_addr.sin_family	= AF_INET;
	serv_addr.sin_port	= htons(20000);
	inet_aton("127.0.0.1", &serv_addr.sin_addr);
	
	// client establishes a connection to the server
	if ((connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0) {
		perror("Unable to connect to server\n");
		exit(0);
	}

	int r_bytes;
	int counter=0;
	char input[100];
	char buf[BUFF_SIZE+1];
	char doesnotexist[]="File Not Found";
	
	// client reads the filename from the user
	read(0,input,100);
	// client sends the file name to the server
	send(sockfd, input, strlen(input)-1, 0);
	

	// if size of received bytes is 0 then file does not exist
	memset(buf,'\0',sizeof(buf));
	// r_bytes will be 0 when server closes the connection and also when file is empty
	buf[0]='A';
	r_bytes=recv(sockfd, buf, BUFF_SIZE,0);
	if(r_bytes==0 && buf[0]=='A')
	{	//if file is not found
		write(2,doesnotexist, sizeof(doesnotexist));
		return 0;
	}
	else
	{
		// running count of the number of bytes
		// running count of the number of words in the file
		long int total_bytes=0, total_words=0;
		int n,prev=0;
		
		int new_file = open(input, O_WRONLY | O_APPEND | O_CREAT, 0644);
		close(new_file);
		new_file = open(input, O_WRONLY);
		
		write(new_file, buf, strlen(buf));
		total_bytes+=strlen(buf);
		total_words+=count_words(buf,strlen(buf),0);
		n=strlen(buf);
		if(buf[n-1]==' ' || buf[n-1]=='\t' || buf[n-1]=='\n' || buf[n-1]=='.' || buf[n-1]==';' || buf[n-1]==',')
			prev=0;
		else
			prev=1;
		memset(buf,'\0',sizeof(buf));
		
		// client reads the file contents sent by the server and copies it into a new file
		while((r_bytes=recv(sockfd, buf, BUFF_SIZE, 0))>0){
			write(new_file, buf, strlen(buf));
			total_bytes+= strlen(buf);
			total_words+= count_words(buf,strlen(buf),prev);
			n = strlen(buf);
			if(buf[n-1]==' ' || buf[n-1]=='\t' || buf[n-1]=='\n' || buf[n-1]=='.' || buf[n-1]==';' || buf[n-1]==',')
				prev = 0;
			else
				prev = 1;
			memset(buf,'\0',sizeof(buf));
		}

		// After the entire file is transferred
		// client prints a message
		write(1, "The file transfer is successful. Size of the file = ", 52);		
		char byte_buf[20];
		sprintf(byte_buf, "%ld", total_bytes);
		write(1,byte_buf, strlen(byte_buf));
		write(1," bytes, no. of words = ",23);
		memset(byte_buf, '\0' , sizeof(byte_buf));
		sprintf(byte_buf, "%ld", total_words);
		write(1,byte_buf, strlen(byte_buf));

		close(new_file);
	}
	close(sockfd);
	return 0;
}


// r_bytes=	recv(sockfd, buf, 100, 0)
// 			send(sockfd, input, strlen(input)-1, 0);
// use strlen instead of sizeof() in write() systemcall