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

#define BUFF_SIZE 100
// function to find maximum of two numbers
int max(int a, int b){
	return a>b? a:b;
}
int main()
{
	char buf[100];
	memset(buf, '\0', sizeof(buf));
	int s_id, news_id, client_size;
	int us_id, unews_id;
	
	struct sockaddr_in SA,CA;
	struct sockaddr_in uSA,uCA;
	SA.sin_family 		= AF_INET;
	uSA.sin_family    	= AF_INET;
	SA.sin_addr.s_addr 	= INADDR_ANY;
	uSA.sin_addr.s_addr = INADDR_ANY; 
	SA.sin_port 		= htons(6000);
    uSA.sin_port 		= htons(5000);  
	
	// Creating Socket
	s_id  = socket(AF_INET, SOCK_STREAM, 0);
    us_id = socket(AF_INET, SOCK_DGRAM, 0);
	if ((s_id || us_id) < 0) { printf("Socket Creation Failed\n"); exit(0); }
	
	// binding both the sockets
	int b1=bind(s_id,  (struct sockaddr *) &SA,  sizeof(SA) );
	int b2=bind(us_id, (struct sockaddr *) &uSA, sizeof(uSA));
	if (b1<0 ||b2<0) { printf("Unable to bind\n"); exit(0); }
	
	// This id for TCP
	listen(s_id, 5);

	fd_set readfs;
	int nfds;
	int r,read_size;
	printf("Server Running...\n");
	while(1){
		FD_ZERO(&readfs);
		FD_SET(s_id,	&readfs);
		FD_SET(us_id, 	&readfs);
		nfds = max(s_id,us_id)+1;
		
		r=select(nfds, &readfs, 0,0,0);
		if(r<0){perror("No more connection.\n");exit(1);}
		
		if(FD_ISSET(s_id, 	&readfs)) 	// checking for TCP request
		{
			client_size = sizeof(CA);
			news_id = accept(s_id, (struct sockaddr *) &CA, &client_size) ;
			
			if(news_id<0){printf("Error\n");}
			
			int child = fork();
			if(child==0){

				printf("TCP Client Request.\n");
				int f=open("word.txt",O_RDONLY);
				while(1)
				{
					memset(buf,'\0',sizeof(buf));
					read_size = read(f, buf, BUFF_SIZE);
					for (int i = 0; i < BUFF_SIZE; ++i)
					{
						if(buf[i]=='\n')
							buf[i]='\0';
					}
					send(news_id, buf, BUFF_SIZE, 0);
					if(read_size==0)
						break;
				}
				close(news_id);
			}	
			close(news_id);
		}
		if(FD_ISSET(us_id, &readfs)) 	// checking for UDP request
		{
			int child = fork();
			if(child==0)
			{
				struct hostent *he;
		   	 	struct in_addr **addr_list;
				struct hostent * h;
				socklen_t len; 

				len = sizeof(CA);
			   	memset(buf,'\0',sizeof(buf));
			   	recvfrom(us_id, buf, BUFF_SIZE, 0, (struct sockaddr*)&uCA, &len);
			    
			    printf("UDP Client Request: %s\n",buf);
			    
			    h=gethostbyname(buf);
			    if(h!=NULL)
				    strcpy(buf,(char*)inet_ntoa( *((struct in_addr *)((h->h_addr_list)[0])) ));		    
			    else
			    	strcpy(buf,"Not Found!!");
			   
			    sendto(us_id, buf, strlen(buf)+1, 0, (struct sockaddr*)&uCA, sizeof(uCA));
				close(unews_id);
				exit(0);
			}
		}
	}
	return 0;
}