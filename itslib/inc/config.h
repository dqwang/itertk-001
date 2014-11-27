//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   config.h
/// @brief  配置定义
///
///
///
/// @version    2.0
/// @author     xuliang<gxuliang@gmail.com>
/// @date       2010－04－24
///
///
///     修订说明：最初版本
//////////////////////////////////////////////////////////////////////////#include <stdio.h>

#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "def.h"





#define USR_NAME_LEN    24
#define USR_PSW_LEN     24
#define USR_SALT        "NC616"
#define MAX_COM_PORT    16
#define MAX_SESSION     6
#define MAX_USR_NUM     16
/**    本类的功能：打印错误信息
*
*     本类是一个单件
*     在程序中需要进行错误信息打印的地方
*/
typedef struct tagCONFIG_SYS
{
    char  host_name[32];
    char  host_pos[32];
    char  host_sn[32];
    char  sw_ver[32];
    char  fpga_ver[32];
    char  web_ver[32];
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
    int alarm[2];
}CONFIG_GPIO;



typedef struct tagUSR_INFO
{    
	WORD usr_id;    
	BYTE usr_name[USR_NAME_LEN];   //!< user name    
	BYTE usr_passwd[USR_PSW_LEN];  //!< password    
	BYTE usr_auth;	               //!< user authority, 0 - guest, 1 - normal user，2-administrator，3-super user    
	BYTE usr_state;                //!< user state    
	BYTE reserved[4];	           //!<
} USR_INFO, *p_USR_INFO;

#endif

