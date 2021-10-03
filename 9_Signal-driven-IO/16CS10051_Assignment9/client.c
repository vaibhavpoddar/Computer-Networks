#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <netinet/in.h> 
#define BUFF_SIZE 100  

int main() { 

    int                     s; 
    struct sockaddr_in      SA; 
    socklen_t               len;
    char BUF[BUFF_SIZE];
    
    memset(BUF, '\0', sizeof(BUF));
    memset(&SA,   0 ,  sizeof(SA)); 

    SA.sin_family       = AF_INET; 
    SA.sin_port         = htons(5000); 
    SA.sin_addr.s_addr  = INADDR_ANY; 
  
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if ( s < 0 ) { perror("socket creation failed"); exit(EXIT_FAILURE);} 
    
    write(1, "Enter the string:", 17);
    read(0, BUF, sizeof(BUF));
    BUF[strlen(BUF)-1]='\0';

    write(1, "Sending...\n", 11);
    sendto(s, BUF, strlen(BUF), 0, (struct sockaddr *) &SA, sizeof(SA));    
    len = sizeof(SA);

    write(1, "Received: ", 10);
    recvfrom(s, BUF, BUFF_SIZE, 0, (struct sockaddr*)&SA, &len);
    write(1, BUF, strlen(BUF));
    close(s); 
    return 0; 
}