//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file  libits.h
/// @brief  协议库头文件
///
///
///
/// @version    2.0
/// @author     xuliang<gxuliang@gmail.com>
/// @date       2010－04－24
///
///
///     修订说明：最初版本
//////////////////////////////////////////////////////////////////////////

#ifndef __ITSIP_H__
#define __ITSIP_H__

#include "def.h"

//! itsip protocol version
#define ITSIP_VERSION  0x01
//! the head of itsip head length
#define ITSIP_THLEN   8
#if 0
enum ITS_CMD
{
    ITS_HEAD              = 0xff,//!< ITSIP head
    ITS_LOGIN             = 0xa0,//!< User login
    ITS_SYSINFO_QUERY     = 0xa1,//!< System Information Query
    ITS_CONF_QUERY        = 0xa2,//!< Configuration Query
    ITS_CONF_SET          = 0xa3,//!< Configuration Setup
    ITS_LOG_QUERY         = 0xa4,//!< System Log Query
    ITS_USER_SET          = 0xa5,//!< User account setup
    ITS_SYS_UP            = 0xa6,//!< System upgrade
    ITS_SYSUP_LOGIN       = 0xa7,//!< System upgrade login
    ITS_CONN_TEST         = 0xa8,//!< Network connection test
    ITS_DELETE            = 0xa9,//!< File remove
    ITS_LOGOUT            = 0xaa,//!< Logout
    ITS_TIME_SET          = 0xab,//!< Time setup
    ITS_TALK_REQ          = 0xac,//!< Talking request
    ITS_KEEPLIVE          = 0xad,//!< Keep alive
    ITS_USER_QUERY		  = 0xae,//!< User accunt query
    ITS_SYSINFO_SET       = 0xaf,
    ITS_NETINFO_QUERY		= 0xc0,
    ITS_NETINFO_SET			= 0xc1,
    ITS_VLAN_QUERY		= 0xc2,
	ITS_VLAN_SET			= 0xc3,
	ITS_FACTORY_SET		= 0xc4,

    /*! \note command definitions of ACK
    */
    ITS_ACK_LOGIN         = 0xb0,/*!< ACK for user Login */
    ITS_ACK_SYSINFO_QUERY = 0xb1,/*!< ACK for System information query */
    ITS_ACK_CONF_QUERY    = 0xb2,/*!< ACK for configuration query */
    ITS_ACK_CONF_SET      = 0xb3,/*!< ACK for configuration setup */
    ITS_ACK_LOG_QUERY     = 0xb4,/*!< ACK for log query */
    ITS_ACK_USER_SET      = 0xb5,/*!< ACK for User account setup */
    ITS_ACK_SYS_UP        = 0xb6,/*!< ACK for system upgrade */
    ITS_ACK_SYSUP_LOGIN   = 0xb7,/*!< ACK for system upgrade login */
    ITS_ACK_CONN_TEST     = 0xb8,/*!< ACK for Network connection test */
    ITS_ACK_DELETE        = 0xb9,/*!< ACK for file remove */
    ITS_ACK_LOGOUT        = 0xba,/*!< ACK for Logout */
    ITS_ACK_TIME_SET      = 0xbb,/*!< ACK for time setup */
    ITS_ACK_TALK_REQ      = 0xbc,/*!< ACK for talking request */
    ITS_ACK_KEEPLIVE      = 0xbd,/*!< ACK for keep-alive */
    ITS_ACK_USER_QUERY	  = 0xbe,/*!< Ack for User accunt query */
    ITS_ACK_SYSINFO_SET   = 0xbf,
    ITS_ACK_NETINFO_QUERY = 0xd0,
    ITS_ACK_NETINFO_SET	  = 0xd1,
    ITS_ACK_VLAN_QUERY		= 0xd2,
	ITS_ACK_VLAN_SET			= 0xd3,
	ITS_ACK_FACTORY_SET		= 0xd4,
};
#endif
enum ITS_CMD
{
    ITS_HEAD              = 0xff,//!< ITSIP head
    ITS_LOGIN             = 0xa0,//!< User login
    ITS_SYSINFO_QUERY     = 0xa1,//!< System Information Query
    ITS_CONF_QUERY        = 0xa2,//!< Configuration Query
    ITS_CONF_SET          = 0xa3,//!< Configuration Setup
    ITS_LOG_QUERY         = 0xa4,//!< System Log Query
    ITS_USER_SET          = 0xa5,//!< User account setup
    ITS_SYS_UP            = 0xa6,//!< System upgrade
    ITS_SYSUP_LOGIN       = 0xa7,//!< System upgrade login
    ITS_CONN_TEST         = 0xa8,//!< Network connection test
    ITS_DELETE            = 0xa9,//!< File remove
    ITS_LOGOUT            = 0xaa,//!< Logout
    ITS_TIME_SET          = 0xab,//!< Time setup
    ITS_TALK_REQ          = 0xac,//!< Talking request
    ITS_KEEPLIVE          = 0xad,//!< Keep alive
	ITS_USER_QUERY		  = 0xae,//!< User accunt query
	ITS_SYSINFO_SET       = 0xaf,
	ITS_NETINFO_QUERY		= 0xc0,
	ITS_NETINFO_SET			= 0xc1,
	ITS_VLAN_QUERY		= 0xc2,
	ITS_VLAN_SET			= 0xc3,
	ITS_FACTORY_SET		= 0xc4,
	ITS_SERVER_QUERY		=0xc5,
	ITS_SERVER_SET		=0xc6,

    /*! \note command definitions of ACK
    */
    ITS_ACK_LOGIN         = 0xb0,/*!< ACK for user Login */
    ITS_ACK_SYSINFO_QUERY = 0xb1,/*!< ACK for System information query */
    ITS_ACK_CONF_QUERY    = 0xb2,/*!< ACK for configuration query */
    ITS_ACK_CONF_SET      = 0xb3,/*!< ACK for configuration setup */
    ITS_ACK_LOG_QUERY     = 0xb4,/*!< ACK for log query */
    ITS_ACK_USER_SET      = 0xb5,/*!< ACK for User account setup */
    ITS_ACK_SYS_UP        = 0xb6,/*!< ACK for system upgrade */
    ITS_ACK_SYSUP_LOGIN   = 0xb7,/*!< ACK for system upgrade login */
    ITS_ACK_CONN_TEST     = 0xb8,/*!< ACK for Network connection test */
    ITS_ACK_DELETE        = 0xb9,/*!< ACK for file remove */
    ITS_ACK_LOGOUT        = 0xba,/*!< ACK for Logout */
    ITS_ACK_TIME_SET      = 0xbb,/*!< ACK for time setup */
    ITS_ACK_TALK_REQ      = 0xbc,/*!< ACK for talking request */
    ITS_ACK_KEEPLIVE      = 0xbd,/*!< ACK for keep-alive */
    ITS_ACK_USER_QUERY	  = 0xbe,/*!< Ack for User accunt query */
    ITS_ACK_SYSINFO_SET   = 0xbf,
    ITS_ACK_NETINFO_QUERY = 0xd0,
    ITS_ACK_NETINFO_SET	  = 0xd1,
	ITS_ACK_VLAN_QUERY		= 0xd2,
	ITS_ACK_VLAN_SET			= 0xd3,
	ITS_ACK_FACTORY_SET		= 0xd4,
	ITS_ACK_SERVER_QUERY		=0xd5,
	ITS_ACK_SERVER_SET		=0xd6,
};


typedef struct tagITSIP
{
    BYTE itsip_head;     //!< head, 0xff
    BYTE itsip_cmd;      //!< command
    BYTE itsip_ver;      //!< protocol version
    BYTE itsip_thl;      //!< the head length
    WORD itsip_extlen;   //!< external data length
    BYTE itsip_dev_t;    //!< Device types
    BYTE itsip_rsvd;     //!< Reserved byte
    BYTE itsip_user[24]; //!< User
    BYTE itsip_data[24]; //!< data
} ITSIP, *p_ITSIP;


#define MAX_LENGTH  1024
typedef struct tagITSIP_PKT
{
    ITSIP head;				//!< ITSIP head
    BYTE data[MAX_LENGTH];	//!< external data, 1024 bytes
} ITSIP_PACKET, *p_ITSIP_PACKET;

//////////////////////////////////////////////////////////////////////////
///
///     填充协议内容
///     @param  cmd   命令字
///     @param  extlen  扩展长度
///     @param  dev_t   设备类型
///     @param  *data   扩展内容
///     @param  *its_pkt   协议结构体
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
inline void itsip_pack(BYTE cmd, WORD extlen, BYTE dev_t, void *data, ITSIP_PACKET *its_pkt);

#endif

