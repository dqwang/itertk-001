#ifndef _CLIENT_H_
#define _CLIENT_H_

enum protocol{
	PROTOCOL_GET_DEVICE_ATTR=0x01,
	PROTOCOL_ACK_GET_DEVICE_ATTR,
	PROTOCOL_SET_DEVICE_ATTR,
	PROTOCOL_ACK_SET_DEVICE_ATTR,
	
	PROTOCOL_GET_ALARM_STATUS,
	PROTOCOL_ACK_GET_ALARM_STATUS,
	PROTOCOL_GET_TIME,
	PROTOCOL_SET_TIME,
	
	PROTOCOL_SET_HEARTBEAT,
	PROTOCOL_ACK_SET_HEARTBEAT,
};

#define PROTOCOL_HEAD_1 0x55
#define PROTOCOL_HEAD_2 0xaa

#define ALARM_DEVICE_ID 0x01

#define ERROR_NUM_IS_NOT_ENOUGH	-1
#define ERROR_HEAD_OR_DEVICE_ID	-2
#define ERROR_CRC_CHECK			-3
#define ERROR_DATA_FORMAT		-4

#define HEARTBEAT_TIMEOUT 10

#define CLIENT_RECONNECT_DURATION 10//s
#define CLIENT_CONNECT_TIMEOUT 300//ms

#define RECONNECT_ON 1
#define RECONNECT_OFF 0
#define HEARTBEAT_OFFLINE 2
#define HEARTBEAT_ONLINE 3

#define SOCKFD_CLIENT_OK 1
#define SOCKFD_CLIENT_NULL 0



int client_connect_server(void);
int client_reconnect(void);
void client_process(void);
void client_thread(void);
int dns_resolution(char * dns_str);



#endif/*_CLIENT_H_*/

