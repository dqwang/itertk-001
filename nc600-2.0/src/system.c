//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, �㽭�����������޹�˾
///    All rights reserved.
///
/// @file   system.c
/// @brief  �����̵Ĺᴩ
///
///
///
/// @version    2.0
/// @author     xuliang<gxuliang@gmail.com>
/// @date       2010��04��24
///
///
///     �޶�˵��������汾
//////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "com.h"
//////////////////////////////////////////////////////////////////////////
///
///     ��ӡ�汾��Ϣ
///     @param *stream fprintf�ĵ�һ������
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010��04��24
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
///     ip��ַ�ַ�תת����DWORD
///     @param *str �ַ���
///     @return ip
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010��04��24
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

    return str;
}


//////////////////////////////////////////////////////////////////////////
///
///     Get the count of millisecond
///     @return mscnt -- count of microsecond
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-04-24
//////////////////////////////////////////////////////////////////////////
UQWORD system_mscount_get(void)
{
	struct timeval	tv;
	struct timezone	tz;
	UQWORD  mscnt = 0;

	gettimeofday(&tv, &tz);
	mscnt = (tv.tv_sec * CLOCKS_PER_MSEC) + (tv.tv_usec / CLOCKS_PER_MSEC);
	return mscnt;
}


//////////////////////////////////////////////////////////////////////////
///
///     ��װwrite
///     @param fd-�ļ�������
///     @param *ptr-�����׵�ַ
///     @param nbytes-���ݳ���
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
int  writen( int fd, void *ptr, int nbytes)
{
    int  nleft, nwriten ;

    nleft = nbytes ;
#if 0
{
	int i=0;
	printf("%s:\n",__FUNCTION__);
	for (i=0;i<nbytes;i++){
		printf("%02x ", ((char *)ptr)[i]);
		if ((i+1)%16 == 0 )
			printf("\n");
	}
	printf("\n");
}
#endif
while ( nleft > 0 )
    {
        nwriten = write( fd, ptr, nleft) ;

        if ( nwriten < 0 )
        {
			perror("system.c write");
            return( nwriten) ;
        }
		else if(nwriten == 0)
		{
			usleep(1000);
		}

        nleft = nleft - nwriten ;
        ptr += nwriten ;
    }

    return( nbytes - nleft) ;
}


#define TIORS485WR	0x5461		/*add by WANG -- for RS485 write*/

int  writen_rs485( int fd, void *ptr, int nbytes)
{
	unsigned long arg[2];
	int  nleft, nwriten ;

	nleft = nbytes ;
#if 0
{
	int i=0;
	printf("%s:\n",__FUNCTION__);
	for (i=0;i<nbytes;i++){
		printf("%02x ", ((char *)ptr)[i]);
		if ((i+1)%16 == 0 )
			printf("\n");
	}
	printf("\n");
}
#endif

	/*LOCK*/
	pthread_mutex_lock(&rs485_mutex);

	while ( nleft > 0 ){

		arg[0] = (unsigned long)ptr;
		arg[1] = nleft;
		nwriten = ioctl(fd, TIORS485WR, arg);		

		printf("XXX: n=%d,cnt= %d\n",nbytes,nwriten);

		if ( nwriten < 0 ){
			perror("system.c write");
			/*UNLOCK*/
			pthread_mutex_unlock(&rs485_mutex);
			return( nwriten) ;
		}else if(nwriten == 0){
			usleep(1000);
		}
		
		nleft = nleft - nwriten ;
		ptr += nwriten ;
	}
	/*UNLOCK*/
	pthread_mutex_unlock(&rs485_mutex);
	
	return( nbytes - nleft) ;
}



