#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h> /* _beginthread() */

#pragma comment(lib, "ws2_32.lib")

#define PORT 32001
#define RCVBUFSIZE 256   /* Size of receive buffer */

void DieWithError(char *errorMessage)
{
    printf("%s", errorMessage);
}  /* Error handling function */

void main()
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort;     /* Echo server port */
    char *servIP;                    /* Server IP address (dotted quad) */
    char *echoString;                /* String to send to echo server */
    char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
    int echoStringLen;               /* Length of string to echo */
    int bytesRcvd, totalBytesRcvd;   /* Bytes read in single recv() and total bytes read */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */

    servIP = "10.132.12.220";             /* First arg: server IP address (dotted quad) */
    echoString = "dupeczka";         /* Second arg: string to echo */

        echoServPort = PORT; /* Use given port, if any */

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        fprintf(stderr, "WSAStartup() failed");
        exit(1);
    }

    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(echoServPort); /* Server port */
    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed\n");

    /* Receive message from the server*/
    memset(echoBuffer, 0, RCVBUFSIZE);
    totalBytesRcvd = 0;
    printf("Received: ");
    int i;
    bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0);
    printf("%d\n", bytesRcvd);
    /* Setup to print the echoed string */
    for(i = 0; i< 256; i++)
        printf("%c", echoBuffer[i]);
    echoBuffer[bytesRcvd] = '\0';  /* Add \0 so printf knows where to stop */
//    printf("%s", echoBuffer);            /* Print the echo buffer */
    printf("\n");    /* Print a final linefeed */



    /* Send the string, including the null terminator, to the server */
    echoStringLen = strlen(echoString);          /* Determine input length */
    if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
        DieWithError("send() sent a different number of bytes than expected");

    closesocket(sock);
    WSACleanup();  /* Cleanup Winsock */

    exit(0);
}
