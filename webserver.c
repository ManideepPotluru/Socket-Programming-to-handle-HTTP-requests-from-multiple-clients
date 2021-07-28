#include <netinet/in.h>    
#include <stdio.h>    
#include <stdlib.h>    
#include <sys/socket.h>    
#include <string.h>
#include <sys/stat.h>    
#include <sys/types.h>    
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define IP_PROTOCOL 0 
#define NET_BUF_SIZE 32 
#define cipherKey 'S' 
#define sendrecvflag 0 
#define nofile "File Not Found!" 

bool writeDataToClient(int sckt, const void *data, int datalen)
{
    const char *pdata = (const char*) data;

    while (datalen > 0){
        int numSent = send(sckt, pdata, datalen, 0);
        if (numSent <= 0){
            if (numSent == 0){
                printf("The client was not written to: disconnected\n");
            } else {
                perror("The client was not written to");
            }
            return false;
        }
        pdata += numSent;
        datalen -= numSent;
    }

    return true;
}

bool writeStrToClient(int sckt, const char *str)
{
    return writeDataToClient(sckt, str, strlen(str));
}

void tcp(int portNum){
    int create_socket, new_socket;    
    char *buffer;
    int bufsize = 1024;    
    struct sockaddr_in address;    
    socklen_t addrlen;    

    buffer = (char*) malloc(bufsize);    
    if (!buffer){
        printf("The receive buffer was not allocated\n");
        exit(1);    
    }

    create_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (create_socket == -1){    
        perror("The socket was not created");    
        exit(1);    
    }

    printf("The socket was created\n");

    memset(&address, 0, sizeof(address));    
    address.sin_family = AF_INET;    
    address.sin_addr.s_addr = INADDR_ANY;    
    address.sin_port = htons(portNum);    

    if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == -1){    
        perror("The socket was not bound");    
        exit(1);    
    }

    printf("The socket is bound\n");  

    long fsize;
   

     if (listen(create_socket, 10) == -1){
        perror("The socket was not opened for listening");    
        exit(1);    
    }    

    printf("The socket is listening\n");

    while (1) {    

        addrlen = sizeof(address);
        new_socket = accept(create_socket, (struct sockaddr *) &address, &addrlen);

        if (new_socket == -1) {    
            perror("A client was not accepted");    
            exit(1);    
        }    

        printf("A client is connected from %s:%hu...\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));    

        // HTTP request parses here
        int numRead = recv(new_socket, buffer, bufsize, 0);
	
        printf("%d",numRead);
        printf("%s : my buffer:  ", buffer);
        if (numRead < 1){
            if (numRead == 0){
                printf("The client was not read from: disconnected\n");
            } else {
                perror("The client was not read from");
            }
            close(new_socket);
            continue;
        }
        //printf("%.*s\n", numRead, buffer);    


        memmove(&buffer[0], &buffer[1], strlen(buffer) - 0);

        printf("Trying to open filename : %s", buffer);
        
        FILE *fp = fopen(buffer, "rb");
        if (!fp){
            perror("The file was not opened");    
            exit(1);    
        }

        printf("The file was opened\n");

        if (fseek(fp, 0, SEEK_END) == -1){
            perror("The file was not seeked");
            exit(1);
        }

        fsize = ftell(fp);
        if (fsize == -1) {
            perror("The file size was not retrieved");
            exit(1);
        }
        rewind(fp);

        char *msg = (char*) malloc(fsize);
        if (!msg){
            exit(1);
        }

        if (fread(msg, fsize, 1, fp) != 1){
            perror("The file was not read\n");
            exit(1);
        }
        fclose(fp);

        printf("The file size is %ld\n", fsize);

	

        if (!writeStrToClient(new_socket, "HTTP/1.1 200 OK\r\n")){
            close(new_socket);
            continue;
        }

        char clen[40];
        sprintf(clen, "Content-length: %ld\r\n", fsize);
        if (!writeStrToClient(new_socket, clen)){
            close(new_socket);
            continue;
        }

        if (!writeStrToClient(new_socket, "Content-Type: text/html\r\n")){
            close(new_socket);
            continue;
        }

        if (!writeStrToClient(new_socket, "Connection: close\r\n\r\n") == -1){
            close(new_socket);
            continue;
        }

        //if (!writeStrToClient(new_socket, "<html><body><H1>Hello world</H1></body></html>")){
        if (!writeDataToClient(new_socket, msg, fsize)){
            close(new_socket);
            continue;
        }

        printf("The file was sent successfully\n");
        close(new_socket);    
   }    

   close(create_socket);        
}

// clear buffer 
void clearBuf(char* b) 
{ 
	int i; 
	for (i = 0; i < NET_BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

// encryption 
char Cipher(char ch) 
{ 
	return ch ^ cipherKey; 
} 

// sending a file 
int sendFile(FILE* fp, char* buf, int s) 
{ 
	int i, len; 
	if (fp == NULL) { 
		strcpy(buf, nofile); 
		len = strlen(nofile); 
		buf[len] = EOF; 
		for (i = 0; i <= len; i++) 
			buf[i] = Cipher(buf[i]); 
		return 1; 
	} 

	char ch, ch2; 
	for (i = 0; i < s; i++) { 
		ch = fgetc(fp); 
		ch2 = Cipher(ch); 
		buf[i] = ch2; 
		if (ch == EOF) 
			return 1; 
	} 
	return 0; 
} 


void udp(int portNumber)
{ 
	int sockfd, nBytes; 
	struct sockaddr_in addr_con; 
	int addrlen = sizeof(addr_con); 
	addr_con.sin_family = AF_INET; 
	addr_con.sin_port = htons(portNumber); 
	addr_con.sin_addr.s_addr = INADDR_ANY; 
	char net_buf[NET_BUF_SIZE]; 
	FILE* fp; 

	// socket() 
	sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL); 

	if (sockfd < 0) 
		printf("\nfile descriptor not received!!\n"); 
	else
		printf("\nfile descriptor %d received\n", sockfd); 

	// bind() 
	if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0) 
		printf("\nSuccessfully binded!\n"); 
	else
		printf("\nBinding Failed!\n"); 

 		while(1)
		{
		printf("\nWaiting for file name...\n"); 

		// receive file name 
		clearBuf(net_buf); 

		nBytes = recvfrom(sockfd, net_buf, 
						NET_BUF_SIZE, sendrecvflag, 
						(struct sockaddr*)&addr_con, &addrlen); 

		memmove( &net_buf[0] , &net_buf[1], strlen( net_buf ) - 0) ;
		fp = fopen(net_buf, "r"); 
 		
		printf("\nFile Name Received: %s\n", net_buf); 
		if (fp == NULL) 
			printf("\n File open faileddd!\n"); 
		else
			printf("\nFile Successfully opened!\n"); 

		while (1) { 

			// process 
			if (sendFile(fp, net_buf, NET_BUF_SIZE)) { 
				sendto(sockfd, net_buf, NET_BUF_SIZE, 
					sendrecvflag, 
					(struct sockaddr*)&addr_con, addrlen); 
				break; 
			} 

			// send 
			sendto(sockfd, net_buf, NET_BUF_SIZE, 
				sendrecvflag, 
				(struct sockaddr*)&addr_con, addrlen); 
			clearBuf(net_buf); 
		} 
		if (fp != NULL) 
			fclose(fp); 
	}
} 

int main(int argc, char *argv[]) 
{
  //read arguments
  int portNum;
  char isTCP;

  if (argc < 3) {
    printf("The number of arguments is less than 2.\n");
    exit(0);
  }
  sscanf(argv[1], "%d", &portNum);
  printf("Port Number: %d\n", portNum);

  if (strcmp("TCP", argv[2]) == 0) {
    isTCP = 1;
    printf("TCP\n");
    tcp(portNum);
  }
  else if (strcmp("UDP", argv[2]) == 0) {
    isTCP = 0;
    printf("UDP\n");
    udp(portNum);
  }
  else {
    perror("The 2nd argument should be either TCP or UDP.");
    exit(0);
  }
}
