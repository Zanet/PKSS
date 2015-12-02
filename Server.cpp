#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctime>
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
int cmd = 0;
bool deviceConnected[NUMBER_OF_DEVICES];
bool deviceSentData[NUMBER_OF_DEVICES];
char addr_buf[INET_ADDRSTRLEN]; 

time_t timer;
struct tm* info;
char buffer[9];

/*void logToFile(int iteration, int command, const char* dataToBeLogged)
{
    static FILE *fp;
    fp = fopen("logs/a.log", "a");
    fprintf(fp, "%s", dataToBeLogged);
    fclose(fp);
}*/


void fastForwardTimeInMinutes(int minutes)
{    //printf("Current time is: %s\n", ctime(&timer));
            
    timer += 60*minutes;
    //printf("Current time after modification is: %s\n", ctime(&timer));

    info = localtime(&timer);

    //info->tm_min += minutes;

    strftime(buffer, 9, "%d %H:%M", info);
}

void printAllData()
{
    printf("--------DATA IN SYSTEM--------\n");
    printf("| Elektrocieplownia: %s, %s\n", elektrocieplownia_zeszle.T_o, elektrocieplownia_zeszle.T_zm);
    printf("| Wymiennik: %s, %s\n", wymiennik_zeszle.T_pm, wymiennik_zeszle.T_zco);
    printf("| Budynek: %s\n", budynek_zeszle.T_pco);
    printf("| Regulator: %s\n", regulator_zeszle.F_zm);
    printf("------------------------------\n");
}


void updateDeviceValues()
{
    strncpy(elektrocieplownia_zeszle.T_o, elektrocieplownia_obecne.T_o, MAX_RECORD_SIZE);
    strncpy(elektrocieplownia_zeszle.T_zm, elektrocieplownia_obecne.T_zm, MAX_RECORD_SIZE);

    strncpy(wymiennik_zeszle.T_pm, wymiennik_obecne.T_pm, MAX_RECORD_SIZE);
    strncpy(wymiennik_zeszle.T_zco, wymiennik_obecne.T_zco, MAX_RECORD_SIZE);

    strncpy(budynek_zeszle.T_pco, budynek_obecne.T_pco, MAX_RECORD_SIZE);

    strncpy(regulator_zeszle.F_zm, regulator_obecne.F_zm, MAX_RECORD_SIZE);
}

void resetConnectionsTable()
{
    for(int i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        deviceConnected[i] = false;
    }
}

void resetSentDataTable()
{
    for(int i = 0; i < NUMBER_OF_DEVICES; i++)
    {
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
    if (argc < 4) {
        printf("Usage: %s port_no day_of_the_month hour_to_start_from\n", argv[0]);
        exit(2);
    }

//this snippet is to avoid waiting for timeout after closing socket
    int yes=1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) 
    {
        perror("setsockopt");
        exit(1);
    }
//end of snippet

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
    resetSentDataTable();

    //use the following if you want to start from present time
    //timer = time(NULL); //initial time the same as current system time
    //info = localtime(&timer);
    
    //convert data to day and hour
    timer = 24*60*60*(atoi(argv[2])-1) + 60*60*(atoi(argv[3])-1);
    info = localtime(&timer); 
    
    
    strftime(buffer, 9, "%d %H:%M", info);
    printf("Time in buffer: %s\n", buffer);

    while(true) 
    {     
        printf("%d.%d Waiting for connection.\n", iteration, cmd);
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

        const char separator = '!';
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
                //printf("We are still missing someone :(\n");
                send(sockfd2, nop, sizeof("nop"), 0);
            }               
        }
        else if(strcmp(command, "GetTime") == 0)
        {
            printf("Date set to: %s\n", buffer);

            send(sockfd2, buffer, sizeof("DD HH:MM"), 0);
        }
        else if(strcmp(command, "GetParams") == 0)
        {
            int dataSize = 0;
            char strTo[3*MAX_RECORD_SIZE+2] = "";
            switch(clientNumber) //0 - E, 1 - W, 2 - B, 3 - R, 4 - 
            {
                case 0:
                    printf("Elektrocieplownia - zadnych danych\n");
                    break;
                case 1:
                    printf("Wymiennik - Tpco, Tzm, Fzm\n");
                    dataSize = 3*MAX_RECORD_SIZE+2;   
   
                    strcat(strTo, budynek_zeszle.T_pco);
                    strcat(strTo, "!");
                    strcat(strTo, elektrocieplownia_zeszle.T_zm);
                    strcat(strTo, "!");
                    strcat(strTo, regulator_zeszle.F_zm);  

                    send(sockfd2, strTo, dataSize, 0);
                    break;
                case 2:
                    printf("Budynek - Tzco, To\n");
                    dataSize = 2*MAX_RECORD_SIZE+1;   

                    strcat(strTo, wymiennik_zeszle.T_zco);
                    strcat(strTo, "!");
                    strcat(strTo, elektrocieplownia_zeszle.T_o);
 
                    send(sockfd2, strTo, 2*MAX_RECORD_SIZE+1, 0);
                    break;
                case 3:
                    printf("Regulator - Tzco, To\n");
                    dataSize = 2*MAX_RECORD_SIZE+1;   

                    strcat(strTo, wymiennik_zeszle.T_zco);
                    strcat(strTo, "!");
                    strcat(strTo, elektrocieplownia_zeszle.T_o);
 
                    send(sockfd2, strTo, 2*MAX_RECORD_SIZE+1, 0);
                    break;
                default:
                    printf("Unknown client has sent 'GetParams'\n");
            }
        }

        else if(strcmp(command, "SendData") == 0)
        {
            //memset(inBuf, 0, BUFFER_SIZE);

            if(clientNumber < NUMBER_OF_DEVICES)
            {
                deviceSentData[clientNumber] = true;
            }  

            char *data1 = NULL;
            char *data2 = NULL;

            switch(clientNumber) //0 - E, 1 - W, 2 - B, 3 - R, 4 - 
            {
                case 0:
                    data1 = strtok(NULL, &separator);
                    data2 = strtok(NULL, &separator);
                    if(data1 && data2)
                    {
                        printf("We got %s and %s\n", data1, data2);
                        strncpy(elektrocieplownia_obecne.T_o, data1, MAX_RECORD_SIZE);
                        strncpy(elektrocieplownia_obecne.T_zm, data2, MAX_RECORD_SIZE);               
                    }                
                    break;
                
                case 1:
                    data1 = strtok(NULL, &separator);
                    data2 = strtok(NULL, &separator);
                    if(data1 && data2)
                    {
                        printf("We got %s and %s\n", data1, data2);
                        strncpy(wymiennik_obecne.T_pm, data1, MAX_RECORD_SIZE);
                        strncpy(wymiennik_obecne.T_zco, data2, MAX_RECORD_SIZE);               
                    } 
                    break;
               case 2:
                    printf("Received data: %s\n", inBuf);
                    data1 = strtok(NULL, &separator);
                    if(data1)
                    {
                        printf("We got  %s\n", data1);
                        strncpy(budynek_obecne.T_pco, data1, MAX_RECORD_SIZE);              
                    } 
                    break;
               case 3:
                    data1 = strtok(NULL, &separator);
                    if(data1)
                    {
                        printf("We got %s\n", data1);
                        strncpy(regulator_obecne.F_zm, data1, MAX_RECORD_SIZE);               
                    } 
                    break;

                default:
                    printf("Unknown client has sent 'Data'\n");
            }
        }

        cmd++;
        memset(inBuf, 0, BUFFER_SIZE);

        if(allClientsSentData())
        {
            if(allClientsConnected())
            {               
                printf("Iteration is complete, go to next\n");
                resetConnectionsTable();
                fastForwardTimeInMinutes(10);
                iteration++;
                cmd = 0;
            }   
            resetSentDataTable();
            printf("Time in system (day & time): %s\n", buffer);
            printAllData(); 
            updateDeviceValues();
            printAllData();
        }   
    }

    free(info);
    close(sockfd);
    return SUCCESS;
}

