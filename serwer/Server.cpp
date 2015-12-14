#include "pkss_structs.h"

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
FILE *logger;
FILE *csvPrinter;
char TimeBuffer[9];
char logBuffer[100];

void logToFile()
{
    logger = fopen("2015pkss.log", "a");
    if(logger)
    {
        fprintf(logger, "%s", logBuffer);
        fclose(logger);
    }
}

#define BUDYNEK 1
#define WYMIENNIK 2

void czytaj_nastawy(char dlaKogo)
{
        char bufor[40];
        char *data;
        char separator = ',';
        FILE * nastawy;

        if(dlaKogo == BUDYNEK) {
            nastawy = fopen("nastawy_bud.txt", "r");
            if(nastawy)
            {   
                fscanf(nastawy, "%s", bufor);
                //printf("Bufor: %s\n", bufor);

                data = strtok(bufor, &separator);
                strncpy(budynek_zeszle.P, data, MAX_RECORD_SIZE);
                data = strtok(NULL, &separator);
                strncpy(budynek_zeszle.I, data, MAX_RECORD_SIZE);
                fclose(nastawy);
            }
        }
        else if (dlaKogo == WYMIENNIK) {
            nastawy = fopen("nastawy_wym.txt", "r");
            if(nastawy)
            {   
                fscanf(nastawy, "%s", bufor);
                //printf("Bufor: %s\n", bufor);

                data = strtok(bufor, &separator);
                strncpy(regulator_zeszle.P, data, MAX_RECORD_SIZE);
                data = strtok(NULL, &separator);
                strncpy(regulator_zeszle.I, data, MAX_RECORD_SIZE);
                fclose(nastawy);
            }
        }

        
}


void fastForwardTimeInMinutes(int minutes)
{    //printf("Current time is: %s\n", ctime(&timer));
            
    timer += 60*minutes;
    //printf("Current time after modification is: %s\n", ctime(&timer));

    info = localtime(&timer);

    //info->tm_min += minutes;

    strftime(TimeBuffer, 9, "%d %H:%M", info);
}

/*void printHeaderForCsv()
{
    char buffer[150];
    csvPrinter = fopen("data.csv", "a");
    if(csvPrinter)
    {                                     //elektro   wymiennik     budynek      regulator
        sprintf(buffer, "day, hour, minute, T_o. T_zm, T_pm, T_zco, T_pco, F_zco, F_zm, P, I\n");      
        fprintf(csvPrinter, "%s", buffer);  
        fclose(csvPrinter);       
    }
}*/

void printAllData()
{
    printf("--------DATA IN SYSTEM--------\n");
    printf("| Elektrocieplownia: %s, %s\n", elektrocieplownia_zeszle.T_o, elektrocieplownia_zeszle.T_zm);
    printf("| Wymiennik: %s, %s\n", wymiennik_zeszle.T_pm, wymiennik_zeszle.T_zco);
    printf("| Budynek: %s, %s, %s, %s\n", budynek_zeszle.T_pco, budynek_zeszle.F_zco, budynek_zeszle.P, budynek_zeszle.I);
    printf("| Regulator: %s, %s, %s\n", regulator_zeszle.F_zm, regulator_zeszle.P, regulator_zeszle.I);
    printf("------------------------------\n");
    
    csvPrinter = fopen("data.csv", "a");
    if(csvPrinter)
    {
        char CSVbuffer[150];
        if(iteration == 0)
        {   
            sprintf(CSVbuffer, "day, hour, minute, T_o, T_zm, T_pm, T_zco, T_pco, F_zco, Bud_P, Bud_I, F_zm, Wym_P, Wym_I, T_bud\n");      
            fprintf(csvPrinter, "%s", CSVbuffer);   
        }
        
        sprintf(CSVbuffer, "%d, %d, %d, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s\n",
                info->tm_mday, info->tm_hour, info->tm_min,
                elektrocieplownia_zeszle.T_o, elektrocieplownia_zeszle.T_zm,
                wymiennik_zeszle.T_pm, wymiennik_zeszle.T_zco,
                budynek_zeszle.T_pco, budynek_zeszle.F_zco, budynek_zeszle.P, budynek_zeszle.I,
                regulator_zeszle.F_zm, regulator_zeszle.P, regulator_zeszle.I,
                budynek_zeszle.T_bud);
        fprintf(csvPrinter, "%s", CSVbuffer); 
        fclose(csvPrinter);
    }
}


void updateDeviceValues()
{
    strncpy(elektrocieplownia_zeszle.T_o, elektrocieplownia_obecne.T_o, MAX_RECORD_SIZE);
    strncpy(elektrocieplownia_zeszle.T_zm, elektrocieplownia_obecne.T_zm, MAX_RECORD_SIZE);

    strncpy(wymiennik_zeszle.T_pm, wymiennik_obecne.T_pm, MAX_RECORD_SIZE);
    strncpy(wymiennik_zeszle.T_zco, wymiennik_obecne.T_zco, MAX_RECORD_SIZE);

    strncpy(budynek_zeszle.T_pco, budynek_obecne.T_pco, MAX_RECORD_SIZE);
    strncpy(budynek_zeszle.F_zco, budynek_obecne.F_zco, MAX_RECORD_SIZE);
    strncpy(budynek_zeszle.T_bud, budynek_obecne.T_bud, MAX_RECORD_SIZE);

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
    if (argc < 5) {
        printf("Usage: %s port_no day_of_the_month hour_to_start_from mins_to_add\n", argv[0]);
        exit(2);
    }
    const int minsToBeAdded = atoi(argv[4]);
    printf("Time will be added: %d\n", minsToBeAdded);

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
    czytaj_nastawy(WYMIENNIK);
    czytaj_nastawy(BUDYNEK);
    //use the following if you want to start from present time
    //timer = time(NULL); //initial time the same as current system time
    //info = localtime(&timer);
    
    //convert data to day and hour
    timer = 24*60*60*(atoi(argv[2])-1) + 60*60*(atoi(argv[3])-1);
    info = localtime(&timer); 
    
    
    strftime(TimeBuffer, 9, "%d %H:%M", info);
    //printf("Time in buffer: %s\n", buffer);

    while(true) 
    {     
        printf("%d.%d Waiting for connection.\n", iteration, cmd);
        addrlen = sizeof(cli_name);
        sockfd2 = accept(sockfd, (struct sockaddr*) &cli_name, &addrlen);
        if(sockfd2 == -1) {
            printf("accept() error\n");
            //sockfd2 = accept(sockfd, (struct sockaddr*) &cli_name, &addrlen);
            continue;
        }
        status = recv(sockfd2, inBuf, sizeof(inBuf), 0);
        if (status == -1) {
            printf("recv() error\n");
            //status = recv(sockfd2, inBuf, sizeof(inBuf), 0);
            //exit(5);
            continue;
        }
        
        if ( inet_ntop( AF_INET, &cli_name.sin_addr, addr_buf, INET_ADDRSTRLEN ) == NULL )
        {
            printf("Ip address problem\n"); 
            //exit(1); 
        }
       
        printf("%s\n", inBuf);

        if(strcmp(inBuf, "engcmd!closeServer") == 0)
        {
            printf("Closed manually");
            close(sockfd2);
            break;             
        }
        

        if(strchr(inBuf, '!')) //string contains '!'
        {
            const char separator = '!';
            char *id = strtok(inBuf, &separator);
            char *command = strtok(NULL, &separator);        
            if(id != NULL && command != NULL)
            {
                //printf("ID: %s ", id);
                //printf("Command: %s\n", command);
            }        
            //useless - every client is seen in the same way
            //printf( "...from [%s:%d]\n",addr_buf, ntohs( cli_name.sin_port ));       
     
            unsigned int clientNumber = 255; //0 - E, 1 - W, 2 - B, 3 - R,4 - 

            if (strcmp(id, "E") == 0)
            {
                clientNumber = 0;
                //printf("E - Elektrociepłownia\n");
            }
            else if(strcmp(id, "W") == 0)
            {
                clientNumber = 1;
                //printf("W - Wymiennik ciepła\n");
            } 
            else if(strcmp(id, "B") == 0)
            {
                clientNumber = 2;
                //printf("B - Budynek\n");
            } 
            else if(strcmp(id, "R") == 0)
            {
                clientNumber = 3;
                //printf("R - Regulator\n");
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
                    //fastForwardTimeInMinutes(10);
                    //resetConnectionsTable();
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
                printf("Date set to: %s\n", TimeBuffer);

                send(sockfd2, TimeBuffer, sizeof("DD HH:MM"), 0);
            }
            else if(strcmp(command, "GetParams") == 0)
            {
                int dataSize = 0;
                char strTo[4*MAX_RECORD_SIZE+3] = "";
                switch(clientNumber) //0 - E, 1 - W, 2 - B, 3 - R, 4 - 
                {
                    case 0:
                        //printf("Elektrocieplownia - zadnych danych\n");
                        dataSize = 3*MAX_RECORD_SIZE+2; 
                        strcat(strTo, wymiennik_zeszle.T_pm);
                        send(sockfd2, strTo, dataSize, 0);
                        break;
                    case 1:
                        //printf("Wymiennik - Tpco, Tzm, Fzm\n");
                        dataSize = 4*MAX_RECORD_SIZE+3;   
       
                        strcat(strTo, budynek_zeszle.T_pco);
                        strcat(strTo, "!");
                        strcat(strTo, elektrocieplownia_zeszle.T_zm);
                        strcat(strTo, "!");
                        strcat(strTo, regulator_zeszle.F_zm);
                        strcat(strTo, "!");
                        strcat(strTo, budynek_zeszle.F_zco);
                        send(sockfd2, strTo, dataSize, 0);
                        break;
                    case 2:
                        //printf("Budynek - Tzco, To\n");
                        czytaj_nastawy(BUDYNEK);
                        dataSize = 4*MAX_RECORD_SIZE+3;   

                        strcat(strTo, wymiennik_zeszle.T_zco);
                        strcat(strTo, "!");
                        strcat(strTo, elektrocieplownia_zeszle.T_o);
                        strcat(strTo, "!");
                        strcat(strTo, budynek_zeszle.P);
                        strcat(strTo, "!");
                        strcat(strTo, budynek_zeszle.I);

                        printf("Wyslano: %s\n", strTo);
                        send(sockfd2, strTo, dataSize, 0);
                        break;
                    case 3:
                        //printf("Regulator - Tzco, To\n");
                        czytaj_nastawy(WYMIENNIK);
                        dataSize = 4*MAX_RECORD_SIZE+3;   

                        strcat(strTo, wymiennik_zeszle.T_zco);
                        strcat(strTo, "!");
                        strcat(strTo, elektrocieplownia_zeszle.T_o);
                        strcat(strTo, "!");
                        strcat(strTo, regulator_zeszle.P);
                        strcat(strTo, "!");
                        strcat(strTo, regulator_zeszle.I);
     
                        send(sockfd2, strTo, dataSize, 0);
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
                char *data3 = NULL;
                //strtok(NULL,...) perform strtok again on inBuf
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
                        //printf("Received data: %s\n", inBuf);
                        data1 = strtok(NULL, &separator);
                        data2 = strtok(NULL, &separator);
                        data3 = strtok(NULL, &separator);
                        if(data1)
                        {
                            printf("We got  %s and %s\n", data1, data2);
                            strncpy(budynek_obecne.T_pco, data1, MAX_RECORD_SIZE);
                            strncpy(budynek_obecne.F_zco, data2, MAX_RECORD_SIZE);
                            strncpy(budynek_obecne.T_bud, data3, MAX_RECORD_SIZE);               
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

                    updateDeviceValues();
                }
            }
        }

        cmd++;
        memset(inBuf, 0, BUFFER_SIZE);

        if(allClientsSentData())
        {
            if(allClientsConnected())
            {                              
                resetConnectionsTable();
                printf("Iteration is complete, go to next\n");
                
                iteration++;
                cmd = 0;
            } 
  
            
            fastForwardTimeInMinutes(minsToBeAdded);
            resetSentDataTable();
            //sprintf(logBuffer, "Time in system (day & time): %s\n", buffer);
            printf("Time in system (day & time): %s\n", TimeBuffer);
            //logToFile();
            //printAllData(); 
            updateDeviceValues();
            printAllData();
        }  
        close(sockfd2); 
    }

    free(info);
    close(sockfd);
    return 0;
}


/*void getPIParamsFromFile()
{
    static FILE *fp;
    fp = fopen("nastawy.pi", "r");
    if(fp)
    {
        size_t paramLength = 0;
        char *data = 0;

        getdelim(&data, &paramLength, '!' , fp);
        printf("Read from file: %s\n", data);

        free(data);
        fclose(fp);
    }    
}*/

