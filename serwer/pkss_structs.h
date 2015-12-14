#ifndef __PKSS_STRUCTS__
#define __PKSS_STRUCTS__

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>


const int MAX_RECORD_SIZE = 10;


typedef struct {
    char T_o[MAX_RECORD_SIZE];
    char T_zm[MAX_RECORD_SIZE];
} Elektrocieplownia;

typedef struct {
    char T_pm[MAX_RECORD_SIZE];
    char T_zco[MAX_RECORD_SIZE];
} Wymiennik;

typedef struct {
    char T_pco[MAX_RECORD_SIZE];
    char F_zco[MAX_RECORD_SIZE];    
    char P[MAX_RECORD_SIZE];
    char I[MAX_RECORD_SIZE];
    char T_bud[MAX_RECORD_SIZE];
} Budynek;

typedef struct {
    char F_zm[MAX_RECORD_SIZE];
    char P[MAX_RECORD_SIZE];
    char I[MAX_RECORD_SIZE];
} Regulator;

#endif
