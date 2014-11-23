//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   def.h
/// @brief  类型重定义
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

#ifndef __DEF_H__
#define __DEF_H__

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

/*! standard C library */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>

/*! Linux system calls */
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/select.h>
#include <sys/vfs.h>
#include <sys/time.h>
#include <sys/reboot.h>
#include <sys/ioctl.h>
#include <sys/types.h>
//#include <sys/io.h>
#include <sys/sendfile.h>

/*! other interfaces */
#include <time.h>
#include <string.h>
#include <memory.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <termios.h>
#include <dirent.h>
#include <crypt.h>
#include <limits.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
//! type redefinition
typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef signed int              LONG;
typedef unsigned int            DWORD;
typedef unsigned int            PARAM;
typedef void *                  HANDLE;
typedef BYTE *                  STRING;
typedef signed int              BOOL;
typedef unsigned long long      UQWORD;
typedef long long               SQWORD;

#ifndef NULL
#define NULL 0
#endif

#ifndef CLOCKS_PER_MSEC
#define CLOCKS_PER_MSEC 	1000
#endif

#define FOR(i, max)         for (i = 0; i < (max); i++)

#define ON  1
#define OFF 0
#define NO  0
#define YES 1
#define SUCCESS 1
#define FAILURE 0

#endif
