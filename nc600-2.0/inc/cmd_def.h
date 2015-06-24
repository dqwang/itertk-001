//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   cmd_def.h
/// @brief  协议相关定义
///
///
///
/// @version    2.0
/// @author     xuliang<gxuliang@gmail.com>
/// @date       2010-04-24
///
///
///     修订说明：最初版本
//////////////////////////////////////////////////////////////////////////


#ifndef __CMD_DEF_H__
#define __CMD_DEF_H__

typedef enum tagCMD_CODE
{
	//connect
	CONN_FAILED = 1,
	SEND_FAILED,
	RECV_FAILED,

	//login
	LOGIN_OK,
	NO_USER,
	ERROR_PSW,

	//query
	QUERY_OK,
	QUERY_FAILED,

	//system set
	SYS_SET_OK,
	SYS_SET_FAILED,

	//net_set
	NET_SET_OK,
	NET_SET_FAILED,

	//Vlan_set
	VLAN_SET_OK,
	VLAN_SET_FAILED,

	//Factory_set
	FACTORY_SET_OK,
	FACTORY_SET_FAILED,

	//user 
	RC_NOUSR,
	RC_NORIGHT,
	RC_USREXIST,

	//user add
	USR_ADD_OK,
	USR_ADD_NORIGHT,
	USR_ADD_FAILED,

	//user del
	USR_DEL_OK,
	USR_DEL_NORIGHT,
	USR_DEL_FAILED,

	//user modify
	USR_MODIFY_OK,
	USR_MODIFY_NORIGHT,
	USR_MODIFY_FAILED,

	//com set
	COM_SET_OK,
	COM_SET_FAILED,

	//mode set
	MODE_SET_OK,
	MODE_SET_FAILED,

	//limit set
	LIMIT_SET_OK,
	LIMIT_SET_FAILED,

	//server set
	SERVER_SET_OK,
	SERVER_SET_FAILED,

	//DNS
	DNS_OK,
	DNS_FAILED,


	//GPIO
	GPIO_SET_OK,
	GPIO_SET_FAILED
	
}CMD_CODE;


enum USER_SET
{
    USER_ADD,
    USER_DEL,
    USER_MODIFY, 
};


enum 
{
    CONF_COM,
    CONF_MODE,
    CONF_LIMIT,
    CONF_GPIO,
    CONF_SERVER,
};


#endif


