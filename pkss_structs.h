#ifndef __PKSS_STRUCTS__
#define __PKSS_STRUCTS__

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
} Budynek;

typedef struct {
    char F_zm[MAX_RECORD_SIZE];
} Regulator;

#endif
