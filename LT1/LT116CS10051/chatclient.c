#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#define STDIN 0
int max(int a, int b){
	if(a>b)
		return a;
	else 
		return b;
}
int main(){
	int s,ns;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s<0){
		perror("Scocket Error");
		exit(0);
	}
	struct sockaddr_in C;
		
	memset(&C,0,sizeof(C));

	C.sin_family = AF_INET;
	C.sin_port = htons(20000);
	inet_aton("127.0.0.1",&C.sin_addr);

	if(connect(s, (struct sockaddr*)&C, sizeof(C))<0){
		perror("Connection Error!!");
		exit(0);
	}

	fd_set A;
	int n,x,len;	
	char buf[10];
	unsigned long ip_;
	unsigned short port_;
	while(1){
		memset(buf, '\0', sizeof(buf));
		
		FD_ZERO(&A);
	
		FD_SET(STDIN, &A);
		FD_SET(s, &A);

		n=max(STDIN, s)+1;
		x = select(n,&A,0,0,0);	

		if(FD_ISSET(s,&A)){
			recv(s, &ip_, sizeof(unsigned long ),MSG_WAITALL); 
			recv(s, &port_,sizeof( unsigned short) ,MSG_WAITALL); 
			recv(s, buf , sizeof(buf),MSG_WAITALL);
			printf("Client: Received %s from <%ld:%d>", buf, ip_, port_);
		}
		if(FD_ISSET(STDIN,&A)){
			memset(buf, '\0', sizeof(buf));
			read(STDIN,buf,10);
			send(s, buf, sizeof(buf), 0);
			printf("Client Message %s sent to the server", buf);
		}
	}
	return 0;
}
