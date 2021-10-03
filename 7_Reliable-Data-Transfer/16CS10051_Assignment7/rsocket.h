// What is P and dropMessage(float p)? which p is what?
// What is the use of so many arguments in r_recvfrom

#include <time.h>
#include <pthread.h>
#include <netinet/in.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#define T 2
#define SI ((int)sizeof(int))
#define MAX_ID 	1000
#define MAX_MSGS 100
#define P 0.45
#define SOCK_MRP 10051

int r_socket(int domain, int type, int protocol);
int r_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t r_recvfrom(int sockfd, void *buf, size_t l, int f, struct sockaddr *src_addr, socklen_t *addrlen);
int r_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
int r_close(int fd);

void HandleRetransmit(int s);
void HandleAppMsgRecv(void * buf, struct sockaddr* servaddr, int s);
void HandleACKMsgRecv(int id);
int retransmission_count();

void* XJob(void * a);

void* new_ack(int id);
void* new_buffer(const void* buf, size_t len, int id);

void add_to_buffer(void* buf, struct sockaddr * recv_sockaddr);

int dropMessage(float p);
int min(int a, int b);
