#include "rsocket.h"  

int main() { 
    int s; 
    struct sockaddr_in S,C; 
  
    s = r_socket(AF_INET, SOCK_MRP, 0);
    if ( s < 0 ) {
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }

    memset(&S, 0, sizeof(S)); 
    memset(&C, 0, sizeof(C));  
    
    S.sin_family      = AF_INET; 
    S.sin_port        = htons(50000+(2*1005)); //Roll_no:16CS10051, taking it as 1005 
    S.sin_addr.s_addr = INADDR_ANY; 

    if ( r_bind(s, (const struct sockaddr *)&S, sizeof(S)) < 0 ) 
    { 
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len,n;
    char buffer[100];
    while(1){

        memset(buffer, '\0', sizeof(buffer));
        len = sizeof(C);
        write(0,"Waiting...\n",11);
        n = r_recvfrom(s, (char *)buffer, 10, 0, (struct sockaddr *)&C, &len);
        if(n!=-1){
            printf("user1:[%s]\n", buffer);
        }
    }
    r_close(s); 
    return 0; 
} 
