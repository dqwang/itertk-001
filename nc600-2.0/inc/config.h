#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "def.h"
#include "system.h"
#include "user.h"


//#define CONFIG_PATH	"/usr/data/"
#define CONFIG_PATH	"/mnt/nand1-2/data/"

#define  CONFIG_FILE CONFIG_PATH""PRODUCT_NAME".cfg"

#define MAX_COM_PORT    1
#define MAX_SESSION     6
#define MAX_USR_NUM     16

#define SERVER_PORT	8888
//#define SERVER_IP "192.168.1.100"
#define SERVER_IP "192.168.1.118"
//#define SERVER_DNS "www.baidu.com"
#define SERVER_DNS "192.168.1.108"

#define DEVICE_IP "192.168.1.166"
#define HW_SN "RTNS2D1I3S2K-XX****X"

typedef struct tagCONFIG_SYS
{
    char  host_name[32];
    char  host_pos[32];
    char  host_sn[32];
    char  sw_ver[32];
    char  hw_ver[32];
    char  host_id[32];
    char  description[32];
    BYTE  rsvd[32];

} CONFIG_SYS;

typedef struct tagCONFIG_NET
{
    DWORD dev_ip;
    DWORD dev_nm;	//net mask
    DWORD dev_gw;	//gate way
    BOOL  dev_dhcp;
    DWORD dhcp_id;
    DWORD dns[2];
    BYTE  dev_mac[6];
    BYTE  rsvd[26];
}CONFIG_NET;

typedef struct tagCONFIG_COM
{
    DWORD id;
    DWORD bps;
    DWORD sbit;
    BYTE  dbit;
    BYTE  chk;
    BYTE  bctrl;
    BYTE  RTS;
    BYTE  DTR;
    BYTE  all;
    BYTE  type;		//RS232/RS485
    BYTE  isloop;
    BYTE  rsvd[4];
    DWORD alive;

} CONFIG_COM;

typedef struct tagCONFIG_SESSION
{

    DWORD protocol;
    DWORD ip;
    DWORD lport;
    DWORD dport;
    BYTE  conn;
    BYTE  dis_conn;
    BYTE  time_out;
    BYTE  max_num;
    BYTE  rsvd[3];
}CONFIG_SESSION;

typedef struct tagCONFIG_MODE
{
    DWORD id;
    BYTE  mode;
    BYTE  tcp_mode;
    BYTE  CR;
    BYTE  LF;
    BYTE  is_null;
    BYTE  attestation;//认证
    BYTE  att;//认证提示
    BYTE  reg;
    BYTE  rsvd[5];
    CONFIG_SESSION  session[MAX_SESSION];
} CONFIG_MODE;

typedef struct tagCONFIG_VLAN
{
	BOOL  enable;
	DWORD	id;
	char  rsvd[8];
}CONFIG_VLAN;

typedef struct tagCONFIG_LIMIT
{
    BYTE id;
    BYTE enable;
    BYTE read[MAX_USR_NUM];
    BYTE modify[MAX_USR_NUM];
}CONFIG_LIMIT;


typedef struct tagCONFIG_GPIO
{
	int alarm[3];
	char alarm_on_off[3];
	char output[4];
	char sensor[30];
}CONFIG_GPIO;


#define DNS_STR_LEN 50
typedef struct tagCONFIG_SERVER
{
	DWORD server_ip;/*server ip  / dns name*/	
	WORD server_port;
	char dns_str[DNS_STR_LEN];
}CONFIG_SERVER;


#define SINGLE_SENSOR_MAX_NUM 8
#define MAX_SENSOR_NUM 32

#define SENSOR_TYPE_UART 0
#define SENSOR_TYPE_IO_IN 1
#define SENSOR_TYPE_IO_OUT 2
#define SENSOR_TYPE_SINGLE_BUS 3

#define SENSOR_TYPE_NUM 4

typedef struct tagCONFIG_SENSOR{
   unsigned char type; 
   unsigned char num;     
   unsigned char seq_num[SINGLE_SENSOR_MAX_NUM];
   unsigned char attr[SINGLE_SENSOR_MAX_NUM];
}CONFIG_SENSOR;

typedef struct tagCONFIG_DATA
{
	CONFIG_SYS  con_sys;
	CONFIG_NET  con_net;
	CONFIG_VLAN  con_vlan;
	CONFIG_COM con_com[MAX_COM_PORT];
	CONFIG_MODE con_mode[MAX_COM_PORT];
	CONFIG_LIMIT con_lim[MAX_COM_PORT];
	USR_INFO con_usr[MAX_USR_NUM];
	CONFIG_SERVER con_server;
	CONFIG_GPIO con_gpio;
   CONFIG_SENSOR con_sensor[SENSOR_TYPE_NUM];
} CONFI_DATA;


#define MAX_THREAD_ID 20


extern  CONFI_DATA g_conf_info;
void reconfig(CONFI_DATA *pConf);
extern  void config_init ( void );
extern  void config_save ( CONFI_DATA *pConf );
extern USR_INFO* get_usr_info(BYTE *name);
extern int sys_usr_add(BYTE* name, BYTE* psw);
extern int set_usr_name(BYTE *oname, BYTE *nname);
extern int set_usr_psw(BYTE *name, BYTE *psw);

extern void config_net_set(CONFIG_NET *pConf);
extern void print_config(void);
#endif

