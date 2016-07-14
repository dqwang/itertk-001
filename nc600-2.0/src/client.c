#include "def.h"
#include "config.h"
#include "client.h"
#include "log.h"
#include "thread.h"
#include "cmd_def.h"
#include "gpio.h"
#include "com.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

CONFIG_COM get_com_attr;
CONFIG_COM set_com_attr;


extern u8 g_alarm_in[ALARM_MAX];

u8 heartbeat_s =HEARTBEAT_TIMEOUT;
u16 heartbeat_timeout = 0;/*FIX BUG001:*/



int g_sockfd_client = -1;
int g_reconnect_flag = RECONNECT_OFF;
int g_sockfd_client_status = SOCKFD_CLIENT_NULL;

char buf[1024*1024*2];
u8 buf_send[1024];

void printf_hex(char *buf, int n);

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

void make_ack_head( u8 * _buf, u8 _protocol_id)
{
	_buf[0] = PROTOCOL_HEAD_1;
	_buf[1] = PROTOCOL_HEAD_2;
	_buf[2] = _protocol_id;
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
#if 0
int make_ack_get_device_attr(u8 * _buf)
{
	make_ack_head(_buf,(u8)PROTOCOL_ACK_GET_DEVICE_ATTR);
	_buf[4] = 0x00;
	_buf[5] = 0x00;
	_buf[6] = make_crc_num(_buf,6);
	return 7;
}
#else

int gen_one_sensor_buf(CONFIG_SENSOR * pcs, u8 * buf)
{
   u8 i;

   if (pcs == NULL || buf == NULL){
      sys_log(FUNC, LOG_ERR , "CONFIG_SENSOR pointer is %p, u8 pointer is %p", pcs, buf);
      return -1;
   }
   buf[0] = pcs->type;
   buf[1] = pcs->num;

   if (pcs->num == 0){
      buf[2] = 0;
      buf[3] = 0;
      sys_log(FUNC, LOG_ERR, "CONFIG_SENSOR %d,  num is %d", pcs->type, pcs->num);
      return 4;
   }
   
   for (i = 0;i < pcs->num; i += 2){
         buf[2+i] = pcs->seq_num;
         buf[3+i] = pcs->attr;
   }

   sys_log(FUNC, LOG_ERR, "CONFIG_SENSOR %d,  num is %d", pcs->type, pcs->num);
   return (2 + 2*pcs->num);
}


int gen_all_sensor_buf(u8 *buf)
{
   int cnt=0;
   int index=0;
   
   if (buf == NULL){
      sys_log(FUNC, LOG_ERR, "u8 pointer is %p", buf);
      return -1;
   }

   
   cnt = gen_one_sensor_buf(&g_conf_info.con_sensor[SENSOR_TYPE_UART], buf);
   index = cnt;
   cnt = gen_one_sensor_buf(&g_conf_info.con_sensor[SENSOR_TYPE_IO_IN], buf+index);
   index +=cnt;
   cnt = gen_one_sensor_buf(&g_conf_info.con_sensor[SENSOR_TYPE_IO_OUT], buf+index);
   index +=cnt;
   cnt = gen_one_sensor_buf(&g_conf_info.con_sensor[SENSOR_TYPE_SINGLE_BUS], buf+index);
   index +=cnt;

   sys_log(FUNC, LOG_DBG, "index is %d", index);
   return index;   
}


int make_ack_get_device_attr(u8 * _buf)
{
   int cnt=0;
   
   make_ack_head(_buf,(u8)PROTOCOL_ACK_GET_DEVICE_ATTR);
	_buf[4] = SENSOR_TYPE_NUM;

   cnt = gen_all_sensor_buf(buf+4);
   
	_buf[5+cnt] = 0x00;
	_buf[6+cnt] = make_crc_num(_buf,6+cnt);
	return 7+cnt;
}
#endif
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
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_SET_TIME))
		return ERROR_HEAD_OR_DEVICE_ID;
	
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;
		
	return 0;
}


int make_ack_set_time(u8 * _buf)
{
	make_ack_head( _buf, (u8)PROTOCOL_SET_TIME);
	_buf[4] = make_crc_num( _buf, 4);/*crc data num= sizeof(buf)-1*/
	return 4+1;	
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

int check_for_get_uart_qty(u8* _buf, u8 _num)
{
	if(_num < 5) return ERROR_NUM_IS_NOT_ENOUGH;
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_UART_QTY))
		return ERROR_HEAD_OR_DEVICE_ID;
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;
	
	return 0;
}

int make_ack_get_uart_qty(u8 * _buf)
{
	make_ack_head(_buf, PROTOCOL_ACK_UART_QTY);
	_buf[4]=MAX_COM_PORT;/*2 UART*/
	_buf[5]=make_crc_num(_buf, 5);
	return 6;
	
}

#define INVALID_ID 0xff

int check_for_get_uart_attr(u8* _buf, u8 _num)
{
	if(_num < 6) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_UART_ATTR))
		return ERROR_HEAD_OR_DEVICE_ID;
	if (_buf[4]==1 || _buf[4] ==2){
		get_com_attr.id=_buf[4]-1;
	}else{
		get_com_attr.id=_buf[4];
	}

	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;

	return 0;
}

#define calc_high_byte(x) (x/0x10000)
#define calc_mid_byte(x) (x%0x10000/0x100)
#define calc_low_byte(x) (x%0x10000%0x100)

#define calc_baud_rate(a,b,c) (a*0x10000+b*0x100+c)

u8 return_local_odd_even(u8 remote_oddeven)
{
	switch(remote_oddeven){
		case 0:
			return 3;//none
		case 1:
			return 2;//odd
		case 2:
			return 1;//even
		default:
			break;
	}	
	return -1;
}

u8 return_remote_odd_even(u8 local_oddeven)
{
	switch(local_oddeven){
		case 3:
			return 0;//none
		case 2:
			return 1;//odd
		case 1:
			return 2;//even
		default:
			break;
	}	
	return -1;
}



int make_ack_get_uart_attr(u8* _buf)
{
	make_ack_head(_buf, PROTOCOL_ACK_GET_UART_ATTR);
	
	if (get_com_attr.id <2){
		_buf[4]=get_com_attr.id+1;
		
		memcpy(&get_com_attr,  &g_conf_info.con_com[get_com_attr.id], sizeof(CONFIG_COM));
			
		
		_buf[5]=calc_high_byte(get_com_attr.bps);
		_buf[6]=calc_mid_byte(get_com_attr.bps);
		_buf[7]=calc_low_byte(get_com_attr.bps);
		_buf[8]=get_com_attr.dbit;
		_buf[9]=get_com_attr.sbit;

		_buf[10]=return_remote_odd_even(get_com_attr.chk);
		
	}else{
		_buf[4]=get_com_attr.id;
		_buf[5]=0;
		_buf[6]=0;
		_buf[7]=0;
		_buf[8]=0;
		_buf[9]=0;
		_buf[10]=0;
	}	
	_buf[11]=make_crc_num(_buf, 11);
	return 12;
}


int check_for_set_uart_attr(u8* _buf, u8 _num)
{
	if(_num < 12) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_SET_UART_ATTR))
		return ERROR_HEAD_OR_DEVICE_ID;

	set_com_attr.id=_buf[4]-1;
	set_com_attr.bps = calc_baud_rate(_buf[5], _buf[6], _buf[7]);
	set_com_attr.dbit = _buf[8];
	set_com_attr.sbit = _buf[9];
	set_com_attr.chk =return_local_odd_even( _buf[10]);

	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;

	return 0;
}

#define SET_UART_ATTR_PASS 1
#define SET_UART_ATTR_FAIL 0

u8  is_good_uart(CONFIG_COM * pcom)
{
	u8 ret = 0;
	switch (pcom->bps)
	{
		case 300:
		case 600:		
		case 1200:			
		case 2400:			
		case 4800:		
		case 9600:
		case 19200:		
		case 38400:				
		case 57600:		
		case 115200:
		case 230400:
		case 460800:
		case 576000:
		case 921600:
			ret += 1; 
		break;
		
		default:
			sys_log(FUNC, LOG_DBG, "Baud rate Error: %d",pcom->bps);
			return SET_UART_ATTR_FAIL;
		
	}
	switch (pcom->dbit)
	{
		case 5:			
		case 6:			
		case 7:		
		case 8:
			ret += 1; 
		break;

		default:
			sys_log(FUNC, LOG_DBG, "Databit Error: %d",pcom->dbit);
			return SET_UART_ATTR_FAIL;
	}
	

	switch (pcom->chk)
	{
		case 3:		
		case 2://奇			
		case 1://偶
			ret += 1;
		break;
		
		default:
			sys_log(FUNC, LOG_DBG, "Checkbit Error: %d",pcom->chk);
			return SET_UART_ATTR_FAIL;
	}

	//set stop bits
	switch (pcom->sbit)
	{
		case 1:			
		case 2:
			ret += 1;
		break;
		
		default:
			sys_log(FUNC, LOG_DBG, "Stopbit Error: %d",pcom->sbit);
			return SET_UART_ATTR_FAIL;
	}

	return SET_UART_ATTR_PASS;
}


int make_ack_set_uart_attr(u8* _buf)
{
	make_ack_head( _buf, PROTOCOL_ACK_SET_UART_ATTR);

	if (set_com_attr.id <2 && is_good_uart(&set_com_attr) == SET_UART_ATTR_PASS){
		_buf[4]=set_com_attr.id+1;
		_buf[5]=SET_UART_ATTR_PASS;
		
		/* TODO */
		memcpy(&g_conf_info.con_com , &set_com_attr, sizeof(CONFIG_COM));
		/*still available if reboot*/
		config_save(&g_conf_info);
		/*set com*/
		com_set(&set_com_attr);
		
	}else{
		_buf[4]=set_com_attr.id+1;
		_buf[5]=SET_UART_ATTR_FAIL;
	}
	
	_buf[6]=make_crc_num(_buf, 6);
	return 7;
}

int check_for_query_uart_sendbuf(u8* _buf, u8 _num)
{
	if(_num < 6) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_QUERY_UART_SENDBUF))
		return ERROR_HEAD_OR_DEVICE_ID;
	get_com_attr.id = _buf[4]-1;
	

	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;

	return 0;
}

int make_ack_query_uart_sendbuf(u8* _buf)
{
	make_ack_head( _buf, PROTOCOL_ACK_QUERY_UART_SENDBUF);
	_buf[4]=get_com_attr.id+1;
	_buf[5]=0x08;
	_buf[6]=0x00;
	_buf[7]=make_crc_num(_buf,7);
	return 7+1;
}

u16 g_uart_send_cnt=0;

int check_for_uart_send_data(u8* _buf, u8 _num)
{
	
	if(_num < 8) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_UART_SEND_DATA))
		return ERROR_HEAD_OR_DEVICE_ID;
	
	if(check_for_crc(_buf,_num))
			return ERROR_CRC_CHECK;
	get_com_attr.id = _buf[4]-1;	
	g_uart_send_cnt=_buf[5]*0x100+_buf[6];
	
	return 0;
}

int  uart_send_data(u8 *_buf)
{
	int  ret=0;
	
	//get_com_attr.id = _buf[4]-1;	
	//g_uart_send_cnt=_buf[5]*0x100+_buf[6];	
	
	ret=SendNetDataToCom(get_com_attr.id, _buf+7, g_uart_send_cnt);
	if (ret == -1){
		ret=0;
	}
	return ret;
}

int make_ack_uart_send_data(u8* _buf)
{
	u16 cnt=0;
	
	make_ack_head( _buf, PROTOCOL_ACK_UART_SEND_DATA);
	_buf[4]=get_com_attr.id +1;

	if (get_com_attr.id >=MAX_COM_PORT ||g_uart_send_cnt ==0 ||g_uart_send_cnt>0x800){
		_buf[5]=0;
		_buf[6]=0;
	}else{
		cnt=uart_send_data(buf);
		_buf[5]=cnt/0x100;
		_buf[6]=cnt%0x100;
	}	
	_buf[7]=make_crc_num(_buf, 7);
	return 7+1;
}


//#define UART_RECV_POLL_MODE 0/*default mode :buffer data*/
//#define UART_RECV_INT_MODE 1/*send data in real time*/

int check_for_set_uart_recv_attr(u8* _buf, u8 _num)
{
	DWORD id;
	
	
	if(_num < 7) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_SET_UART_RECV_ATTR))
		return ERROR_HEAD_OR_DEVICE_ID;	

	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;
	id = _buf[4]-1;

	if (id == 0 ||id ==1){
		switch(_buf[5]){
			case UART_RECV_POLL_MODE:
				g_conf_info.con_com[id].rsvd[0]=UART_RECV_POLL_MODE;
				break;
			case UART_RECV_INT_MODE:
				g_conf_info.con_com[id].rsvd[0]=UART_RECV_INT_MODE;
				break;
			default:
				g_conf_info.con_com[id].rsvd[0]=UART_RECV_POLL_MODE;
				break;
		}
		config_save(&g_conf_info);
		
	}
	
	sys_log(FUNC, LOG_DBG, "%d\n", g_conf_info.con_com[id].rsvd[0]);


	return 0;
}

#define UART_RECV_BUF_SIZE 0X800

u8 uart1_recv_buf[UART_RECV_BUF_SIZE]={0};
u16 uart1_cnt=0;
u8 uart2_recv_buf[UART_RECV_BUF_SIZE]={0};
u16 uart2_cnt=0;

int check_for_query_uart_recv_data(u8* _buf, u8 _num)
{
	if(_num < 6) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_QUERY_UART_RECV_DATA))
		return ERROR_HEAD_OR_DEVICE_ID;
	
	
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;

	return 0;
}

int make_ack_query_uart_recv_data(u8* _buf)
{
	int recv_size;
	DWORD id=0;
	make_ack_head( _buf, PROTOCOL_ACK_QUERY_UART_RECV_DATA);

	id=buf[4]-1;
	
	_buf[4]=id+1;

	if (id==0){
		_buf[5]=uart1_cnt/0x100;
		_buf[6]=uart1_cnt%0x100;
		recv_size=uart1_cnt;
		memcpy(_buf+7, uart1_recv_buf, uart1_cnt);
	}else if (id==1){
		_buf[5]=uart2_cnt/0x100;
		_buf[6]=uart2_cnt%0x100;
		recv_size=uart2_cnt;
		memcpy(_buf+7, uart2_recv_buf, uart1_cnt);
	}else{
		_buf[5]=0;
		_buf[6]=0;
		recv_size=0;
	}
		
	_buf[recv_size+7]=make_crc_num(_buf, recv_size+7);

	/*clear buf*/
	return recv_size+7+1;
}

 

 void  Protocol_SendComDataToNet_PollMode(int id, void *data, int len)
{	
	u16 cnt1=0, cnt2=0;
	if (id==0){
		cnt1=uart1_cnt;
		uart1_cnt += len;
		if (uart1_cnt<UART_RECV_BUF_SIZE){
			memcpy(uart1_recv_buf+cnt1, data, len);
		}		
	}

	if (id==1){
		cnt1=uart2_cnt;
		uart2_cnt += len;
		if (uart2_cnt<UART_RECV_BUF_SIZE){
			memcpy(uart2_recv_buf+cnt2, data, len);
		}		
	}
}

 void  Protocol_SendComDataToNet_IntMode(int id, void *data, int len)
{	
	int num_to_send, ret;
	Protocol_SendComDataToNet_PollMode(id, data, len);
	num_to_send = make_ack_query_uart_recv_data(buf_send);
	
	ret = write(g_sockfd_client, buf_send, num_to_send);
	if(ret != num_to_send) 
		printf("write socket error!\n");
	sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
	printf_hex(buf_send, num_to_send);
	
}

int check_for_get_io_num(u8* _buf, u8 _num)
{
	if(_num < 5) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_IO_NUM))
		return ERROR_HEAD_OR_DEVICE_ID;

	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;

	return 0;
}

#define IO_NUM 3
int make_ack_get_io_num(u8* _buf)
{
	make_ack_head( _buf, PROTOCOL_ACK_GET_IO_NUM);
	_buf[4]=IO_NUM;
	_buf[5]=make_crc_num(_buf, 5);
	return 5+1;
}

int check_for_set_io(u8* _buf, u8 _num)
{
	if(_num < 7) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_SET_IO))
		return ERROR_HEAD_OR_DEVICE_ID;
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;	

	return 0;
}


int check_for_get_io_status(u8* _buf, u8 _num)
{
	if(_num < 6) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_IO_STATUS))
		return ERROR_HEAD_OR_DEVICE_ID;
	
	sys_log(FUNC, LOG_DBG, "%d\n", buf[4]);
	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;

	return 0;
}
	
int make_ack_get_io_status(u8* _buf)
{
	gpio_status gs=GS_LOW;

	make_ack_head( _buf, PROTOCOL_ACK_GET_IO_STATUS);
	_buf[4]=buf[4];
	//sys_log(FUNC, LOG_DBG, "%d %d\n", buf[4], _buf[4]);
	
	switch(_buf[4]){
		case 0:
			get_gpio(OUTPUT_1,&gs);	
			_buf[5]=(u8)gs;
			break;
		case 1:
			get_gpio(OUTPUT_2, &gs);
			_buf[5]=(u8)gs;
			break;
		case 2:
			get_gpio(OUTPUT_3, &gs);	
			_buf[5]=(u8)gs;
			break;
		default:
			_buf[5]=0xff;
			break;
	}	
	
	//sys_log(FUNC, LOG_DBG, "%d %d %x\n", buf[4], _buf[4], gs);
	_buf[6]=make_crc_num(_buf, 6);
	return 6+1;
}


int check_for_get_sensor_type(u8* _buf, u8 _num)
{
	if(_num < 6) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_SENSOR_TYPE))
		return ERROR_HEAD_OR_DEVICE_ID;
	

	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;

	return 0;
}
#define SENSOR_TYPE_AM2301 0
int make_ack_get_sensor_type(u8* _buf)
{
	make_ack_head( _buf, PROTOCOL_ACK_GET_SENSOR_TYPY);
	_buf[4]=buf[4];

	_buf[5]=SENSOR_TYPE_AM2301;

	
	_buf[6]=make_crc_num(_buf, 6);
	return 6+1;
}


int check_for_get_sensor_data(u8* _buf, u8 _num)
{
	if(_num < 6) return ERROR_NUM_IS_NOT_ENOUGH;
	
	if(check_protocol_head(_buf,(u8)PROTOCOL_GET_SENSOR_DATA))
		return ERROR_HEAD_OR_DEVICE_ID;
	

	if(check_for_crc(_buf,_num))
		return ERROR_CRC_CHECK;

	return 0;
}
#define AM2301_DATA_CNT 30
int make_ack_get_sensor_data(u8* _buf)
{
	make_ack_head( _buf, PROTOCOL_ACK_GET_SENSOR_DATA);
	_buf[4]=buf[4];

	_buf[5]=AM2301_DATA_CNT;

	get_sensor(g_conf_info.con_gpio.sensor);
	memcpy(_buf+6, g_conf_info.con_gpio.sensor, sizeof (g_conf_info.con_gpio.sensor));
		
	_buf[5+30+1]=make_crc_num(_buf, 36);
	return 36+1;
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

#define DNS_ON
#ifdef DNS_ON

#define MAX_SIZE_DNS 1024
#define SERVER_PORT_DNS 53

void setHead(unsigned char *buf)
{
	buf[0] = 0x00;
	buf[1] = 0;
	buf[2] = 0x01;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 1;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = buf[9] = buf[10] = buf[11] = 0;
}

void setQuery(char *name, unsigned char *buf, int len)
{
	int i;
	//strcat(buf+12,name);
	for(i=0;i<len;i++)
		buf[12+i] = name[i];
	int pos = len + 12;
	
	buf[pos] = 0;
	buf[pos+1] = 1;
	buf[pos+2] = 0;
	buf[pos+3] = 1;
}
int changeDN(char *DN,char *name)
{
	int i = strlen(DN) - 1;
	int j = i + 1;
	int k;
	
	name[j+1] = 0;
	for(k = 0; i >= 0; i--,j--) {
		if(DN[i] == '.') {
			name[j] = k;
			k = 0;
		}
		else {
			name[j] = DN[i];
			k++;
		}
	}
	name[0] = k;
	return (strlen(DN) + 2);
}
void printName(int len, char *name)
{
	  int i;
	  for(i = 0; i < len; i++) printf("%x.",name[i]);
	  printf("\n");
}

int sendDNSPacket(unsigned char *buf, int len, char *recvMsg, unsigned int dns_ip)
{
	int socket_fd;
	struct sockaddr_in sin;
	fd_set rfds;
	struct timeval tv;
	int retval;
	int ret=0;

	memset(&sin,0,sizeof(sin));
	//sin.sin_addr.s_addr = inet_addr("192.168.1.1");
	sin.sin_addr.s_addr = dns_ip;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT_DNS);

	socket_fd = socket(PF_INET,SOCK_DGRAM,0);
	//sys_log(FUNC, LOG_WARN, " %s %x", "debug 2", s);
	sendto(socket_fd,buf,len,0,(struct sockaddr *)&sin,sizeof(sin));	
	FD_ZERO(&rfds);
	FD_SET(socket_fd, &rfds);
	//sys_log(FUNC, LOG_WARN, " %s %x", "debug",rfds);
	/* Wait up to 2 seconds. */
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	retval = select(socket_fd+1, &rfds, NULL, NULL, &tv);
	/* Don't rely on the value of tv now! */

	if (retval == -1)
	perror("select()");
	else if (retval){		
		ret=recv(socket_fd,recvMsg,MAX_SIZE_DNS,0);			
	}		
	else{
		;//sys_log(FUNC, LOG_ERR, " %s", "dns failed");	
	}

	close(socket_fd);/*FIX BUG002*/
	return ret;

}

int resolve(unsigned char *recvMsg, int len, int len_recvMsg, char *ip)
{
	int pos = len;
	int cnt = 12;

	while(pos < len_recvMsg) {		
		unsigned char now_pos = recvMsg[pos+1];
		unsigned char retype = recvMsg[pos+3];
		unsigned char reclass = recvMsg[pos+5];
		unsigned char offset = recvMsg[pos+11];

		if(retype == 1) {			
			if(now_pos == cnt && reclass == 1) {
				//printf("%d.%d.%d.%d\n",recvMsg[pos+12],recvMsg[pos+13],recvMsg[pos+14],recvMsg[pos+15]);

				sprintf(ip, "%d.%d.%d.%d", recvMsg[pos+12],recvMsg[pos+13],recvMsg[pos+14],recvMsg[pos+15]);

				return DNS_OK;
			}
		}
		else if(retype == 5) {			
			cnt = pos + 12 ;
		}		
		pos = pos + 12 + offset;
	}

	return DNS_FAILED;
}


CMD_CODE its_dns(char *www , unsigned int dns, char *ip)
{	
	unsigned char buf[MAX_SIZE_DNS]; /* socket发送的数据 */
	char *DN=www; /* 将要解析的域名(www.xxx.xxx) */
	char name[MAX_SIZE_DNS]; /* 转换为符合DNS报文格式的域名 */
	char recvMsg[MAX_SIZE_DNS]; /* 接收的数据 */
	int len; /* socket发送数据的长度 */
	int len_recvMsg;
	int ret =-1;
	
	sys_log(FUNC, LOG_DBG, " www=%s, dns=0x%08x, ip=%s", www, dns, ip);
	
	len = changeDN(DN,name);	
	setHead(buf);
	setQuery(name,buf,len);
	len += 16;
	//sys_log(FUNC, LOG_DBG, " %s", "---->1");
	len_recvMsg = sendDNSPacket(buf,len,recvMsg, dns);
	//sys_log(FUNC, LOG_DBG, " %s", "---->2");
	ret = resolve(recvMsg,len,len_recvMsg, ip);
	//sys_log(FUNC, LOG_DBG, "---->3  ret=%d",ret);
	return ret;
}

/*
 * IP String Check
 */
#define TRUE 1
#define FALSE 0

int ip_check(char * ip)
{
	if(NULL == ip)	
		return FALSE;
	
	unsigned int result = 0;
	unsigned int d = 0;  
	int dotNum = 0;  
	int dNum = 0;  
	char c = 0;
	int char_exit=0;
	int i;
	for(i = 0; ; ++i){
		c = ip[i];
		if(c >= '0' && c <= '9')   {
			++ dNum;
			char_exit++;
			d = 10 * d + (c - '0');
			if(d > 255 || d < 0) 
				return FALSE;
		}else if(c == '.') {
			++ dotNum;
			if(dotNum > dNum)  
			{
				return FALSE;
			}else {
				result = (result<<8) | d;
				d = 0;
			}
			if (char_exit == 0)
				return -1;
			if (char_exit > 0)
				char_exit = 0;
		}else if(c == '\0')  {
			if(dotNum != 3 || char_exit == 0){
				return FALSE;
			}else
			{
				result = (result<<8) + d;
				return TRUE;
			}
		}else  {
			return FALSE;
		}
	}
}


int dns_resolution(char * dns_str)
{
	char ip_dest[16]="";
	struct hostent *h;
	
	sys_log(FUNC, LOG_DBG, " %s", "dns start :)");	

	if (ip_check(dns_str) == TRUE){
		h = gethostbyname(dns_str);	
		g_conf_info.con_server.server_ip = sys_str2ip(inet_ntoa(*((struct in_addr *)h->h_addr_list[0])));
		strcpy(g_conf_info.con_server.dns_str, dns_str);
		sys_log(FUNC, LOG_MSG, " %s", "DNS  OK gethostbyname");	
		return DNS_OK;
	}else if (strlen(g_conf_info.con_server.dns_str) !=0 ){
		if (its_dns(dns_str, g_conf_info.con_net.dns[0], ip_dest) == DNS_OK){
			 g_conf_info.con_server.server_ip = sys_str2ip(ip_dest);
			 sys_log(FUNC, LOG_MSG, " %s", "DNS  OK  its_dns:)");	
			 return DNS_OK;
		}else{
			sys_log(FUNC, LOG_ERR , " %s", "DNS  FAILED :(");
			return DNS_FAILED;
		}
		
	}
	return DNS_FAILED;	
}

#endif
int client_connect_server(void)
{
	struct sockaddr_in  alarm_server;
	struct in_addr in_ip;
	int ret = -1;
	int timeout=CLIENT_CONNECT_TIMEOUT;
	struct pollfd pollfd_s;
	int old_fl;
	int poll_ret;
	int error;
	int errorlen=sizeof(error);	

	/*socket*/
	g_sockfd_client =  socket( AF_INET, SOCK_STREAM, 0 );
	if (g_sockfd_client < 0){
		perror("socket");
		return FAILURE;
	}
	/*dns*/	
	ret=dns_resolution(g_conf_info.con_server.dns_str);
	if (DNS_FAILED == ret){
		return FAILURE;
	}
	memset(&alarm_server , 0, sizeof(struct sockaddr_in ));	
	alarm_server.sin_family = AF_INET;
	alarm_server.sin_addr.s_addr = g_conf_info.con_server.server_ip;
	alarm_server.sin_port = htons(g_conf_info.con_server.server_port);	

	in_ip.s_addr = g_conf_info.con_server.server_ip;

	sys_log(FUNC, LOG_WARN, "alarm server:ip %s: port %d, fd=%d, ret=%d", inet_ntoa(in_ip) , \
		g_conf_info.con_server.server_port, g_sockfd_client, ret);
	/*connect poll*/
	old_fl=fcntl(g_sockfd_client, F_GETFL, 0);
	if (fcntl(g_sockfd_client, F_SETFL, old_fl |O_NONBLOCK ) < 0) {
		goto err;
	}
	ret = connect(g_sockfd_client,(const struct sockaddr *)&alarm_server, sizeof(alarm_server));/*from block to nonblock */
	if ((-1 == ret)&&(errno != EINPROGRESS)){
		goto err;
	}
	pollfd_s.fd = g_sockfd_client;
	pollfd_s.events = POLLOUT | POLLERR | POLLHUP | POLLNVAL;
	errno = 0;

	poll_ret = poll(&pollfd_s, 1, timeout);
	if (0 == poll_ret){/*timeout*/
		goto err;
	}
	if (-1 == poll_ret){/*errors*/
		if ( EINTR == errno){
			goto err;
		}
		goto err;
	}
	if (pollfd_s.revents & (POLLERR | POLLHUP | POLLNVAL) ){
		goto err;
	}
	ret = getsockopt(g_sockfd_client, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&errorlen);
	if ((-1 == ret) || 0 != error ){
		goto err;
	}

	sys_log(FUNC, LOG_MSG, "Connect Alarm Server Successfully");
	fcntl(g_sockfd_client, F_SETFL, old_fl);
	return SUCCESS;
err:
	g_reconnect_flag = RECONNECT_ON;
	//led_ctrl(LED_D3, LED_OFF);
	if (g_sockfd_client > 0){
		g_sockfd_client_status = SOCKFD_CLIENT_NULL;
		close(g_sockfd_client);
		g_sockfd_client = -1;			
	}
	sys_log(FUNC, LOG_ERR, "Connect Alarm Server Failed");
	return FAILURE;
}


/*
case 1: server configuration(ip , port) updated
case 2: server offline(No heartbeat)
*/
int client_reconnect(void)
{
	static int counter =0;
	if (g_sockfd_client > 0){
		g_sockfd_client_status = SOCKFD_CLIENT_NULL;
		close(g_sockfd_client);
		g_sockfd_client = -1;		
	}
	led_ctrl(LED_D3_ALARM_SERVER_STATUS, LED_OFF);
	while (FAILURE == client_connect_server()){
		if (g_sockfd_client > 0){
			g_sockfd_client_status = SOCKFD_CLIENT_NULL;
			close(g_sockfd_client);
			g_sockfd_client = -1;
			
		}
		sys_log(FUNC, LOG_WARN, "Reconnect duration %dS:  counter: %d",CLIENT_RECONNECT_DURATION, ++counter);
		sleep(CLIENT_RECONNECT_DURATION);
			
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

	sys_log(FUNC, LOG_MSG, "start");
	while (1)
	{			
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
			//sys_log(FUNC, LOG_WARN, "g_sockfd_client");
			g_reconnect_flag = RECONNECT_ON;
			continue;
		}	
		
		FD_ZERO(&readfds);
		FD_SET(g_sockfd_client, &readfds);
		tv.tv_sec = 0;
		tv.tv_usec = 1000;		

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

		g_sockfd_client_status = SOCKFD_CLIENT_OK;
		
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
					num_to_send = make_ack_get_alarm_status(buf_send, g_alarm_in);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
				break;
			case PROTOCOL_GET_TIME:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_TIME");
				/*TODO ?*/
				
				break;
			case PROTOCOL_SET_TIME:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_SET_TIME");				
				if(!check_for_set_time(buf,num_read_from_socket))
				{
					/* TODO */
					/*set time*/
				
					num_to_send = make_ack_set_time(buf_send);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send)
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
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
			case PROTOCOL_GET_UART_QTY:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_UART_QTY");
				if(!check_for_get_uart_qty(buf,num_read_from_socket))
				{					
					num_to_send = make_ack_get_uart_qty(buf_send);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}

				break;
			case PROTOCOL_GET_UART_ATTR:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_UART_ATTR");
				if (!check_for_get_uart_attr(buf, num_read_from_socket)){
					num_to_send = make_ack_get_uart_attr(buf_send);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
				
				break;
			case PROTOCOL_SET_UART_ATTR:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_SET_UART_ATTR");
				if (!check_for_set_uart_attr(buf, num_read_from_socket)){
					num_to_send=make_ack_set_uart_attr(buf_send);
					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
				break;
			case PROTOCOL_QUERY_UART_SENDBUF:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_QUERY_UART_SENDBUF");
				if (!check_for_query_uart_sendbuf(buf, num_read_from_socket)){
					num_to_send = make_ack_query_uart_sendbuf(buf_send);

					/*TODO :buf*/

					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
			break;

			case PROTOCOL_UART_SEND_DATA:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_UART_SEND_DATA");
				if (!check_for_uart_send_data(buf, num_read_from_socket)){
					num_to_send = make_ack_uart_send_data(buf_send);

					/*TODO*/

					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
			break;
			case PROTOCOL_SET_UART_RECV_ATTR:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_SET_UART_RECV_ATTR");
				if (!check_for_set_uart_recv_attr(buf, num_read_from_socket)){
					
					/*TODO*/

					
				}
			break;

			case PROTOCOL_QUERY_UART_RECV_DATA:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_QUERY_UART_RECV_DATA");
				if (!check_for_query_uart_recv_data(buf, num_read_from_socket)){
					num_to_send = make_ack_query_uart_recv_data(buf_send);

					/*TODO*/

					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
			break;

			case PROTOCOL_GET_IO_NUM:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_IO_NUM");
				if (!check_for_get_io_num(buf, num_read_from_socket)){
					num_to_send = make_ack_get_io_num(buf_send);

					/*TODO*/

					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
			break;

			case PROTOCOL_SET_IO:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_SET_IO");
				if (!check_for_set_io(buf, num_read_from_socket)){
					
					/*TODO*/

					switch(buf[4]){
						case 0:
							set_gpio(OUTPUT_1, GD_OUT,buf[5]);
							break;
						case 1:
							set_gpio(OUTPUT_2, GD_OUT,buf[5]);
							break;
						case 2:
							set_gpio(OUTPUT_3, GD_OUT,buf[5]);
							break;
						default:
							break;
					}					
				}
			break;

			case PROTOCOL_GET_IO_STATUS:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_IO_STATUS");
				if (!check_for_get_io_status(buf, num_read_from_socket)){
								
					num_to_send = make_ack_get_io_status(buf_send);


					ret = write(g_sockfd_client, buf_send, num_to_send);
					if(ret != num_to_send) 
						printf("write socket error!\n");
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
			break;
			case PROTOCOL_GET_SENSOR_TYPE:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_SENSOR_TYPE");
				if (!check_for_get_sensor_type(buf, num_read_from_socket)){
					num_to_send = make_ack_get_sensor_type(buf_send);

					ret = write(g_sockfd_client, buf_send, num_to_send);
					if (ret != num_to_send){
						printf("write socket error!\n");
					}
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
				}
			break;

			case PROTOCOL_GET_SENSOR_DATA:
				sys_log(FUNC, LOG_DBG, "PROTOCOL_GET_SENSOR_DATA");
				if (!check_for_get_sensor_data(buf, num_read_from_socket)){
					num_to_send = make_ack_get_sensor_data(buf_send);

					ret = write(g_sockfd_client, buf_send, num_to_send);
					if (ret != num_to_send){
						printf("write socket error!\n");
					}
					sys_log(FUNC, LOG_DBG, "send %d bytes", num_to_send);
					printf_hex(buf_send, num_to_send);
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






