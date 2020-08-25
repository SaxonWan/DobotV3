#ifndef __EMW3080_H
#define __EMW3080_H

#include "main.h"
#include "configuration.h"
#include "usart.h"
#include "string.h"
#include "hmac_sha1.h"



#define DEFAULT_TIMEOUT			5
#define MAX_CMD_LEN 			400


bool emw3080_init(void);
bool Ali_connect(void);
bool service_pub_set(const char* service_name);
bool status_pub_set(void);
bool pub_data(const char* msg);

#endif
