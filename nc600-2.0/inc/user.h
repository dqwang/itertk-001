//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   user.h
/// @brief  用户定义头文件
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

#ifndef __USER_H__
#define __USER_H__

#include "def.h"

#define USR_NAME_LEN    24
#define USR_PSW_LEN     24
#define USR_SALT        "NC616"

enum USR_AUTH
{
    USR_NORMAL,
    USR_ROOT,
};

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

