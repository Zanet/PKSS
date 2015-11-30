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


Elektrocieplownia elektrocieplownia_zeszle;
Wymiennik wymiennik_zeszle;
Budynek budynek_zeszle;
Regulator regulator_zeszle;

Elektrocieplownia elektrocieplownia_obecne;
Wymiennik wymiennik_obecne;
Budynek budynek_obecne;
Regulator regulator_obecne;



const int BUFFER_SIZE = 4096;
const int TIME_STR_SIZE = 12;
const int NUMBER_OF_DEVICES = 4;
int iteration = 0;
bool deviceConnected[NUMBER_OF_DEVICES];
bool deviceSentData[NUMBER_OF_DEVICES];
char addr_buf[INET_ADDRSTRLEN]; 


void resetConnectionsTable()
{
    for(int i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        deviceConnected[i] = false;
        deviceSentData[i] = false;
    }
}

bool allClientsSentData()
{
    bool allSent = true;
    for(int i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        if(deviceSentData[i] == false)
            return false;
    }
    return true;
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
        printf("%d. Waiting for connection.\n", iteration);
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

        if(strcmp(inBuf, "engcmd!closeServer") == 0)
        {
            printf("Closed manually");
            close(sockfd2);
            break;             
        }

        char separator = '!';
        char *id = strtok(inBuf, &separator);
        char *command = strtok(NULL, &separator);
        if(id != NULL && command != NULL)
        {
            printf("ID: %s\n", id);
            printf("Command: %s\n", command);
        }        
        //useless - every client is seen in the same way
        //printf( "...from [%s:%d]\n",addr_buf, ntohs( cli_name.sin_port ));       
 
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
        

        if(strcmp(command, "KeepAlive") == 0)
        {
            const char* nop = "nop";
            const char* start = "sta";
            if(clientNumber < NUMBER_OF_DEVICES)
            {
                deviceConnected[clientNumber] = true;      
            }
            
            if(allClientsConnected())
            {   
                printf("Everyone is connected! We can start\n");
                send(sockfd2, start, sizeof("sta"), 0);       
            }
            else
            {   
                printf("We are still missing someone :(\n");
                send(sockfd2, nop, sizeof("nop"), 0);
            }               
        }
        else if(strcmp(command, "GetTime") == 0)
        {
            time_t timer;
            struct tm* info;
            
            time(&timer); //current system time            
            info = localtime(&timer);

            char buffer[9] = "";
            strftime(buffer, 9, "%d %I:%M", info);

            printf("Date set to: %s\n", buffer);

            send(sockfd2, buffer, sizeof("DD HH:MM"), 0);
        }
        else if(strcmp(command, "GetParams") == 0)
        {
            const char* parameters = "3.14";
            switch(clientNumber) //0 - E, 1 - W, 2 - B, 3 - R, 4 - 
            {
                
                case 0:
                    send(sockfd2, parameters, sizeof("3.14"), 0); 
                break;

                case 1:
                    send(sockfd2, parameters, sizeof("3.14"), 0);
                break;

                default:
                    printf("Unknown client has sent 'Data'\n");
            }
        }

        else if(strcmp(command, "SendData") == 0)
        {
            memset(inBuf, 0, BUFFER_SIZE);

            if(clientNumber < NUMBER_OF_DEVICES)
            {
                deviceSentData[clientNumber] = true;
            }  

            switch(clientNumber) //0 - E, 1 - W, 2 - B, 3 - R, 4 - 
            {
                case 0:
                    status = recv(sockfd2, inBuf, sizeof(inBuf), 0);                     
                    printf("Received data: %s\n", inBuf);                
                break;
                
                case 1:

                break;

                default:
                    printf("Unknown client has sent 'Data'\n");
            }
        }

        memset(inBuf, 0, BUFFER_SIZE);
        if(allClientsSentData())
        {
            printf("Iteration is complete, go to next\n");
           // iteration++;
        } 
        iteration++;       
    }

    close(sockfd);
    return SUCCESS;
}
