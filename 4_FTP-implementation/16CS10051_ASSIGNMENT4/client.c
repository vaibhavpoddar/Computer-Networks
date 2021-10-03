#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
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
#include <signal.h>
#include <sys/wait.h>
typedef int ERROR_CODE;
#define dataF_SIZE 60
#define FILE_NOT_FOUND 550
#define FILE_EXIST 250
#define DATA_SIZE 20
#define CMD_SIZE 500
void  port_manage_code(int code){
	if(code == 503)
		printf("Please give the port!\n");
	else if( code == 200)
		printf("Port saved Successfully!\n");
	else if ( code == 550)
		printf("Incorrect Port!\n");
}
void cd_manage_code(int code){
	if( code == 200)
		printf("Directory changed Successfully!\n");
	else if( code == 501)
		printf("Incorrect Directory!\n");
}
void get_manage_code(int code){
	if( code == 550)
		printf("Either file does not exist, or it cannot be opened for read!\n");
	else if( code == 250)
		printf("Successfully File Transfered!\n");
}
void put_manage_code(int code){

}
void exit_manage_code(int code){

}
char * parse_input(char * Arr, int n){
	char * A = malloc((n+1)*sizeof(char));
	memset(A, '\0', sizeof(A));
	int state=0;
	int k=0,i=0;
	for (int k = 0; k < n; ++k)
	{
		if(state==0){
			if(Arr[k]==' ')
				continue;
			A[i++]=Arr[k];
			state=1;
			continue;
		}
		if(state==1){
			if(Arr[k]==' ')
				{state=2; continue;}
			A[i++]=Arr[k];
		}
		if(state==2){
			if(Arr[k]==' ')
				continue;
			A[i++]=' ';
			A[i++]=Arr[k];
			state=1;
			continue;
		}
	}
	return A;
}
int has_slash(char* A){
	int i=0;
	while(A[i]!='\0')
	{
		if(A[i]=='/')
			return 1;
		i++;
	}
	return 0;
}
char * get_last_name(char * A){
	int i=0,mark=0;
	char* t=(char*)malloc(100*sizeof(char));
	while(A[i]!='\0')
	{	
		if(A[i]=='/')
			mark = i;
		i++;
	}
	i=mark;
	i++;
	t[0]='\0';
	t[1]='\0';
	int j=0;
	while(A[i]!='\0'){
		t[j] = A[i];
		i++;j++;
	}
	t[j]='\0';
	return t;
}
char * get_input()
{	printf(">");
	char * str=(char *)malloc(100*sizeof(char));
	// int x= read(0, str, 100);
	scanf("%[^\n]s",str);
	char c;
	scanf("%c",&c);
	return parse_input(str, strlen(str));
}
int getY(){
	char *token, *Y_string = get_input();
	token = strtok(Y_string, " ");
	token = strtok(NULL, " ");
	return atoi(token);
}
char * cmd_get(){
	char *file_name, *get_cmd = get_input();
	file_name = strtok(get_cmd, " ");
	file_name = strtok(NULL, " ");
	return file_name;
}
int min( int a, int b){
	if(a>b)
		return b;
	else
		return a;
}
int compare( char * a, char * b){
	int i=0;
	while(a[i]==b[i]){
		if(a[i++]=='\0')
			return 1;
	}
	return 0;
}
char * duplicate(char * A, int n){
	char * T = (char*)malloc((n+1)*sizeof(char));
	int i=0;
	while(n){
		T[i]=A[i];
		i++;
		n--;
	}
	T[i]='\0';
	// printf("hell:%s++\n",T);
	return T;
}
int main()
{
	int C1, C2, opt, S2, a2, size_of_packet, flag=0;
	if( (C1 = socket(AF_INET, SOCK_STREAM, 0) ) <0 )
	{	perror("Cant create socket.\n"); exit(0); }

	opt = 1;
	if( setsockopt(C1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt Error......");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in SA , SA2, CA2;
	SA.sin_family 	   = AF_INET;
	SA.sin_port        = htons(20001);
	inet_aton("127.0.0.1", &SA.sin_addr);

	if( connect(C1, (struct sockaddr*)&SA, sizeof(SA)) < 0 ){
		perror("Unable to connect");
		exit(0);
	}

	int Y,n,r,b1=0,b2=0,b3=0,b4=0,d1=0,d2=0,d3=0,d4=0,c1=0,c2=0,c3=0,c4=0,status=0,fw,x,code,GET_CODE,counter,total_size,d_SIZE,last_packet_received=0;
	char ch, data[DATA_SIZE];
	char * file_name , *server_input;
	char *token, *input, *temp;

	memset(data,'\0',sizeof(data));
	int port_saved=0;		
	int port_send = 0;

	while(1){
		input = NULL;
		input = get_input();
		if (strlen(input)==0)
			continue;
		// printf("INPUT string:[%s]\n",input);
		server_input = duplicate(input, strlen(input));
		// printf("SERVER string:[%s]\n",server_input);
		token = strtok(input, " ");
		if(!port_saved)
		{	
			temp = strtok(NULL," ");
			if(temp== NULL){
				port_saved = 0;
			}else
			{	
				Y = atoi(temp);
				port_saved = 1;
			}	

		}
		// printf("Y is [%d]\n", Y);
		if(port_send==0){
			// printf("Sending port to server : [%d]\n", Y);
			send(C1, server_input, strlen(server_input)+1, 0);
			c1=0;c2=0;c3=0;c4=0;
			code = 0;
			recv(C1, &c1, 1, 0); 
			recv(C1, &c2, 1, 0);
			recv(C1, &c3, 1, 0);
			recv(C1, &c4, 1, 0); 
			code = c4*16777216 + c3*65536 + c2*256 + c1;
			// printf("Code got [%d]\n", code);
			if (code == 503)
			{	printf("\tERROR CODE 503: Port not send.\n");
				port_send = 0;
				close(C1);
				printf("\tAll Connections CLosed!\n");
				return 0;
			}else if (code == 200)
			{
				printf("\tSUCCESS CODE 200: Port Saved.\n");
				port_send = 1;
			}
			else if (code == 550)
			{	printf("\tERROR CODE 550: Invalid Port.\n");
				port_send = 0;	
				close(C1);
				printf("\tAll Connections CLosed!\n");
				return 0;
			}else if (code == 501)
			{	printf("\tERROR CODE 501: Invalid Arguments.\n");
				port_send = 0;	
			}
			else{
				printf("\tXXXXXXXXXXXcrashedXXXXXXXXXXXXXXXX[%d]\n",code);
			}
		}
		else if (compare(token, "port\0") && port_send==1){
			send(C1, server_input, strlen(server_input)+1, 0);
			c1=0;c2=0;c3=0;c4=0;
			code = 0;
			recv(C1, &c1, 1, 0); 
			recv(C1, &c2, 1, 0);
			recv(C1, &c3, 1, 0);
			recv(C1, &c4, 1, 0); 
			code = c4*16777216 + c3*65536 + c2*256 + c1;
			// printf("Code got [%d]\n", code);
			if (code == 503)
			{	printf("\tERROR CODE 503: Port not send.\n");
			}else if (code == 200)
			{
				printf("\tSUCCESS CODE 200: Port Saved.\n");
				port_send = 1;
				token = strtok(NULL, " ");
				Y = atoi(token);
			}
			else if (code == 550)
			{	printf("\tERROR CODE 550: Invalid Port.\n");
			}else if (code == 501)
			{	printf("\tERROR CODE 501: Invalid Arguments.\n");
			}
			else{
				printf("\tXXXXXXXXXXXcrashedXXXXXXXXXXXXXXXX[%d]\n",code);
			}
			// printf("\tPort Already Send.\n");
		}
		else if(compare(token, "cd\0")&& port_send==1){
			// printf("cd\n");
			send(C1, server_input, strlen(server_input)+1, 0);
			d1=0;d2=0;d3=0;d4=0;
			GET_CODE = 0;
			recv(C1, &d1, 1, 0); 
			recv(C1, &d2, 1, 0);
			recv(C1, &d3, 1, 0);
			recv(C1, &d4, 1, 0); 
			GET_CODE = d4*16777216 + d3*65536 + d2*256 + d1;
			if(GET_CODE == 200){
				printf("\tSUCCESS CODE 200: Directory changed.\n");
			}
			else if(GET_CODE == 501)
			{
				printf("\tERROR CODE 501: Can't change the Directory because either Arguments are Incorrect or Directory does not exist.\n");
			}else{
				printf("\tXXXXXXXXXXXcrashedXXXXXXXXXXXXXXXX[%d]\n",GET_CODE);
			}
		}
		else if(compare(token, "get\0")&& port_send==1){
			// printf("getting [%s][%d]\n",server_input,Y);
			status = 0;
			int child = fork();
			if(child == 0){				// Data Process
				// sleep(1);
				if( (C2 = socket(AF_INET, SOCK_STREAM, 0) ) <0 )
				{	perror("Cant create Client Data socket.\n"); exit(0); }

				// printf("Client port is:[%d]\n", Y);
				SA2.sin_family 	   = AF_INET;
				SA2.sin_port        = htons(Y);
				inet_aton("127.0.0.1", &SA2.sin_addr);
				sleep(1);
				// printf("Client Data waiting for connection... \n");
				if( connect(C2, (struct sockaddr*)&SA2, sizeof(SA2)) < 0 ){
					perror("Unable to connect");
					exit(0);
				}
				// printf("\nClient Data Process connected.\n");

				last_packet_received =0;
				ch = 'N';
				b1=0;b2=0;b3=0;b4=0;
				total_size = 0;
				counter = 0;

				printf("\tReceiving Packets....\n");
				file_name = strtok(server_input, " "); 
				file_name = strtok(NULL, " ");
				flag = 0;
				if(has_slash(file_name)){
					file_name = get_last_name(file_name);
					flag = 1;
				}
				fw = open(file_name, O_WRONLY | O_CREAT, 0644);
				while(!last_packet_received )
				{	recv(C2, &ch, 1, 0);
					// printf("Packet:\nch: %c", ch);
					if(ch == 'L') last_packet_received = 1; 
					recv(C2, &b1, 1, 0); 
					recv(C2, &b2, 1, 0);
					recv(C2, &b3, 1, 0);
					recv(C2, &b4, 1, 0); 
					total_size = b4*16777216 + b3*65536+ b2*256 + b1;
					counter = 0;
					// printf("total_size: %d\n", total_size);
					while(total_size!=0){
						x = recv(C2, data, min(total_size,DATA_SIZE), 0);
						write(fw, data, x);
						total_size -= x;
					}
				}
				if(flag)
					{free(file_name);}
				close(fw);
				close(C2);
				// printf("All Packed Received.\n");
				exit(1);
			}
			else{
				// Control Process
				// printf("sending to server: [%s]\n", server_input );
				send(C1, server_input, strlen(server_input)+1, 0);
				d1=0;d2=0;d3=0;d4=0;
				GET_CODE = 0;
				recv(C1, &d1, 1, 0); 
				recv(C1, &d2, 1, 0);
				recv(C1, &d3, 1, 0);
				recv(C1, &d4, 1, 0); 
				GET_CODE = d4*16777216 + d3*65536 + d2*256 + d1;
				if(GET_CODE == 550 || GET_CODE == 501)
				{	
					// printf("Killing child...\n");
					kill(child, SIGKILL);
					// printf("Child Killed!\n");
					if(GET_CODE == 550)
						printf("\tERROR CODE 550: Either file does not exist, or it cannot be opened for read\n");
					else
						printf("\tERROR CODE 501: Invalid Arguments!\n");
				}
				else if ( GET_CODE == 250)
				{
					waitpid(child, &status, 0);
					printf("\tSUCCESS CODE 250: File received successfully.\n");
				}else{
					printf("\tXXXXXXXXXXXcrashedXXXXXXXXXXXXXXXX");
				}
			}
		}
		else if(compare(token, "put\0") && port_send==1){
				file_name = strtok(NULL, " ");	
				FILE *f = fopen(file_name,"rb");
				// printf("file to send: [%s] at port: [%d]\n",file_name, Y);
				if(f==NULL)
				{	
					// code = 550;
					printf("\tFile does not exist.\n");
					continue;
					// send(C1, &code, sizeof(int) , 0);
				}
				else
				{	
					// printf("Forking...\n");
					fclose(f);
					int child = fork();
					if(child==0)
					{
						// printf("asdfasf\n");
						if( (S2 = socket(AF_INET, SOCK_STREAM, 0) ) <0 )
							{	perror("Cant create socket.\n"); exit(0); }

						opt = 1;
						if( setsockopt(S2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
						{
							perror("setsockopt Error......");
							exit(EXIT_FAILURE);
						}

						SA2.sin_family 	   = AF_INET;
						SA2.sin_addr.s_addr = INADDR_ANY;
						SA2.sin_port        = htons(Y);
						
						if( bind(S2, (struct sockaddr *)&SA2, sizeof(SA2)) < 0 )
						{	perror("Unable to bind local address\n"); exit(0); }

						listen( S2, 5);
						a2 = sizeof(CA2);
						// printf("Client Data waiting for connection... at port [%d]\n",Y);
						C2 = accept(S2, (struct sockaddr *)&CA2, &a2);
						if( C2 < 0){
							perror("Client Data Process Accept Error."); exit(0);
						}
						// printf("Client Data Process Connected with Server Data Process.\n");
					
						// printf("Sending file: \t[%s]\n",file_name);
						FILE *f = fopen(file_name,"rb");
						if(f==NULL)
							printf("\tFile not opened.\n");							
						// printf("File opened [%s][%d]\n", file_name, counter);
						counter = 0;
						printf("\tSending....Please Wait...\n");
						while ( fread(&ch, 1, 1 , f) )
						{	
							//printf(",\n" );
							data[counter++] = ch; 		// store ch to data[]
							if( counter >= DATA_SIZE ) 		// if data[] becomes full
							{	if( fgetc(f) != EOF) 	// if not pointing currently to last character
								{	ch='N';
									// printf("N send\n");
									fseek( f, -1, SEEK_CUR ); 	// above if checking moved f by one place, so move it back
									send(C2, &ch, 1, 0);	
								
								}
								else{					// if file size is exactly (16*n)
									ch='L';
									// printf("L send\n");
									send(C2, &ch, 1, 0);
								}
								size_of_packet = DATA_SIZE;
								send(C2, &size_of_packet, sizeof(int), 0);
								send(C2, data, DATA_SIZE, 0);
								counter = 0;
							}
							// printf("+%d\n",counter);
						}
						// printf("Last packed sending..\n");
						if(counter!=0)
						{
							ch = 'L';
							// printf("L send.\n");
							send(C2, &ch, 1, 0);
							send(C2, &counter, sizeof(int), 0);
							send(C2, data, counter, 0);
						}
						// printf("All packets send.\n");
						fclose(f); 
						// fclose(fw);
						close(C2);
						close(S2);
						exit(1);
					}
					else{
						// printf("sending to server: [%s]\n", server_input );
						send(C1, server_input, strlen(server_input)+1, 0);
						d1=0;d2=0;d3=0;d4=0;
						GET_CODE = 0;
						recv(C1, &d1, 1, 0); 
						recv(C1, &d2, 1, 0);
						recv(C1, &d3, 1, 0);
						recv(C1, &d4, 1, 0); 
						GET_CODE = d4*16777216 + d3*65536 + d2*256 + d1;
						if(GET_CODE == 550 || GET_CODE == 501)
						{	
							// printf("Killing child...\n");
							kill(child, SIGKILL);
							// printf("Child Killed!\n");
							if(GET_CODE == 550){
								printf("\tERROR CODE 550: Unsuccessful transfer!\n");
							}else{
								printf("\tERROR CODE 501: Invalid Arguments! \n");
							}
						}
						else if ( GET_CODE == 250)
						{
							waitpid(child, &status, 0);
							printf("\tSUCCESS CODE 250: File transfer successful.\n");
						}
						else{
							printf("\tXXXXXXXXXXXcrashedXXXXXXXXXXXXXXXX[%d]\n",GET_CODE);
						}
					}
				}
				
			}
		else if(compare(token, "quit\0")&& port_send==1){
			// printf("quit\n");
			// printf("sending to server: [%s]\n", server_input );
			send(C1, server_input, strlen(server_input)+1, 0);
			d1=0;d2=0;d3=0;d4=0;
			GET_CODE = 0;
			recv(C1, &d1, 1, 0); 
			recv(C1, &d2, 1, 0);
			recv(C1, &d3, 1, 0);
			recv(C1, &d4, 1, 0); 
			GET_CODE = d4*16777216 + d3*65536 + d2*256 + d1;
			if(GET_CODE == 421){
				close(C1);
				printf("\tSUCCESS CODE 421: All connections cLosed.\n");
				return 0;
			}
			else if(GET_CODE == 501){
				printf("\tERROR CODE: 501 Invalid Arguments.\n");
			}
			else{
				printf("\tXXXXXXXXXXXcrashedXXXXXXXXXXXXXXXX[%d]\n",GET_CODE);
			}
		}else{
			send(C1, server_input, strlen(server_input)+1, 0);
			d1=0;d2=0;d3=0;d4=0;
			GET_CODE = 0;
			recv(C1, &d1, 1, 0); 
			recv(C1, &d2, 1, 0);
			recv(C1, &d3, 1, 0);
			recv(C1, &d4, 1, 0); 
			GET_CODE = d4*16777216 + d3*65536 + d2*256 + d1;
			if(GET_CODE == 502){
				printf("\tERROR CODE 421: Command not found.\n");
			}else{
				printf("\tXXXXXXXXXXXcrashedXXXXXXXXXXXXXXXX[%d]\n",GET_CODE);	
			}
		}
	}
	return 0;
}

