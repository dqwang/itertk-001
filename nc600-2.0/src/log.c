//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   log.c
/// @brief  日志相关函数定义
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
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include "log.h"
#include <string.h>

//////////////////////////////////////////////////////////////////////////
///
///     打印日志
///     @param  module  函数名称
///     @param  level   日志等级
///     @param  format  数据内容
///     @return SUCCESS FAILURE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
int sys_log ( char* module, LOG_Lv level, char* format, ... )
{
    char szbuf[1024*8+256] = "", szcontent[1024*8] = "";
    va_list ap;
    struct timeval tv;
    struct tm stm;
    const char * g_log_lv[] =
    {
        "NULL",
        "TRACE",
        "DBG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
    };

    const int color[] = {0, 37, 36, 32, 33, 31, 35};
    
    va_start ( ap, format );
    vsprintf ( szcontent, format, ap );
    va_end ( ap );

    gettimeofday ( &tv, NULL );
    stm = * ( localtime ( &tv.tv_sec ) );

    if(szcontent[strlen(szcontent) - 1] == '\n') 
        szcontent[strlen(szcontent) - 1] = '\0';
    
    sprintf ( szbuf, "\033[%d;40m%04d-%02d-%02d %02d:%02d:%02d:%06ld---[%s][%s] %s\033[0m\n",color[level],
                stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, tv.tv_usec,
                module, g_log_lv[level], szcontent );
        
    printf ( "%s", szbuf );
    return 1;

}
