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
#include "system.h"
#include "config.h"
#include "log.h"
#include "web_net.h"
#include "com.h"
#include "mode.h"

//////////////////////////////////////////////////////////////////////////
///
///     程序入口
///     @param argc 参数个数
///     @param *argv[] 参数内容
///     @return always 0
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
int main ( int argc, char* argv[] )
{
static int cnt =0;
    sys_version_show ( stdout );
    config_init();
    web_init();
    mode_init();
    com_init();
    while(1)
    {
    sys_log(FUNC, LOG_DBG,"%d\n",cnt++);
        web_process();
    }
    return 0;
}
