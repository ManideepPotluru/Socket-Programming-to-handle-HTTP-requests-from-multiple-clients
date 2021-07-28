#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define IP_PROTOCOL 0 
#define NET_BUF_SIZE 32 
#define cipherKey 'S' 
#define sendrecvflag 0 
#define BUFFER_SIZE 102400


#include "utils.h"

// ./client <url> <port>

void udp();

int main(int argc, char **argv) {
    printf("Client starting...\n");
    int temp;
    int port; // port
    char hostname[MAX_LEN] = "localhost"; // host name of server
    char filename[MAX_LEN] = "/"; 
    char portname[MAX_LEN];
    
    char url[2*MAX_LEN + 8]; 
    if(argc > 1) {
        strncpy(url, argv[1], MAX_LEN); 

        if(strncmp(url, "http://", 7) != 0) {
            fprintf(stderr, "URL must start with 'http://'\n");
            return 1;
        }

        char *h = &url[7];
	char *b = strchr(h,':');
	if (b != NULL)
	  strncpy(hostname, h, strlen(h) - strlen(b));
        char *c = strchr(b, '/');
        if(c != NULL) {
            strncpy(filename, c, strlen(c));
            /* printf("My filename is : %s\n", filename); */
            *c = '\0';
        }
	strncpy(portname, b + 1, strlen(b) - strlen(c) + 1);
        sscanf(portname, "%d", &port);	
    }

char *fileptr = filename;
    // inside this "if" statement, we parse the <port> parameter
    
 if (strcmp("UDP", argv[2]) == 0) {
   printf("TCP\n");
   udp(fileptr, sizeof(filename), port, hostname);
   exit(0);
  }
 else if (strcmp("TCP", argv[2]) == 0) {}
 else {
   perror("The 2nd argument should be either TCP or UDP.");
    exit(0);
 }

    int sockfd = 0;
    printf("upto here");
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
   
	
	struct sockaddr_in serv_addr;  	
	struct hostent *server;
	server = gethostbyname(hostname);
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);

	char *errno;
	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed %s \n", errno);
       return 1;
    }

    printf("Connecting to '%s', port %d\n", hostname, port);

	int n = 0;	
	char recvBuff[BUFFER_SIZE];

	/* 
	while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    } 

    printf("Requesting file '%s'\n", filename);

	*/

    // "GET %s HTTP/1.1\r\n\r\n"
    
	send(sockfd, filename, 1024, 0);
	

    printf("request sent");

	
    while(true){
		memset(recvBuff ,0 , BUFFER_SIZE);
		if((n = recv(sockfd, recvBuff, BUFFER_SIZE, 0)) == -1)
		{
    		perror("recv()");
			break;
    		exit(1);
			
		}else if ( n== 0){  // n is 0 if the connection is closed. 
            printf("Received 0 length -- connection closed");
            break;
        }
		else{
 	   		printf("Client-The recv() is OK... %d the value of n \n", n);
			printf("Client-Received: %s", recvBuff);
            //break;
        }
	}
	recvBuff[n] = '\0';
	
	printf("Client-Received: %s", recvBuff);
	
	printf("Client-Closing sockfd\n");
	close(sockfd);

   
   
}

//  clear buffer 
void clearBuf(char* b) 
{ 
	int i; 
	for (i = 0; i < NET_BUF_SIZE; i++) 
		b[i] = '\0'; 
} 

//  decryption 
char Cipher(char ch) 
{ 
	return ch ^ cipherKey; 
} 

// function to receive file 
int recvFile(char* buf, int s) 
{ 
	int i; 
	char ch; 
	for (i = 0; i < s; i++) { 
		ch = buf[i]; 
		ch = Cipher(ch); 
		if (ch == EOF) 
			exit(0); 
		else
			printf("%c", ch); 
	} 
 
} 


void udp(char *fileptr, int filenamesize,int port, const char *hostname) 
{ 
	
	int sockfd, nBytes; 
	struct sockaddr_in addr_con; 
	int addrlen = sizeof(addr_con); 
	struct hostent *server;
	server = gethostbyname(hostname);
	addr_con.sin_family = AF_INET; 
	addr_con.sin_port = htons(port); 
	/* addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);  */
	bcopy((char *)server->h_addr, (char *)&addr_con.sin_addr.s_addr, server->h_length);
	char net_buf[MAX_LEN]; 
	FILE* fp; 
	
	
	memcpy(net_buf, fileptr, filenamesize); // memcpy("destination","source","size")
	printf("%s", net_buf);

	// socket() 
	sockfd = socket(AF_INET, SOCK_DGRAM, 
					IP_PROTOCOL); 

	if (sockfd < 0) 
		printf("\nfile descriptor not received!!\n"); 
	else
		printf("\nfile descriptor %d received\n", sockfd); 

		//net_buf= filename;
		sendto(sockfd, net_buf, NET_BUF_SIZE, 
			sendrecvflag, (struct sockaddr*)&addr_con, 
			addrlen); 

		printf("\n---------Data Received---------\n"); 

		while (1) { 
			// receive 
			clearBuf(net_buf); 
			nBytes = recvfrom(sockfd, net_buf, NET_BUF_SIZE, 
							sendrecvflag, (struct sockaddr*)&addr_con, 
							&addrlen); 

			// process

			printf("%d  Nbytes: ", nBytes); 
			if (recvFile(net_buf, NET_BUF_SIZE)) { 
				
			} 
		} 
		printf("\n-------------------------------\n"); 


		//close(sockfd);
	 
} 
