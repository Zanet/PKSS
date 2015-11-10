#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define FAIL    (-1)
#define SUCCESS (0)

const int BUFFER_SIZE = 4096;
const int TIME_STR_SIZE = 12;

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
    svr_name.sin_addr.s_addr = INADDR_ANY;
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
        printf("Received: %s\n", inBuf);

        if (status == -1) {
            printf("send() error\n");
            exit(5);
        }
     
        if (strcmp(inBuf, "!shutdown") == 0)
        {
            printf("Shutdown forced !\n");
            close(sockfd2);
            break;        
        } 
        else if (strcmp(inBuf, "!getTime") == 0)
        {
            time_t timer;
            struct tm* info;
            
            time(&timer); //current system time            
            info = localtime(&timer);

            char buffer[12] = "";
            strftime(buffer, 12, "%I:%M:00 %p", info);

            printf("Time set to: %s\n", buffer);

            send(sockfd2, buffer, sizeof("HH:MM:00 PM"), 0);
        }      
        
        memset(inBuf, 0, BUFFER_SIZE);
    }

    close(sockfd);
    return SUCCESS;
}

