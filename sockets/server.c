#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

//#include <cstring> // strstr
#include <string.h>
#include "sha1.h"

// compile:
// gcc server.c -o server

// Tornado:
// https://developer.mbed.org/cookbook/Websockets-Server

#define SHA_DIGEST_LENGTH 20
/*
void printHash(uint8_t* hash) {
	int i;
	for (i=0; i<20; i++) {
		printf("%02x", hash[i]);
	}
	printf("\n");
}
*/

int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[600];
   char response_buf[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5008;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
	
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }

   while(1)
   {

	   /* If connection is established then start communicating */
	   bzero(buffer,600);
	   n = read( newsockfd,buffer,600 );
	   
	   if (n < 0) {
		  perror("ERROR reading from socket");
	   }
	   else
	   {
		   printf("Here is the message: %s\n",buffer);
		   
		   // Seek for Key
		   const char keyString[] = "Sec-WebSocket-Key";
		   char* index;
		   char SocketKey[24];
		   index = strstr(buffer, keyString);
		   strncpy(SocketKey, index+19, 24);
		   // Prepare Handshake
		   unsigned char* SocketHash; //[SHA_DIGEST_LENGTH];
           char MagicString = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		   // zrobic SHA klucza, instrukcja w kodzie index.html (link)
		   sha1nfo s;

           sha1_init(&s);
 
		   sha1_write(&s, SocketKey, 24);
           sha1_write(&s, MagicString, 36); // albo 32 bez pauz
		   SocketHash = sha1_result(&s); 

		   // SHA tests
		   printf("Test: \n");

           //printKey(SocketKey, 24);    
		   printf("\nKey is %s\nKoniec kurwa tego\n\n", SocketKey);
		   printf("SHA1 is %s\n", SocketHash);
		   printHash(SocketHash);

		   //strcpy(response_buf, "HTTP Response");
	   	   //n = write( newsockfd, response_buf, 255);
	   }
	   
   }
   
   return 0;
}
