#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

//#include <cstring> // strstr
#include <string.h>
#include <openssl/sha.h>


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
   portno = 5004;
   
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
		   char SocketKey[50];
		   printf("Test\n");
		   index = strstr(buffer, keyString);
		   strncpy(SocketKey, index+19, 35);
		   // Prepare Handshake
		   unsigned char SocketHash[SHA_DIGEST_LENGTH];
		   //SHA1(SocketKey, 35, SocketHash);

			// zrobic SHA klucza, instrukcja w kodzie index.html (link)
		   
		   sha1nfo s;

			// SHA tests
			printf("Test: \n");
	
			sha1_init(&s);
			sha1_write(&s, SocketKey, 35);
			printHash(sha1_result(&s));
		   
		   printf("Key is %s", SocketKey);
		   printf("SHA1 is %s", SocketHash);
		   strcpy(response_buf, "HTTP Response");
	   	   n = write( newsockfd, response_buf, 255);
	   }
	   
   }
   
   return 0;
}