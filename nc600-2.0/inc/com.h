//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   com.h
/// @brief  串口相关声明
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
#include <stdio.h>
#include "config.h"

#ifndef __COM_H__
#define __COM_H__


#define TTY_DEV "/dev/ttyS"

enum    COM_CHK
{
    CHK_ODD = 1,//偶校验
    CHK_QI,
    CHK_NONE,
};

enum    COM_TYPE
{
    RS232 = 0,
    RS485,
    RS422,
};

//////////////////////////////////////////////////////////////////////////
///
///     串口初始化
///     @param argc 参数个数
///     @param *argv[] 参数内容
///     @return always 0
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
extern void com_init(void);
//////////////////////////////////////////////////////////////////////////
///
///     串口数据发送
///     @param id 串口序号
///     @param *data 数据首地址
///     @param len 数据长度
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
inline int SendNetDataToCom(int id, void *data, int len);
//////////////////////////////////////////////////////////////////////////
///
///     串口参数设置
///     @param fd 串口文件描述符
///     @param con_com 参数结构体
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
extern void com_para_set(int fd, CONFIG_COM *con_com);

    
extern void com_set(CONFIG_COM *con_com);

extern pthread_mutex_t rs485_mutex;		/*rs485半双工锁*/

#endif
