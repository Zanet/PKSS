#include <stdio.h>
#include "string.h"
#include "json.h"

char jsonMessage[JSON_MAX_LEN];

// Json template to send
const char JSON_TEMPLATE[JSON_MAX_LEN] =
"PKSS Symulacyjny model instalacji grzewczej\r\n"
"From: %s\r\n"
"To(Socket number): %d\r\n"
"Data length: %d\r\n"
"Data: %s\r\n"
"Time: %d\r\n"
"\r\n"
;

char *jsonCompute(char* hostIP, int socket_number, int data_len, char* data, int time)
{


	sprintf(jsonMessage, JSON_TEMPLATE, hostIP, socket_number, data_len,
		data, time);

	return jsonMessage;

}
