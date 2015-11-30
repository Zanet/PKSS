#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include "pkss_structs.h"

#define FAIL    (-1)
#define SUCCESS (0)

const int BUFFER_SIZE = 4096;
const int TIME_STR_SIZE = 12;
const int NUMBER_OF_DEVICES = 4;
int iteration = 0;
bool deviceConnected[NUMBER_OF_DEVICES];
char addr_buf[INET_ADDRSTRLEN];

void resetConnectionsTable()
{
    for(int i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        deviceConnected[i] = false;
    }
}

bool allClientsConnected()
{
    bool allConnected = true;
    for(int i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        if(deviceConnected[i] == false)
            return false;
    }
    return true;
}

int main(int argc, char* argv[]) 
{
    int sockfd, sockfd2;
    socklen_t addrlen;
    struct sockaddr_in svr_name, cli_name;
    int status;
    char inBuf[BUFFER_SIZE];
    /* Create the socket */
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == (-1)) {
        printf("socket() error\n");
        exit(1);
    }
    if (argc < 2) {
        printf("Usage: %s port_no\n", argv[0]);
        exit(2);
    }
    /* Server parameters */
    svr_name.sin_family = PF_INET;
    svr_name.sin_addr.s_addr = INADDR_ANY; //tu ip serwera?
    svr_name.sin_port = htons(atoi(argv[1]));
    status = bind(sockfd, (struct sockaddr*) &svr_name, sizeof(svr_name));
    if (status == -1) {
        printf("bind() error\n");
        exit(3);
    }

    status = listen(sockfd, 5);
    if (status == -1) {
        printf("listen() error\n");
        exit(4);
    }


    resetConnectionsTable();
    while(true) 
    {
        

        
        printf("Waiting for connection.\n");
        addrlen = sizeof(cli_name);
        sockfd2 = accept(sockfd, (struct sockaddr*) &cli_name, &addrlen);
        if (sockfd2 == -1) {
            printf("accept() error\n");
            exit(1);
        }
        status = recv(sockfd2, inBuf, sizeof(inBuf), 0);
        if (status == -1) {
            printf("recv() error\n");
            exit(5);
        }
        
        if ( inet_ntop( AF_INET, &cli_name.sin_addr, addr_buf, INET_ADDRSTRLEN ) == NULL )
        {
            printf("Ip address problem\n"); 
            exit(1); 
        }
       
        printf("Received: %s\n", inBuf);

        if(strcmp(inBuf, "!closeServer") == 0)
        {
            printf("Closed manually");
            close(sockfd2);
            break;             
        }

        char separator = '!';
        char *id = strtok(inBuf, &separator);
        char *command = strtok(NULL, &separator);
        printf("ID: %s\n", id);
        printf("Command: %s\n", command);
        printf( "...from [%s:%d]\n",addr_buf, ntohs( cli_name.sin_port ));       
 
        unsigned int clientNumber = 255; //0 - E, 1 - W, 2 - B, 3 - R,4 - 


        if (strcmp(id, "E") == 0)
        {
            clientNumber = 0;
            printf("E - Elektrociepłownia\n");
        }
        else if(strcmp(id, "W") == 0)
        {
            clientNumber = 1;
            printf("W - Wymiennik ciepła\n");
        } 
        else if(strcmp(id, "B") == 0)
        {
            clientNumber = 2;
            printf("B - Budynek\n");
        } 
        else if(strcmp(id, "R") == 0)
        {
            clientNumber = 3;
            printf("R - Regulator\n");
        }
        else 
        {
            printf("Nie rozpoznano hosta\n");
        }      
        
        if(clientNumber < NUMBER_OF_DEVICES)
        {
            if(strcmp(command, "KeepAlive") == 0)
            {
                deviceConnected[clientNumber] = true;      
            }


        }

        if(allClientsConnected())
        {   
            printf("Everyone is connected!\n We can start");
        }
        else
        {   
            printf("We are missing someone :(\n");
        }

        memset(inBuf, 0, BUFFER_SIZE);
        iteration++;

        
    }

    close(sockfd);
    return SUCCESS;
}




/*
time_t timer;
            struct tm* info;
            
            time(&timer); //current system time            
            info = localtime(&timer);

            char buffer[12] = "";
            strftime(buffer, 12, "%I:%M:00 %p", info);

            printf("Time set to: %s\n", buffer);

            send(sockfd2, buffer, sizeof("HH:MM:00 PM"), 0);



*/

