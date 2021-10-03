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
#include <dirent.h>
#include <errno.h>
typedef int ERROR_CODE;
#define BUFF_SIZE 100
#define FILE_NOT_FOUND 550
#define DATA_SIZE 20
#define CMD_SIZE 500
int compare( char * a, char * b){
	int i=0;
	while(a[i]==b[i]){
		if(a[i++]=='\0')
			return 1;
	}
	return 0;
} 
int min( int a, int b){
	if(a>b)
		return b;
	else
		return a;
}
char* add_(char * A, char * B){
	char* t =(char*)malloc(500*sizeof(char));
	int i=0,j=0;
	while(A[i]!='\0')
		{t[i]=A[i];i++;}
	while(B[j]!='\0')
		{t[i]=B[j];i++;j++;}
	t[i]='\0';
	return t;
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
char * parse_input(char * Arr, int n){
	char * A = malloc((n+1)*sizeof(char));
	memset(A, '\0', sizeof(A));
	int state=0;
	int k=0,i=0;
	for ( k = 0; k < n; ++k)
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
int has_char(char * A){
	int i=0;
	while(A[i]!='\0'){
		if(A[i] <'0' || A[i]> '9')
			return 1;
		i++;
	}
	return 0;
}
int	main()
{

	int S1,C1, S2, C2;
	int b1=0,b2=0,b3=0,b4=0, total_size=0, x, last_packet_received=0;
	int a,a2,code, opt, status=0, fw;
	if( (S1 = socket(AF_INET, SOCK_STREAM, 0) ) <0 )
	{	perror("Cant create socket.\n"); exit(0); }

	struct sockaddr_in SA,CA;
	struct sockaddr_in SA2,CA2;
	SA.sin_family 	   = AF_INET;
	SA.sin_addr.s_addr = INADDR_ANY;
	SA.sin_port        = htons(20001);

	opt = 1;
	if( setsockopt(S1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt Error......");
		exit(EXIT_FAILURE);
	}
	

	if( bind(S1, (struct sockaddr *)&SA, sizeof(SA)) < 0 )
	{	perror("Unable to bind local address\n"); exit(0); }


	listen( S1, 5);
	int Y,k, child, flag;
	int counter=0,size_of_packet, port_saved=0;
	char input[500],base[500],*file_name, *token, *temp, *tmp_dir;
	char data[DATA_SIZE],ch;
	memset(data,'\0',sizeof(data));

	while(1){

		port_saved = 0;
		Y=0;
		k=0;
		code = 0;
		memset(input, '\0', sizeof(input));
		memset(base, '\0', sizeof(base));
		a = sizeof(CA);
		printf("==================SERVER=================\n");
		printf("Waiting For a new Client....\n");
		C1 = accept(S1, (struct sockaddr *)&CA, &a);
		if( C1 < 0 ) 
			{ perror("Accept error\n");	exit(0); }
		
		while(1){
			port_saved = 0;
			Y=0;
			k=0;
			memset(input, '\0', sizeof(input));
			memset(base, '\0', sizeof(base));
			code = 0;
			printf("*********************************************\nWaiting for Port..\n");
			while(recv(C1, &ch, 1, 0)){
				input[k]=ch; k++;
				// printf("%c:\n",ch );
				if(ch=='\0')
					break;
			}


			temp = parse_input(input, strlen(input));
			for (int j = 0; j < strlen(temp); ++j)
			{
				input[j] = temp[j];	input[j+1] = '\0';
			}
			printf("Client request: [%s]\n",input);	
		
			token = strtok(input, " ");
			
			if(!compare(token, "port\0")){
				code = 503;
				printf("Error 503: Port not received!!\n");
				send(C1, &code, sizeof(int) , 0);
				close(C1);
				printf("Connection Closed from Client!!\n");
				break;
			}
			token = strtok(NULL, " ");
		
			char * check = strtok(NULL, " ");
			if(check != NULL || token == NULL){
				code = 501;
				send(C1, &code, sizeof(int) , 0);
				printf("Invalid Argumnets!!\n");
				continue;
			}
			
			break;
				
		}

		if(code == 503)
			continue;
		
		Y = atoi(token);
		if(Y==0)
		{
			code = 503;
			printf("Error 503: Port not received!!\n");
			send(C1, &code, sizeof(int) , 0);
			close(C1);
			printf("Connection Closed from Client!!\n");
			continue;
		}
		else if(Y<1025 || Y>65534 || has_char(token))
		{	code = 550;
			printf("Error 550: Invalid Port!!\n");
			send(C1, &code, sizeof(int) , 0);
			close(C1);
			printf("Connection Closed from Client!!\n");
			continue;
		}
		else{
			code = 200;
			send(C1, &code, sizeof(int) , 0);
			printf("Success CODE 200: Port Saved [%d]!!\n", Y);
			port_saved = 1;
		}

		while(1){
			printf("***************************************************\n");
			printf("Present Working Directory: [%s]\nClient request: ",base );
			
			memset(input, '\0', sizeof(input));
			int k=0;

			while(recv(C1, &ch, 1, 0)){
				input[k]=ch;
				k++;
				// printf("[%c]\n", ch);
				if(ch=='\0')
					break;
			}
			printf("[%s]\n",input);
			token = strtok(input, " ");
			if(compare(token, "port\0")){
				token = strtok(NULL, " ");
				char * check= strtok(NULL, " ");;
				int temp_Y = atoi(token);
				if(check != NULL){
					code = 501;
					send(C1, &code, sizeof(int) , 0);
					printf("Invalid Argumnets!!\n");
					continue;
				}
				if(temp_Y==0)
				{
					code = 503;
					printf("Error 503: Port not received!!\n");
					printf("\tStill working on Previous send port.\n");
					send(C1, &code, sizeof(int) , 0);
					// close(C1);
					// printf("Connection Closed from Client!!\n");
					continue;
				}
				else if(temp_Y<1025 || temp_Y>65534 || has_char(token))
				{	code = 550;
					printf("Error 550: Invalid Port!!\n");
					send(C1, &code, sizeof(int) , 0);
					printf("\tStill working on Previous send port.\n");
					
					// close(C1);
					// printf("Connection Closed from Client!!\n");
					continue;
				}
				else{
					code = 200;
					Y=temp_Y;
					send(C1, &code, sizeof(int) , 0);
					printf("Success CODE 200: Port Saved [%d]!!\n", Y);
					port_saved = 1;
					continue;
				}
			}
			else if(compare(token, "get\0")){
				file_name = strtok(NULL, " ");	
				char * check = strtok(NULL, " ");
				if(check != NULL || file_name == NULL){
					code = 501;
					send(C1, &code, sizeof(int) , 0);
					printf("Invalid Argumnets!!\n");
					continue;
				}
				char * t = (char*)malloc(500*sizeof(char));
				if(!has_slash(file_name))
				{
					int i=0,j=0;
					while(base[i]!='\0')
						{t[i]=base[i];i++;}
					while(file_name[j]!='\0')
						{t[i]=file_name[j];i++;j++;}
					file_name = t;
				}
				printf("File searching: [%s]\n",file_name);
				FILE *f = fopen(file_name,"rb");
				if(f==NULL)
				{	
					code = 550;
					printf("File does not exist!!\n");
					send(C1, &code, sizeof(int) , 0);
				}
				else
				{	
					// printf("Forking...\n");
					child = fork();
					if(child==0)
					{
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
						// printf("Server Data waiting for connection... at port [%d]\n",Y);
						C2 = accept(S2, (struct sockaddr *)&CA2, &a2);
						if( C2 < 0){
							perror("Server Data Process Accept Error!!!!"); exit(0);
						}
						// printf("Server Data Connected!!\n");
					
						printf("Sending file: [%s]\n",file_name);
						// FILE *fw = fopen(file_name,"rb");
						counter =0;
						flag = 0;
						while ( fread(&ch, 1, 1 , f) )
						{	
							flag=1;
							data[counter++] = ch; 		// store ch to data[]
							if( counter >= DATA_SIZE ) 		// if data[] becomes full
							{	if( fgetc(f) != EOF) 	// if not pointing currently to last character
								{	ch='N';
									fseek( f, -1, SEEK_CUR ); 	// above if checking moved f by one place, so move it back
									send(C2, &ch, 1, 0);	
								}
								else{					// if file size is exactly (16*n)
									ch='L';
									send(C2, &ch, 1, 0);
								}
								size_of_packet = DATA_SIZE;
								send(C2, &size_of_packet, sizeof(int), 0);
								send(C2, data, DATA_SIZE, 0);
								// fwrite( data, DATA_SIZE, 1, fw );
								counter = 0;
							}
						}
						if(counter!=0)
						{
							ch = 'L';
							send(C2, &ch, 1, 0);
							send(C2, &counter, sizeof(int), 0);
							send(C2, data, counter, 0);
						}
						if(flag==0){
							ch = 'L';
							size_of_packet = 0;
							send(C2, &ch, 1, 0);
							send(C2, &size_of_packet, sizeof(int), 0);
						}
						fclose(f); 
						close(C2);
						close(S2);
						exit(1);
					}
					else{
						// printf("Waiting for child to exit\n");
						wait(NULL);
						// printf("Child ended..\n");						
						code = 250;
						send(C1, &code, sizeof(int) , 0);
						printf("Transferring Done!!.\n");
					}
				}
				
			}
			else if(compare(token, "put\0")){
				file_name = strtok(NULL, " ");	
				char * check = strtok(NULL, " ");
				if(check != NULL || file_name==NULL){
					code = 501;
					send(C1, &code, sizeof(int) , 0);
					printf("Invalid Argumnets!!\n");
					continue;
				}
				printf("File name received: [%s]\n",file_name);
				status = 0;
				int child = fork();
				if(child == 0){				// Data Process
					// sleep(1);
					if( (C2 = socket(AF_INET, SOCK_STREAM, 0) ) <0 )
					{	perror("Cant create Server Data socket.\n"); exit(0); }

					SA2.sin_family 	   = AF_INET;
					// printf("Server port is:[%d]\n", Y);
					SA2.sin_port        = htons(Y);
					inet_aton("127.0.0.1", &SA2.sin_addr);
					sleep(1);
					// printf("\nServer Data waiting for connection... \n");
					if( connect(C2, (struct sockaddr*)&SA2, sizeof(SA2)) < 0 ){
						perror("Unable to connect");
						exit(0);
					}
					// printf("\nServer Data Process connected!!\n");

					last_packet_received =0;
					ch = 'N';
					b1=0;b2=0;b3=0;b4=0;
					total_size = 0;
					counter = 0;

					// file_name = strtok(server_input, " "); 
					// file_name = strtok(NULL, " ");
					char * t = (char*)malloc(500*sizeof(char));
					int i=0,j=0;
					while(base[i]!='\0')
						{t[i]=base[i];i++;}
					while(file_name[j]!='\0')
						{t[i]=file_name[j];i++;j++;}
					file_name = t;
					fw = open(file_name, O_WRONLY | O_CREAT, 0644);
					printf("Receiving Packets....\n");
					int co=0;
					while(!last_packet_received )
					{	recv(C2, &ch, 1, 0);
						// printf("Packet:\nch: %c", ch);
						if(ch == 'L') last_packet_received = 1; 
						recv(C2, &b1, 1, 0); 
						recv(C2, &b2, 1, 0);
						recv(C2, &b3, 1, 0);
						recv(C2, &b4, 1, 0); 
						total_size = b4*16777216 + b3*65536 + b2*256 + b1;
						counter = 0;
						// printf("total_size: %d\n", total_size);
						while(total_size!=0){
							x = recv(C2, data, min(total_size,DATA_SIZE), 0);
							write(fw, data, x);
							total_size -= x;
						}
						co++;
					}
					free(t);
					close(fw);
					close(C2);
					printf("All Packed Received!!\n");
					exit(0);
				}
				else{
					// Control Process
					// printf("Waiting for child to exit\n");
					waitpid(child, &status, 0);
					// printf("Server Child ended..\n");						
					
					// printf("status code: [%d]\n", status);
					if(status == 0){
						code = 250;
						send(C1, &code, sizeof(int) , 0);
						printf("Success code 250 send to Client!!\n");
					}else {
						code = 550;
						send(C1, &code, sizeof(int) , 0);		
						printf("Unsuccessful code send to Client!!\n");
					}
					
				}
			}
			else if(compare(token, "quit\0"))
			{	
				char * check= strtok(NULL, " ");;

				if(check != NULL){
					code = 501;
					send(C1, &code, sizeof(int) , 0);
					printf("Invalid Argumnets!!\n");
					continue;
				}
				code = 421;
				send(C1, &code, sizeof(int) , 0);
				close(C1);
				printf("Connection successfully closed!!\n");
				break;
			}
			else if(compare(token, "cd\0")){
				file_name = strtok(NULL, " ");
				char * check = strtok(NULL, " ");
				if(check != NULL || file_name == NULL){
					code = 501;
					send(C1, &code, sizeof(int) , 0);
					printf("Invalid Argumnets!!\n");
					continue;
				}
				// printf("Initial Dir: \t[%s]\n", base);
				tmp_dir = add_(base, file_name);
				// printf("Dir searching: \t[%s]\n", tmp_dir);
				DIR* dir = opendir(tmp_dir);
				if(dir){
					int i=0,j=0;
					while(base[i]!='\0') i++;
					while(file_name[j]!='\0') {base[i]=file_name[j];i++;j++;}
					base[i]='/';
					i++;
					base[i]='\0';
					code = 200;
					send(C1, &code, sizeof(int) , 0);
					printf("Directory Changed.\n");

					// printf("New Base: \t[%s]\n", base);
				}else{
					code = 501;
					send(C1, &code, sizeof(int) , 0);
					printf("Cant change Directory.\n");
				}
			}else{
					code = 502;
					send(C1, &code, sizeof(int) , 0);
					printf("Invalid Command.\n");
			}
		}
	}
	close(S1);
	return 0;	
}


