#include "rsocket.h"
int main() { 
    int s; 
    struct sockaddr_in S, S2; 
      
    s = r_socket(AF_INET, SOCK_MRP, 0);
    if ( s < 0 ) {
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    }
    memset(&S, 0, sizeof(S));
    memset(&S2, 0, sizeof(S2));  


    S2.sin_family        = AF_INET;
    S2.sin_addr.s_addr   = INADDR_ANY;
    S2.sin_port          = htons(50000+(2*1005)+1); //Roll_no:16CS10051, taking it as 1005 
    
    S.sin_family        = AF_INET;
    S.sin_addr.s_addr   = INADDR_ANY;
    S.sin_port          = htons(50000+(2*1005)); //Roll_no:16CS10051, taking it as 1005 
      

    if ( r_bind(s, (const struct sockaddr *)&S2, sizeof(S2)) < 0 ) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    char * Enter = "Enter the string\n";
    char buff[200];
    memset(buff,'\0',sizeof(buff));
    write(1,Enter,strlen(Enter));
    read(0,buff,100);
    int length = strlen(buff)-1;
    for (int i = 0; i < length; ++i)
    {
    	printf("sending: [%c]\n", (buff[i]));
        r_sendto(s, (char*)(&(buff[i])), sizeof(char), 0, (const struct sockaddr *) &S, sizeof(S) );
    }
    r_close(s);

    printf("Average Number of Retransmission:%f\n",(float)(retransmission_count()/(1.0*length)));
    return 0; 
} 
