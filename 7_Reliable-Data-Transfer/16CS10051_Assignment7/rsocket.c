#include "rsocket.h"
struct Un_ack_msg{
	int 	valid;
	int 	msg_id;
	char 	msg[109];
	int 	msg_size;
	time_t 	timestamp;
	int 	flags;
	struct sockaddr sa;
	socklen_t l;
};
struct recv_msg_id{
	int valid;
	int id;
};
struct recv_buf
{
	int len;
	void * buf;
	struct sockaddr sa;
	struct recv_buf *next;
};

// int s;

int UNIQUE_ID;
pthread_t X;
pthread_mutex_t lock;

// unacknowledged_message_table
struct Un_ack_msg * UAMT;

// received_message_id_table
struct recv_msg_id * RMIT;

// receive_buffer
struct recv_buf * RB;
struct recv_buf * RB_last;
int is_close;

int retrans_count;

int r_socket(int domain, int type, int protocol){
	if(type!=SOCK_MRP)
		return -1;
	srand(time(0));
	UAMT = (struct Un_ack_msg*)malloc(MAX_MSGS*sizeof(struct Un_ack_msg));
	RMIT = (struct recv_msg_id*)malloc(MAX_MSGS*sizeof(struct recv_msg_id));	
	RB   = NULL;
	RB_last = NULL;
	UNIQUE_ID = 0;
	is_close = 0;
	retrans_count=0;
	// send_addr not used
	for (int i = 0; i < MAX_MSGS; ++i)
	{
		UAMT[i].valid=0;
		RMIT[i].valid=0;
	}

	if (pthread_mutex_init(&lock, NULL) != 0) 
        return -1; 

	int s = socket(domain, SOCK_DGRAM, protocol);
	if ( s < 0 )
        return -1;

    pthread_attr_t attr;
    pthread_attr_init (&attr);
    int* param = (int*)malloc(sizeof(int));
    *param = s;
    int status = pthread_create(&X, &attr, XJob, (void*)param); 
    if(status<0)
    	return -1;
    return s;
}
int r_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
	// if(s!=sockfd)
	// 	return -1;
	if( bind(sockfd, addr, addrlen) < 0 )
		return -1;
	return 1;
}
ssize_t r_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
	int l,i,r;
	while(1){
		pthread_mutex_lock(&lock);
		if(RB!=NULL){
			l = min(len, RB->len);
			for (int i = 0; i < l; ++i)
			{
				((char*)buf)[i] = ((char*)(RB->buf))[i];
			}
			*src_addr = RB->sa;
			
			struct recv_buf * temp;
			temp = RB;
			RB= RB->next;
			if(RB==NULL)
				RB_last = NULL;					
			
			free(temp);
			pthread_mutex_unlock(&lock);
			return l;
		}
		pthread_mutex_unlock(&lock);
		if(is_close==-1){
			break;
		}
		// printf("sleeping..\n");
		sleep(1);
	}
	return -1;
}
int r_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen){
	pthread_mutex_lock(&lock);
	for (int i = 0; i < MAX_MSGS; ++i)
	{
		if(UAMT[UNIQUE_ID].valid==0)
		{
			void* n_buf = new_buffer(buf, len, UNIQUE_ID);
			UAMT[UNIQUE_ID].valid = 1;
			UAMT[UNIQUE_ID].msg_id = UNIQUE_ID;
			for (int i = 0; i < 9+len; ++i)
				UAMT[UNIQUE_ID].msg[i] = ((char*)n_buf)[i];
			UAMT[UNIQUE_ID].msg_size 	= 9+len;
			UAMT[UNIQUE_ID].timestamp 	= time(0);
			UAMT[UNIQUE_ID].flags		= flags;
			UAMT[UNIQUE_ID].sa			= *(dest_addr);
			UAMT[UNIQUE_ID].l 			= addrlen;
			
			UNIQUE_ID++;
			UNIQUE_ID = UNIQUE_ID%MAX_ID;
			sendto(sockfd, n_buf, 9+len, flags, dest_addr, addrlen);
			// printf("New message added to UAMT and send.\n");
			pthread_mutex_unlock(&lock);
			return 1;
		}
		pthread_mutex_unlock(&lock);
	}
	return -1;
}
int r_close(int fd){
	// pthread_mutex_lock(&lock);
	for (int i = 0; i < MAX_MSGS; ++i)
	{
		if(UAMT[i].valid==1){
			sleep(1);
			i=-1;
		}
	}
	// pthread_mutex_unlock(&lock);
	pthread_cancel(X);
	free(UAMT);
	free(RMIT);
	struct recv_buf * temp;
	while(RB!=NULL){
		temp = RB;
		RB=RB->next;
		free(temp);
	}
	pthread_mutex_destroy(&lock);
	close(fd);
	return 1;
}

void* XJob(void * a){
	int s = *((int*)a);
	// printf("Inside thread%d\n", s);
	fd_set A;
	int retval;
	struct timeval tv;
	socklen_t recv_addr_len;
	int ack_id;
	int done;
	struct sockaddr_in client;
	while(1){
		FD_ZERO(&A);
		FD_SET(s, &A);
		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		retval = select(s+1, &A, 0, 0, &tv);
		if(retval==-1){
	   		perror("select error");
	   	}else if(retval ==0){
	   		// printf("s:%d",s);
	   		// printf("Timeout\n");
	   		HandleRetransmit(s);
	   	}else if(FD_ISSET(s, &A)){
	   		int len = 109;
			void * buff = (void*)malloc(len);
			recv_addr_len = sizeof(client);
			is_close = recvfrom(s, buff,  len, 0, (struct sockaddr*)&client, &recv_addr_len);
			if(dropMessage(P)==1)
				{	
					// if(((char*)buff)[0]=='0')
					// 	printf("ACK lost[%d]\n",*((int*)(((char*)buff)+1)));
					// else
					// 	printf("MSG lost[%d][%c]\n", *((int*)(((char*)buff)+1)), ((char*)(((char*)buff)+9))[0]);
					continue;
				}
			if(((char*)buff)[0]=='0'){
				ack_id = *((int*)(((char*)buff)+1));
				// printf("ACK [%d]..\n",ack_id);
				HandleACKMsgRecv(ack_id);
			}else if(((char*)buff)[0]=='1'){	
				HandleAppMsgRecv(buff, (struct sockaddr*)&client, s);
			}
	   	}
	}
}

void HandleRetransmit(int s){
	time_t curr = time(0);
	pthread_mutex_lock(&lock);
	for (int i = 0; i < MAX_MSGS; ++i)
	{	curr  = time(0);
		if(UAMT[i].valid == 1 && difftime(curr, UAMT[i].timestamp)>=T){
			printf("Resending[%d][%c]\n",i,(UAMT[i].msg)[9]);
			retrans_count++;
			sendto(s, (char*)(UAMT[i].msg), UAMT[i].msg_size, UAMT[i].flags, &(UAMT[i].sa), UAMT[i].l);
			UAMT[i].timestamp = time(0);
		}
	}
	pthread_mutex_unlock(&lock);
}
void HandleAppMsgRecv(void * buf, struct sockaddr* servaddr, int s){
	int id = (*(int*)((void*)(((char*)buf)+1)));
	if(id>=MAX_ID)
		perror("ID error: exceeded the range!!");
	if(RMIT[id].valid==1){
		// printf("APP-IV[%d]..X\n",id);
		void* ack_msg= new_ack(id);
		sendto(s, ack_msg, 5, 0, servaddr, sizeof(*servaddr));
	}else{
		// printf("APP-V[%d]..\n",id);
		RMIT[id].valid = 1;
		RMIT[id].id    = id;
		add_to_buffer(buf, servaddr);
		void* ack_msg= new_ack(id);
		sendto(s, ack_msg, 5, 0, servaddr, sizeof(*servaddr));
	}
}
void HandleACKMsgRecv(int id){
	if(UAMT[id].msg_id==id && UAMT[id].valid==1){
		UAMT[id].valid  = 0;
		UAMT[id].msg_id = -1;
		// printf("ACKNOWLEDGED!! [%d]\n",id);
	}else{
		// printf("DROPPED!! [%d]\n", id);
	}
}

void* new_ack(int id){
	int   LEN 	= 1 + SI;
	void* b 	= (void*)malloc(LEN);
	
	((char*)b)[0] = '0';
	*((int*)((void*)(((char*)b)+1))) = id;
	return b;
}

void* new_buffer(const void* buf, size_t len, int id){
	int   LEN 	= 9 + (int)len;
	void* b 	= (void*)malloc(LEN);
	
	((char*)b)[0] = '1';
	*((int*)((void*)(((char*)b)+1))) = id;
	*((int*)((void*)(((char*)b)+1+SI))) = len;
	
	for (int i = 9; i < LEN ; ++i)
	{
		((char*)b)[i] = ((char*)buf)[i-9];
	}
	return b;
}

void add_to_buffer(void* buf, struct sockaddr * recv_sockaddr){
	struct recv_buf* node = (struct recv_buf*)malloc(sizeof(struct recv_buf));
	node->len = *((int*)(((char*)buf)+5));
	node->buf = (void*)malloc(node->len);
	for (int i = 0; i < node->len; ++i)
	{
		((char*)(node->buf))[i] = ((char*)(((char*)buf)+9))[i];
	}
	node->sa   = *recv_sockaddr;
	node->next = NULL;
	pthread_mutex_lock(&lock);
	if(RB==NULL){
		RB=node;
		RB_last=node;
	}else{
		RB_last->next=node;
		RB_last=node;
	}
	pthread_mutex_unlock(&lock);
	return;
}

int dropMessage(float p){
	float f = ((double)((1.0*rand())/RAND_MAX));
	// printf("%f\n", f);
	if( f < p)
		return 1;
	else
		return 0;
}

int min(int a, int b){
	if(a<b)
		return a;
	return b;
}

int retransmission_count(){
	return retrans_count;
}