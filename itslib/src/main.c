//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   main.c
/// @brief  主流程的贯穿
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
#include <stdlib.h>
#include "def.h"
#include "utelnetd.h"
#include "thread.h"
#include "console.h"

int TelnetPort = 6000;//console port

//////////////////////////////////////////////////////////////////////////
///
///     程序入口
///     @param argc 参数个数
///     @param *argv[] 参数内容
///     @return always 0
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
int main ( int argc, char **argv )
{
    TRD_t telnet_trd;
    TRD_t console_trd;
    
    trd_create(&telnet_trd, (void*)&telnet_main, (void*)&TelnetPort);
    trd_create(&console_trd, (void*)&console_proc, (void*)&TelnetPort);
    while(1)
    {
       sleep(1); 
    }
    return 0;
}
