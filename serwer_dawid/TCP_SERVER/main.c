#include<stdio.h>
#include<stdlib.h>

#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <process.h> /* _beginthread() */
#include "json.h"

#pragma comment(lib, "ws2_32.lib")

struct sockaddr_in localaddr,clientaddr;

void DieWithError(char *errorMessage);
void handle(void *pParam);

int main()
{
   SOCKET server,client;
   WSADATA wsaData;

   int iResult;
   int iSize,ret;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        DieWithError("WSAStartup failed: %d\n");
    }

    // Create socket
   server = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
   if(server == SOCKET_ERROR)
   {
	   DieWithError("Server error\n");
   }

    // Set server ip & port
   localaddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
   localaddr.sin_family = AF_INET;
   localaddr.sin_port = htons(32001);

    // Bind socket
   if(bind(server,(struct sockaddr*)&localaddr,sizeof(localaddr)) == SOCKET_ERROR)
   {
	   DieWithError("Bind error\n");
   }
   else
   {
	   DieWithError("Server is running\n");
   }
   listen(server,4);

    // Main loop for determining & handling parallel connections
   while(1) {
        size_t size = sizeof(struct sockaddr);
        struct sockaddr_in their_addr;
        SOCKET newsock;

        ZeroMemory(&their_addr, sizeof (struct sockaddr));
        newsock = accept(server, (struct sockaddr*)&their_addr, &size);

        if (newsock == INVALID_SOCKET) {
            perror("accept\n");
        }
        else {
            // Use the new socket
            uintptr_t thread;
            printf("Got a connection from %s on port %d\n",
                inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port));
            printf("New socket is %d\n", newsock);

            // Calling handle() function for new connection
            thread = _beginthread(handle, 0, (void*)&newsock);
            if (thread == -1) {
                fprintf(stderr, "Couldn't create thread: %d\n", GetLastError());
                closesocket(newsock);
            }
        }
    }


	closesocket(client);

    // Cleanup Winsock
    WSACleanup();
	return 0;

}

void DieWithError(char *errorMessage)
{
    printf("%s", errorMessage);
}  /* Error handling function */

void handle(void *pParam)
{
    // Use functions send(), recv() for tranferring the data
    int sock = *(int*)pParam;
    int ret;
    char data[2048];            /* Receive buffer */
    char *answer;
    answer = jsonCompute("Server", sock, 151, "Wymiennik ciep³a: 1400 stopni", 100);
    // Send data
    send(sock, answer, strlen(answer), 0);

    // Receive data
    ret = recv(sock,data,2048,0);
    data[ret]=0;
    printf("%s", data);


    closesocket(pParam);
}

