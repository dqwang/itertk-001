//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   mode.h
/// @brief  串口工作模式相关声明
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

#ifndef __MODE_H__
#define __MODE_H__

#include "config.h"

enum    WORK_MODE
{
    WORK_NO,
    WORK_SOCKET,
};

enum    TCP_MODE
{
    TCP_RAW,
    TCP_TELNET,
};

enum    TCP_PTL
{
    TCP_NO,
    TCP_CLI,
    TCP_SRV,
    TCP_UDP,
};

enum    CR_LF
{
    TO_CR,
    TO_LF,
};

typedef struct tagMODECONFIG
{
    int id;
    int fd;
    int sfd[MAX_SESSION];
    CONFIG_SESSION* session;
}MODECONFIG;
//////////////////////////////////////////////////////////////////////////
///
///     网络数据发送
///     @param id 网络序号
///     @param *data 数据首地址
///     @param len 数据长度
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
inline int SendComDataToNet(int id, void *data, int len);
//////////////////////////////////////////////////////////////////////////
///
///     网络初始化
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
extern void mode_init(void);


extern void report_dev_info_init(void);/*UDP*/
#endif
