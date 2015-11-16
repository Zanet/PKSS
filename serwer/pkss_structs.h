#ifndef __PKSS_STRUCTS__
#define __PKSS_STRUCTS__

typedef struct {
    float T_o;
    float T_zm;
} Elektrocieplownia;

typedef struct {
    float T_pm;
    float T_zco;
} Wymiennik;

typedef struct {
    float T_pco;
} Budynek;

typedef struct {
    float F_zm;
} Regulator;

#endif
