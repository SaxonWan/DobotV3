#ifndef ___HMAC_SHA1_H
#define ___HMAC_SHA1_H

#include "usart.h"
#include "string.h"
#include "md.h"


int get_mqtt_password(const char* content, const char* devicesecret, char* password);

#endif

