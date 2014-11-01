//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   web_net.h
/// @brief  与web进程的通信
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

#ifndef __WEB_NET_H__
#define __WEB_NET_H__

#include "def.h"
#include "user.h"
#include "thread.h"
#define MAX_TCP_CONN    9
#define WEB_PORT        32000
#define TIMEOUT_CNT     2000

///Net Connection information
typedef struct tagNET_CONN_INFO
{
    int conn_idx;		            //!< Index of connection
    int client_conn;	            //!< client socket descriptor
    USR_INFO *user;		            //!< pointer to user data in memory
    BYTE client_type;	            //!< client type
    BYTE file_type;		            //!< file type, 0 - picture, 1 - video, 2 -- break-point continue
    BYTE idle;	            		//!< idle time count
    char clientip[16];
    LOCK_t lock;
} NET_CONN_INFO, *pNET_CONN_INFO;
//////////////////////////////////////////////////////////////////////////
///
///     web 模块初始化
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern void web_init(void);
//////////////////////////////////////////////////////////////////////////
///
///     web 模块处理客户端连接
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern void web_process(void);


#endif

