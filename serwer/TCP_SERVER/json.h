#ifndef INC_JSON_H
#define INC_JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

#pragma once

// JSON defines preparation
#define JSON_MAX_LEN                512

char *jsonCompute(char* hostIP, int socket_number, int data_len, char* data, int time);

#ifdef __cplusplus
}
#endif

#endif

