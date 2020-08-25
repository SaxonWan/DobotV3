#include "emw3080.h"
const char *AT                   		= "AT\r";
const char *AT_OK                 	= "OK";
//const char *AT_REBOOT             	= "AT+REBOOT\r";
const char *AT_ECHO_OFF           	= "AT+UARTE=OFF\r";
const char *AT_MSG_ON            		= "AT+WEVENT=ON\r";
const char *AT_RESET								= "AT+FACTORY\r";
const char *AT_WIFI_START         	= "AT+WJAP=%s,%s\r";
const char *AT_WIFI_START_SUCC    	= "+WEVENT:STATION_UP";
//const char *AT_WIFI_CLOSE         	= "AT+WJAPQ\r";
//const char *AT_WIFI_CLOSE_SUCC    	= "+WEVENT:STATION_DOWN";

const char *AT_MQTT_CONTENT					= "clientId%sdeviceName%sproductKey%stimestamp%s";
const char *AT_MQTT_AUTH         		= "AT+MQTTAUTH=%s&%s,%s\r";
const char *AT_MQTT_CID           	= "AT+MQTTCID=%s|securemode=3\\,signmethod=hmacsha1\\,timestamp=%s|\r";
const char *AT_MQTT_SOCK          	= "AT+MQTTSOCK=%s.iot-as-mqtt.cn-shanghai.aliyuncs.com,1883\r";
const char *AT_MQTT_CV_OFF        	= "AT+MQTTCAVERIFY=OFF,OFF\r";
const char *AT_MQTT_SSL_OFF       	= "AT+MQTTSSL=OFF\r";
const char *AT_MQTT_RECONN_ON     	= "AT+MQTTRECONN=ON\r";
const char *AT_MQTT_AUTOSTART_OFF 	= "AT+MQTTAUTOSTART=OFF\r";
const char *AT_MQTT_ALIVE         	= "AT+MQTTKEEPALIVE=50\r";
const char *AT_MQTT_START         	= "AT+MQTTSTART\r";
const char *AT_MQTT_START_SUCC    	= "+MQTTEVENT:CONNECT,SUCCESS";
const char *AT_MQTT_PUB_SET       	= "AT+MQTTPUB=/sys/%s/%s/thing/event/property/post,1\r";
const char *AT_MQTT_SER_PUB_SET			= "AT+MQTTPUB=/sys/%s/%s/thing/service/%s_reply,1\r";
const char *AT_MQTT_PUB_DATA      	= "AT+MQTTSEND=%d\r";
const char *AT_MQTT_PUB_DATA_SUCC 	= "+MQTTEVENT:PUBLISH,SUCCESS";

const char *AT_MQTT_SUB_START				= "AT+MQTTSUB=0,/sys/%s/%s/thing/service/Start,1\r";
const char *AT_MQTT_SUB_PAUSE				= "AT+MQTTSUB=1,/sys/%s/%s/thing/service/Pause,1\r";
const char *AT_MQTT_SUB_END					= "AT+MQTTSUB=2,/sys/%s/%s/thing/service/End,1\r";
const char *AT_MQTT_SUB_REFRESH			= "AT+MQTTSUB=3,/sys/%s/%s/thing/service/SuppliesRefresh,1\r";
const char *AT_MQTT_SUB_SUCC				=	"SUBSCRIBE,SUCCESS";
//const char *AT_MQTT_CLOSE         	= "AT+MQTTCLOSE\r";
//const char *AT_MQTT_CLOSE_SUCC    	= "+MQTTEVENT:CLOSE,SUCCESS";

#define ClearBuffer for(i = 0;i < MAX_CMD_LEN;i++)cmd_buffer[i] = '\0'

char cmd_buffer[MAX_CMD_LEN];
char password[44];

bool emw3080_init(void)
{
	bool flag;
	int i;
	
	flag = check_send_command(AT,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]AT No Response\r\n");
		return false;
	}
	printf("[INFO][EMW3080]AT Test Pass\r\n");
	
	flag = check_send_command(AT_RESET,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Reset Factory Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Reset to Factory Success\r\n");
	
	flag = check_send_command(AT_ECHO_OFF,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]AT Echo Cannot Turn OFF\r\n");
		return false;
	}
	printf("[INFO][EMW3080]AT Echo Off\r\n");
	
	flag = check_send_command(AT_MSG_ON,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Message Cannot Turn ON\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Message On\r\n");
	
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_WIFI_START,wifi_ssid,wifi_psw);
	flag = check_send_command(cmd_buffer,AT_WIFI_START_SUCC,20);
	if(!flag)
	{
		printf("[ERROR][EMW3080]WiFi Connect Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]WiFi Connect Success\r\n");
	
	return true;
}

bool Ali_connect(void)
{
	bool flag;
	int i;
	char content[MAX_CMD_LEN];
	
	ClearBuffer;
	
	snprintf(content,MAX_CMD_LEN,AT_MQTT_CONTENT,clientIDstr,deviceName,productKey,timestamp);
	printf("[INFO][MQTT]content:\r\n%s\r\n",content);
	get_mqtt_password(content, deviceSecret, password);
	printf("[INFO][SHA1]Result:\r\n%s\r\n",password);

	ClearBuffer;
	
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_AUTH,deviceName,productKey,password);
	flag = check_send_command(cmd_buffer,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali Auth Set Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali Auth Set Success\r\n");
	
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_CID,clientIDstr,timestamp);
	flag = check_send_command(cmd_buffer,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali Client Set Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali Client Set Success\r\n");
	
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_SOCK,productKey);
	flag = check_send_command(cmd_buffer,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali Socket Set Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali Socket Set Success\r\n");
	
	flag = check_send_command(AT_MQTT_CV_OFF,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali Certificate Verify Cannot Set OFF\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali Certificate Verify Set OFF\r\n");
	
	flag = check_send_command(AT_MQTT_SSL_OFF,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali SSL Cannot Set OFF\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali SSL Set OFF\r\n");
	
	flag = check_send_command(AT_MQTT_RECONN_ON,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali MQTT Reconnect Cannot Turn On\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali MQTT Reconnect On\r\n");
	
	flag = check_send_command(AT_MQTT_AUTOSTART_OFF,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali MQTT Autostart Cannot Turn OFF\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali MQTT Autostart Turn OFF\r\n");
	
	flag = check_send_command(AT_MQTT_ALIVE,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali MQTT Keep Alive Time Set Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali MQTT Keep Alive Time 50 Second\r\n");
	
	flag = check_send_command(AT_MQTT_START,AT_MQTT_START_SUCC,20);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Ali MQTT Start Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Ali MQTT Start Success\r\n");
	
	
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_PUB_SET,productKey,deviceName);
	flag = check_send_command(cmd_buffer,AT_OK,DEFAULT_TIMEOUT);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Pub Set Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Pub Set Success\r\n");
	
	//Subscibe
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_SUB_START,productKey,deviceName);
	flag = check_send_command(cmd_buffer,AT_MQTT_SUB_SUCC,20);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Sub Start Service Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Sub Start Service Success\r\n");
	
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_SUB_PAUSE,productKey,deviceName);
	flag = check_send_command(cmd_buffer,AT_MQTT_SUB_SUCC,20);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Sub Pause Service Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Sub Pause Service Success\r\n");
	
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_SUB_END,productKey,deviceName);
	flag = check_send_command(cmd_buffer,AT_MQTT_SUB_SUCC,20);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Sub End Service Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Sub End Service Success\r\n");
	
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_SUB_REFRESH,productKey,deviceName);
	flag = check_send_command(cmd_buffer,AT_MQTT_SUB_SUCC,20);
	if(!flag)
	{
		printf("[ERROR][EMW3080]Sub Refresh Service Fail\r\n");
		return false;
	}
	printf("[INFO][EMW3080]Sub Refresh Service Success\r\n");
	
	return true;
}


bool pub_data(const char* msg)
{
	int i;
	int len;
	bool flag;
	
	ClearBuffer;
	len = strlen(msg) - 1;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_PUB_DATA,len);
	flag = check_send_command(cmd_buffer,">",DEFAULT_TIMEOUT);
	if(flag) flag = check_send_command(msg,AT_MQTT_PUB_DATA_SUCC,20);
	tim4uart3 = 300;
	if(!flag)
	{
		//printf("[ERROR][EMW3080]Message:\r\n %s\n Pub Fail\r\n",msg);
		return false;
	}
	//printf("[INFO][EMW3080]Message:\r\n %s\n Pub Success\r\n",msg);
	return true;
}

bool status_pub_set(void)
{
	int i;
	int len;
	bool flag;
	tim4uart3 = 400;
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_PUB_SET,productKey,deviceName);
	flag = check_send_command(cmd_buffer,AT_OK,DEFAULT_TIMEOUT);
	return flag;
}

bool service_pub_set(const char* service_name)
{
	bool flag;
	int i;
	tim4uart3 = 400;
	ClearBuffer;
	snprintf(cmd_buffer,MAX_CMD_LEN,AT_MQTT_SER_PUB_SET,productKey,deviceName,service_name);
	flag = check_send_command(cmd_buffer,AT_OK,DEFAULT_TIMEOUT);
	
	return flag;
}