//
// Czesc serwera odpowiedzialna za rozmowe przez WebSocket z interfejsem
// zgodne z RFC6455 http://tools.ietf.org/html/rfc6455 
//

#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

//#include <cstring> // strstr
#include <string.h>
#include "sha1.h"
#include "b64encode.h"
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
   if(argc > 1)
           portno = atoi(argv[1]);
   else
           portno = 5003;

   printf("portno = %d\n", portno); 

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
	   {    ("Here is the message: %s\n",buffer);
		   
		   // Szukaj w buforze klucza
		   const char keyString[] = "Sec-WebSocket-Key";
		   char* index;
		   char SocketKey[24];
		   index = strstr(buffer, keyString);
		   strncpy(SocketKey, index+19, 24);
		   // przygotuj Handshake
		   unsigned char* SocketHash; //[SHA_DIGEST_LENGTH];
           char* Base64Hash;
           char MagicString[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

           // TEST ze znanym kluczem
           // char TestKey[24] = "dGhlIHNhbXBsZSBub25jZQ==";
           // strncpy(SocketKey, TestKey, 24);
           // oczekiwany wynik po base64: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=

		   // zrobic SHA klucza i Magic Stringa 
		   sha1nfo s;
           sha1_init(&s);
		   sha1_write(&s, SocketKey, 24);
           sha1_write(&s, MagicString, 36); // albo 32 bez pauz
		   SocketHash = sha1_result(&s); 

		   printf("\nKlucz Websocket: %.*s\n", 24, SocketKey);
		   printf("SHA1 Klucz+Magic: %s\n", SocketHash);
		   printf("SHA1 jako hex string: "); printHash(SocketHash);

           // Base64 encode
           SocketHash[20] = 0; // encode() musi wiedziec gdzie jest koniec
           Base64Hash = encode(SocketHash);
           printf("Base64 encoding: %s\n", Base64Hash);

		   strcpy(response_buf, "HTTP/1.1 101 Switching Protocols\r\n");
           strcat(response_buf, "Upgrade: websocket\r\n");
           strcat(response_buf, "Connection: Upgrade\r\n");
           strcat(response_buf, "Sec-Websocket-Accept: ");
           strcat(response_buf, Base64Hash);
           printf("%s", response_buf);
	   	   n = write( newsockfd, response_buf, 255);
	   }
   }
   close(sockfd);
   return 0;
}
