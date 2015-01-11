#include "def.h"
#include "config.h"
#include "client.h"
#include "log.h"
#include "thread.h"
#include "gpio.h"
#include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>


u8 heartbeat_s =HEARTBEAT_TIMEOUT;
u8 heartbeat_timeout = 0;



int g_sockfd_client = -1;
int g_reconnect_flag = RECONNECT_OFF;

char buf[1024*1024*2];
u8 buf_send[1024];


int check_head(u8 *_buf)
{
	if((_buf[0] != PROTOCOL_HEAD_1)||(_buf[1] != PROTOCOL_HEAD_2))
		return -1;
	return 0;
}

int check_protocol_head(u8 * _buf,  u8 _protocol_id)
{
	if((_buf[0] != PROTOCOL_HEAD_1)||(_buf[1] != PROTOCOL_HEAD_2)||(_buf[2] != _protocol_id)||(_buf[3] != ALARM_DEVICE_ID))
		return -1;
	return 0;
}

void make_ack_head( u8 * _buf, u8 _protocol_num)
{
	_buf[0] = PROTOCOL_HEAD_1;
	_buf[1] = PROTOCOL_HEAD_2;
	_buf[2] = _protocol_num;
	_buf[3] = ALARM_DEVICE_ID;	
}

u8 make_crc_num(u8 * _buf, u8 _num)
{
	return 0;
}

int check_for_crc(u8 * _buf, u8 _num)
{
	return 0;
}

int check_for_get_device_attr(u8 * _buf, u8 _num)
{
	if(_num < 5) 
		return ERROR_NUM_IS_NOT_ENOUGH;
		
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_DEVICE_ATTR))
		return ERROR_HEAD_OR_DEVICE_ID;
		
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;
	
	return 0;
}

int make_ack_get_device_attr(u8 * _buf)
{
	make_ack_head(_buf,(u8)PROTOCOL_ACK_GET_DEVICE_ATTR);
	_buf[4] = 0x00;
	_buf[5] = 0x00;
	_buf[6] = make_crc_num(_buf,6);
	return 7;
}
int check_for_set_device_attr(u8 * _buf, u8 _num)
{
	if(_num < 7) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_SET_DEVICE_ATTR))
		return ERROR_HEAD_OR_DEVICE_ID;
	
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;
	
	return 0;
}

int make_ack_set_device_attr(u8 * _buf)
{
	make_ack_head(_buf,(u8)PROTOCOL_ACK_SET_DEVICE_ATTR);
	_buf[4] = 0x00;
	_buf[5] = 0x00;
	_buf[6] = make_crc_num(_buf,6);
	return 7;
}


int check_for_get_alarm_status(u8 * _buf, u8 _num)
{
	if(_num < 5) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_ALARM_STATUS))
		return ERROR_HEAD_OR_DEVICE_ID;
	
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;
		
	return 0;
}

int make_ack_get_alarm_status(u8 * _buf, u8 alarm_in[])
{
	make_ack_head(_buf,(u8)PROTOCOL_ACK_GET_ALARM_STATUS);
	_buf[4+ALARM_SUFFIX0] = alarm_in[ALARM_SUFFIX0];
	_buf[4+ALARM_SUFFIX1] = alarm_in[ALARM_SUFFIX1];
	_buf[4+ALARM_SUFFIX2] = alarm_in[ALARM_SUFFIX2];
	_buf[4+ALARM_SUFFIX3] = alarm_in[ALARM_SUFFIX3];
	_buf[4+ALARM_SUFFIX4] = alarm_in[ALARM_SUFFIX4];
	_buf[4+ALARM_SUFFIX5] = alarm_in[ALARM_SUFFIX5];
	_buf[4+ALARM_SUFFIX6] = alarm_in[ALARM_SUFFIX6];
	_buf[4+ALARM_SUFFIX7] = alarm_in[ALARM_SUFFIX7];
	
	
	_buf[4+ALARM_MAX] = make_crc_num(_buf, (4+ALARM_MAX));
	return (4+ALARM_MAX+1);
}

int check_for_set_time(u8 * _buf, u8 _num)
{
	if(_num < 11) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_ALARM_STATUS))
		return ERROR_HEAD_OR_DEVICE_ID;
	
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;
		
	return 0;
}

int make_get_time(u8 * _buf)
{
	make_ack_head(_buf,(u8)PROTOCOL_GET_TIME);	
	_buf[4] = make_crc_num(_buf,4);
	return 5;
}

int check_for_set_heartbeat(u8 * _buf, u8 _num)
{
	if(_num < 6) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_SET_HEARTBEAT))
		return ERROR_HEAD_OR_DEVICE_ID;
	heartbeat_s = _buf[4];
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;
		
	return 0;
}

int make_ack_set_heartbeat(u8 * _buf, u8 _heartbeat_s)
{
	make_ack_head(_buf,(u8)PROTOCOL_ACK_SET_HEARTBEAT);	
	_buf[4] = _heartbeat_s;
	_buf[5] = make_crc_num(_buf,5);
	return 6;
}


void printf_hex(char *buf, int n)
{
	int i;
	
	for (i =0; i < n; i++){
		printf("%02x ", buf[i]);
		if ((i+1)%16 == 0)
			printf("\n");
	}
	printf("\n");
}


void set_heartbeat(int status, u8 time)
{
	if (HEARTBEAT_ONLINE == status){
		g_reconnect_flag = HEARTBEAT_ONLINE;
		heartbeat_s = time;
		heartbeat_timeout = 0;
	}

	if (HEARTBEAT_OFFLINE == status){
		g_reconnect_flag = HEARTBEAT_OFFLINE;
		heartbeat_s = time;
		heartbeat_timeout = 0;
	}
	
}
int client_init(void)
{
	struct sockaddr_in  addr;
	struct in_addr in_ip;
	int ret = -1;
	
	g_sockfd_client =  socket( AF_INET, SOCK_STREAM, 0 );
	if (g_sockfd_client < 0){
		perror("socket");
		return FAILURE;
	}
	memset(&addr , 0, sizeof(struct sockaddr_in ));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = g_conf_info.con_server.server_ip;
	addr.sin_port = htons(g_conf_info.con_server.server_port);	
	
	ret = connect(g_sockfd_client,(const struct sockaddr *)&addr, sizeof(addr));
	in_ip.s_addr = g_conf_info.con_server.server_ip;
	sys_log(FUNC, LOG_WARN, "ip:%s: %d, ret=%d", inet_ntoa(in_ip) , \
		g_conf_info.con_server.server_port, ret);	
	if (ret < 0){
		g_reconnect_flag = RECONNECT_ON;
		//led_ctrl(LED_D3, LED_OFF);
		return FAILURE;
	}
	return SUCCESS;	
}


/*
case 1: server configuration(ip , port) updated
case 2: server offline(No heartbeat)
*/
int client_reconnect(void)
{
	if (g_sockfd_client > 0){
		close(g_sockfd_client);
		g_sockfd_client = -1;
	}
	led_ctrl(LED_D3_ALARM_SERVER_STATUS, LED_OFF);
	while (FAILURE == client_init()){
		if (g_sockfd_client > 0){
			close(g_sockfd_client);
			g_sockfd_client = -1;
		}		
		sleep(CLIENT_RECONNECT_TIMEOUT);
	}	

	return SUCCESS;
}


void client_process(void)
{
	fd_set readfds;		
	int ret;
	int num_read_from_socket;
	int num_to_send;	
	int res = -1;
	struct timeval tv;
	u8 alarm_in[ALARM_MAX];	
	
	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(g_sockfd_client, &readfds);
		tv.tv_sec = 0;
		tv.tv_usec = 1000;
		
		heartbeat_timeout ++;
		sys_log(FUNC, LOG_DBG, "heartbeat_timeout = %d heartbeat_s=%d",heartbeat_timeout, heartbeat_s);
		if (heartbeat_timeout > heartbeat_s){
			sys_log(FUNC, LOG_ERR, "heartbeat timeout  --> reconnect");
			set_heartbeat(HEARTBEAT_OFFLINE,  HEARTBEAT_TIMEOUT);			
		}
		sleep(1);		
		if (g_reconnect_flag == RECONNECT_ON || g_reconnect_flag == HEARTBEAT_OFFLINE){
			if (SUCCESS == client_reconnect()){
				g_reconnect_flag = RECONNECT_OFF;	
			}			
		}
		if (g_sockfd_client <=0){
			sys_log(FUNC, LOG_WARN, "g_sockfd_client");
			g_reconnect_flag = RECONNECT_ON;
			continue;
		}		

		

		res = select(g_sockfd_client + 1, &readfds, NULL, NULL, &tv);
		if (res < 0){
			continue;
		}else if (res > 0){
			if (FD_ISSET(g_sockfd_client, &readfds))
				num_read_from_socket = read(g_sockfd_client, buf, 100);
		}else{
			continue;
		}
		
		if(num_read_from_socket <= 0){
			continue;
		}
		sys_log(FUNC, LOG_WARN, "recv %d bytes!\n",num_read_from_socket);
		printf_hex(buf,num_read_from_socket);		

		if (check_head(buf) == -1 ){
			continue;
		}		
		
		switch (buf[2]){
			case PROTOCOL_GET_DEVICE_ATTR:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_DEVICE_ATTR");
				
				if(!check_for_get_device_attr(buf,num_read_from_socket))
				{
					num_to_send = make_ack_get_device_attr(buf_send);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send)
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
				break;
			case PROTOCOL_SET_DEVICE_ATTR:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_SET_DEVICE_ATTR");
				if(!check_for_set_device_attr(buf,num_read_from_socket))
				{
					num_to_send = make_ack_set_device_attr(buf_send);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send)
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
				break;
			case PROTOCOL_GET_ALARM_STATUS:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_ALARM_STATUS");
				if(!check_for_get_alarm_status(buf,num_read_from_socket))
				{
					get_alarm(alarm_in);					
					num_to_send = make_ack_get_alarm_status(buf_send,alarm_in);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
				break;
			case PROTOCOL_SET_TIME:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_SET_TIME");
				/*
				if(!check_for_set_device_attr(buf,num_read_from_socket))
				{
					num_to_send = make_ack_set_device_attr(buf_send);
					ret = write(sockfd, buf_send, num_read_from_socket);
							if(ret != num_read_from_socket) printf("write socket error!\n");
				}*/
				break;
			case PROTOCOL_SET_HEARTBEAT:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_SET_HEARTBEAT");
				if(!check_for_set_heartbeat(buf,num_read_from_socket))
				{
					num_to_send = make_ack_set_heartbeat(buf_send,heartbeat_s);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send)
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);

					set_heartbeat(HEARTBEAT_ONLINE, buf[4]);	
					led_ctrl(LED_D3_ALARM_SERVER_STATUS, LED_ON);
				}
				break;
				
			default:
				sys_log(FUNC, LOG_ERR, "wrong cmd from server ");
				break;				
		}
		
	}	
}
	
void client_thread(void)
{
	TRD_t client_trd;
	
	trd_create(&client_trd, (void*)&client_process, NULL);
}






