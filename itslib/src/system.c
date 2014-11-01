//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   system.c
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
#include <string.h>
#include "system.h"

//////////////////////////////////////////////////////////////////////////
///
///     打印版本信息
///     @param *stream fprintf的第一个参数
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
void sys_version_show ( FILE *stream )
{
    fprintf ( stream, "************************************************************************************\n" );
    fprintf ( stream, "****		Product Name: %s\n", PRODUCT_NAME );
    fprintf ( stream, "****		Program Version: %s\n", PROGRAM_VERSION );
    fprintf ( stream, "****		Build Time: %s %s\n", __DATE__, __TIME__ );
    fprintf ( stream, "************************************************************************************\n" );
}


//////////////////////////////////////////////////////////////////////////
///
///     ip地址字符转转换成DWORD
///     @param *str 字符串
///     @return ip
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
DWORD sys_str2ip ( char *str )
{
    BYTE *p = (BYTE*)str;
    BYTE cIp;
    DWORD ip = 0;
    signed char j = 0;

    cIp = (BYTE ) atoi ( (char*)p );
    ip += ( ( ( DWORD ) ( cIp ) ) << ( j * 8 ) );
    j++;

    while ( ( *p != 0 ) && ( j >= 0 ) )
    {
        if ( *p != '.' )
        {
            p++;
            continue;
        }

        cIp =  (BYTE ) atoi ( (char*)++p );
        ip += ( ( ( DWORD ) ( cIp ) ) << ( j * 8 ) );
        j++;
    }

    return ( ip );
}


//////////////////////////////////////////////////////////////////////////
///
///     ip地址字符转转换成DWORD
///     @param ip ip地址
///     @return *str ip字符串
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
char* sys_ip2str_static ( DWORD ip )
{
    BYTE i;
    BYTE cIp[4];
    DWORD iIp = ( ip );
    static char str [16];
    memset ( str, 0, sizeof ( str ) );

    for ( i = 0; i < 4; i++ )
    {
        cIp[i] = ( BYTE ) ( ( iIp >> ( i * 8 ) ) & ( 0xff ) );
    }

    sprintf ( str, "%d.%d.%d.%d", cIp[0], cIp[1], cIp[2], cIp[3] );
//    printf("sys_ip2str_static: ip = %d, iIp = %d, str is %s\n", ip, iIp, str);

    return str;
}

void sys_ip2str(DWORD ip, BYTE *str)
{
	BYTE i;
	BYTE cIp[4];
	DWORD iIp = ip;
	
	for (i = 0; i < 4; i++)
	{
		cIp[i] = (BYTE)( (iIp>>(i*8)) & (0xff) );
	}
	sprintf((char*)str, "%d.%d.%d.%d", cIp[0], cIp[1], cIp[2], cIp[3]);
}


//////////////////////////////////////////////////////////////////////////
///
///     Get the count of millisecond
///     @return mscnt -- count of microsecond
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
UQWORD system_mscount_get(void)
{
    struct timeval		tv;
    struct timezone	tz;
    UQWORD  mscnt = 0;

    gettimeofday(&tv, &tz);
    mscnt = (tv.tv_sec * CLOCKS_PER_MSEC) + (tv.tv_usec / CLOCKS_PER_MSEC);
    return mscnt;
}

