// NAME: VAIBHAV PODDAR
// ROLL NO: 16CS10051

/* 
		for IP address requests
*/
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <netinet/in.h> 
#define BUFF_SIZE 100  

int main() { 
    int                     n;
    int                     us_id; 
    struct sockaddr_in      SA; 
    socklen_t               len;
    char BUF[BUFF_SIZE];
    
    memset(BUF, '\0', sizeof(BUF));
    memset(&SA,   0 ,  sizeof(SA)); 

    SA.sin_family       = AF_INET; 
    SA.sin_port         = htons(5000); 
    SA.sin_addr.s_addr  = INADDR_ANY; 
  
    us_id = socket(AF_INET, SOCK_DGRAM, 0);
    if ( us_id < 0 ) { perror("socket creation failed"); exit(EXIT_FAILURE);} 
    
    //Enter Different domain
    printf("Enter Domain Name:");
    scanf("%s",BUF);
    
    sendto(us_id, BUF, strlen(BUF), 0, (struct sockaddr *) &SA, sizeof(SA));    
    len = sizeof(SA);
    recvfrom(us_id, BUF, BUFF_SIZE, 0, (struct sockaddr*)&SA, &len);
    printf("IP Address:%s\n",BUF);
    
    close(us_id); 
    return 0; 
}