//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   net.c
/// @brief  处理网络数据
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

#ifndef __NET_H__
#define __NET_H__
#include "def.h"


#define TIMEOUT_CNT 2000
#define WEB_IP      "127.0.0.1"
#define WEB_PORT        32000

///Net Connection information
typedef struct tagNET_CONN_INFO
{
    int conn_idx;		            //!< Index of connection
    int client_conn;	            //!< client socket descriptor
    BYTE client_type;	            //!< client type
    BYTE file_type;		            //!< file type, 0 - picture, 1 - video, 2 -- break-point continue
    BYTE idle;	            		//!< idle time count
    char clientip[16];

} NET_CONN_INFO, *pNET_CONN_INFO;
//////////////////////////////////////////////////////////////////////////
///
///     关闭连接
///     @param  *conn_info  连接信息
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern void net_conn_close(NET_CONN_INFO *conn_info);
extern int net_conn_connect(NET_CONN_INFO *conn_info);
extern int net_conn_send(NET_CONN_INFO *conn_info, ITSIP *net_head, BYTE* net_data, DWORD len);
extern int net_conn_recv(NET_CONN_INFO *conn_info, void* data, DWORD len);


#endif

