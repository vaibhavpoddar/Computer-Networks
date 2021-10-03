#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
struct pair{
	unsigned long IP;
	unsigned short PORT; 
};
int main(){
	int s,ns;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(s<0){
		perror("Scocket Error");
		exit(0);
	}
	struct sockaddr_in S,C;
	
	int opt = 1;
	memset(&S,0,sizeof(S));
	memset(&C,0,sizeof(C));
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT,
                      &opt, sizeof(opt));

	S.sin_family = AF_INET;
	S.sin_port = htons(20000);
	S.sin_addr.s_addr = INADDR_ANY;

	if(bind(s, (struct sockaddr*)&S, sizeof(S))<0){
		perror("Binding error");
		exit(0);
	}	
	listen(s,5);

	fcntl(s, F_SETFL, O_NONBLOCK);
	int clientsockfd[5];
	struct pair clinetId[5];	
	int i=0;
	for(;i<5;i++){
		clientsockfd[i]=-1;	
	}
	int numclient = 0;
	int itr =0;
	int r=-1;
	int J=0;
	char BUF[20]; 
	while(1){
		memset(BUF, '\0' , sizeof(BUF));
		itr =0;
		sleep(1);
		int len = sizeof(C);
		printf("Waiting for accepting..\n");
		ns = accept(s, (struct sockaddr*)&C, &len);
		if(ns<0){
			printf("No new connection yet!! %d\n", numclient);
			
		}else{
			numclient++;
			clientsockfd[numclient-1]=ns;
			printf("New connection--------!!\n");
			printf("Client port:%d",C.sin_port);
			printf("Client address:%d", C.sin_addr.s_addr); 
			clinetId[numclient-1].IP = C.sin_addr.s_addr;
			clinetId[numclient-1].PORT = C.sin_port;
			printf("Received new connection from client <%ld:%d>", clinetId[numclient-1].IP  , clinetId[numclient-1].PORT);
		}
		for(; itr<5;itr++){
			r = -1;
			J=0;
			if(clientsockfd[itr]!=-1){
				r = recv(clientsockfd[itr], BUF, 10, 0);
				if(numclient<=1)
				{
					printf("Server: Insufficient clinets, %s from client <%ld:%d>", clinetId[itr].IP , clinetId[itr].PORT );
					continue;
				}				
				if(r>0){
					for(; J<5; J++){
						if(clientsockfd[J]!=-1 && J!=itr){					
							send(clientsockfd[J], &clinetId[itr].IP, sizeof(unsigned long), 0);
							send(clientsockfd[J], &clinetId[itr].PORT, sizeof(unsigned short), 0);
							send(clientsockfd[J], BUF, strlen(BUF)+1, 0);
							printf("Server: Send message %sfrom client <%ld:%d> to <%ld:%d>", clinetId[itr].IP , clinetId[itr].PORT , clinetId[J].IP , clinetId[J].PORT);
						}
					}			
				}
			}		
		}
		
	}
	close(s);
	return 0;
}
