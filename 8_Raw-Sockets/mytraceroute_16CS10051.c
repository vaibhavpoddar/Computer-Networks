#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h> 
#include <linux/ip.h> /* for ipv4 header */
#include <linux/udp.h>
#include <linux/icmp.h>
// #include <ip_icmp.h>
// #include <netinet/ip_icmp.h> 
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <signal.h>

#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdio.h> 
#include <stdlib.h>

#include <time.h> 
#include <unistd.h> 

#define MSG_SIZE 2048
#define LISTEN_PORT 8080
#define DATA_SIZE 52
#define LISTEN_IP "10.145.145.68"
// #define LISTEN_IP "0.0.0.0"
int main(int argc, char const *argv[]){
	struct iphdr hdrip;
	struct udphdr hdrudp;
	
	fd_set A;
	int nfd;
	int read_size;
	struct timeval tv;
	char toprint[1000];

	if(argc<2){
		write(1, "Incorrect arguments\n", 20);
		exit(0);
	}

	// Finding out the IP Address
	struct hostent *h;
	h=gethostbyname(argv[1]);
	char destination[20];
	memset(destination, '\0', sizeof(destination));
    if(h!=NULL)
	   	strcpy(destination,(char*)inet_ntoa( *((struct in_addr *)((h->h_addr_list)[0])) ));		    
    else
    	{
    		strcpy(destination,"Not Found!!");
			write(1, "Destination Not Found!!", 23);
			return 0;
		}

	memset(toprint, 0, sizeof(toprint));
	sprintf(toprint, "destination is: %s\n", destination);
	write(1, toprint, strlen(toprint));  			
	
	// Destination 
	struct sockaddr_in router_addr;
	int router_addr_len;

	// Creating the socket
	int S1 = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	int S2 = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if( S1 < 0 || S2 < 0 ){
		perror("raw socket");
		exit(__LINE__);
	}

	// Binding
	int opt = 1;
	if( setsockopt(S2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt Error......");
		exit(EXIT_FAILURE);
	}

	// Set the socket options
	int on=1;
	if(setsockopt(S1, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on))<0){
		perror("setsockopt Error......");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in SA;
	SA.sin_family 	   = AF_INET;
	SA.sin_addr.s_addr = INADDR_ANY;
	SA.sin_port        = htons(20001);
	if( bind(S2, (struct sockaddr *)&SA, sizeof(SA)) < 0 )
	{
			perror("Bind Error!!");
			return -1;
	}

	int TTL = 1, timeout=0, set_Again = 1;
	int retval, recv_addr_len, y=0, x, s, type, code;
	struct iphdr t1;
	struct udphdr t2;
	struct icmphdr t3;
	int packet_size = DATA_SIZE + sizeof(t1) + sizeof(t2);
	struct timeval start,end;
	char P[packet_size], ICMPbuf[1000];
	
	struct sockaddr_in client;
	int iphdrlen = sizeof(struct iphdr);
	while(1){

		// Preparing the UDP packet
		memset(P, 0, sizeof(P));
		struct iphdr  *h_ip = (struct iphdr*)P;
		struct udphdr *h_udp = (struct udphdr*)(P+sizeof(struct iphdr));

		// IP Header
		h_ip->ihl = 5;								// Header Length
		h_ip->version = 4;							// Version
		h_ip->tos = 0;								// Type Of Service (?)
		h_ip->tot_len = packet_size;				// Total Packet Size
		h_ip->id = htonl(100); 						// random Id (?)
		h_ip->frag_off = 0;
		h_ip->ttl = TTL;							// Time To Live
		h_ip->protocol = 17; 						// User Datagram Protocol
		h_ip->check = 0;							// Checksum
		h_ip->saddr = INADDR_ANY; 				// Source Address
		// h_ip->saddr = inet_addr("10.145.145.68"); 	// Source Address
		h_ip->daddr = inet_addr(destination);	// Destination Address
		
		// UDP header
		h_udp->dest = htons(32164);			// Destination Port
		h_udp->source = htons(11111); 		// Source Port (?)
		h_udp->len = htons(8 + DATA_SIZE);	// Length 
		h_udp->check = 0;					// Checksum

		// Destination
		router_addr.sin_family = AF_INET;
		router_addr.sin_port   = htons(32164);
		router_addr.sin_addr.s_addr = inet_addr(destination);
		router_addr_len = sizeof(router_addr);

		// Sending the packet using S1
		// clock_t start = time(NULL);
		gettimeofday(&start, NULL);
		s = sendto(S1, P, packet_size, 0, (struct sockaddr *) &router_addr, router_addr_len);
		if(s<0){
			write(1, "Sendto Error\n", 13);
			continue;
		}else{
			// write(1, "Sent!!\n", 7);
		}

		FD_ZERO(&A);
		FD_SET(S2, &A);
		if(set_Again)
		{	tv.tv_sec  = 1;
			tv.tv_usec = 0;
		}		

		retval = select(S2+1, &A, 0,0,&tv);

		if(retval==-1){
	   		perror("select error");
	   		exit(0);
	   	}else if(retval ==0){
	   		set_Again=1;
	   		timeout++;
	   		// printf("Timeout: %d\n", timeout);
	   		if(timeout==3)
	   		{	
	   			memset(toprint, 0, sizeof(toprint));
	   			sprintf(toprint, "Hop_Count(%d)\t*\t*\n", TTL);
	   			write(1, toprint, strlen(toprint));
	   			// printf("Hop_Count(%d)\t*\t*\n",TTL);
	   			TTL++;timeout=0;
	   		}
	   		continue;
	   	}else if(FD_ISSET(S2, &A)){
	   		memset(ICMPbuf, '\0', sizeof(ICMPbuf));
	   		recv_addr_len = sizeof(client);
	   		x = recvfrom(S2, ICMPbuf,  1000, 0, (struct sockaddr*)&client, &recv_addr_len);	
			
			gettimeofday(&end, NULL);
			unsigned long long diff_time = 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000;
    
			// printf("Start:%ld, End:%ld\n",start, end );
			struct iphdr *h_ip_icmp = (struct iphdr*)ICMPbuf;
			struct icmphdr *h_icmp = (struct icmphdr*)(ICMPbuf+sizeof(struct iphdr));
			if(h_ip_icmp->protocol!=1)	// Not ICMP packet
				continue;

			type = h_icmp->type;
			code = h_icmp->code;
			if(type==11)	// ICMP Time Exceeded Message
			{	
				memset(toprint, 0, sizeof(toprint));
	   			sprintf(toprint, "Hop_Count(%d)\t%s\t%lldmsec\n",TTL,inet_ntoa(client.sin_addr), diff_time);
	   			write(1, toprint, strlen(toprint));
 	   			TTL++;
 	   			continue;
			}
			if(type == 3){ 	// ICMP Destination Unreachable
				memset(toprint, 0, sizeof(toprint));
	   			sprintf(toprint, "Hop_Count(%d)\t%s\t%lldmsec\n",TTL,inet_ntoa(client.sin_addr), diff_time);
	   			write(1, toprint, strlen(toprint));
 	  			// printf("Hop_Count(%d)\t%s\t%lldmsec\n",TTL,inet_ntoa(client.sin_addr), diff_time);
 	   			TTL++;
 	   			if(strcmp(inet_ntoa(client.sin_addr), destination)==0){
 	   				break;
 	   			}
			}
			timeout = 0;
	   	}
	}
	close(S1);
	close(S2);
return 0;
}