//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   file.h
/// @brief  file头文件
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

#ifndef __FILE_H__
#define __FILE_H__
#include "def.h"

/*! \enum OPEN_FLAGS
 */
enum OPEN_FLAGS
{
    MODE_READ      =    0x00,	//!< only read in binary format
    MODE_WRITE,					//!< only write in binary format
    MODE_READWRITE,				//!< read and write dual mode
    MODE_CREATE,				//!< create a file
    MODE_APPEND,				//!< append new content to the existed file tail
    MODE_UPDATE,				//!< Open file for update the contents
    MODE_READ_T,				//!< read text file
    MODE_WRITE_T,				//!< write text file
    MODE_RW_T,                  //!< read and write text file, clean existed contents
    MODE_CREATE_T,				//!< create text file
    MODE_APPEND_T,				//!< append content to text file's tail
    MODE_UPDATE_T,              //!< Open text file for update the contents
};

//////////////////////////////////////////////////////////////////////////
///
///     判断文件是否存在
///     @param  file_name   文件名
///     @return 1－存在 0－不存在
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern int file_is_existed ( const    char *file_name );
//////////////////////////////////////////////////////////////////////////
///
///     打开文件句柄
///     @param  file_name   文件名
///     @param  flags   打开属性
///     @return 文件句柄
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern FILE *sys_file_open(char *file_name, DWORD flags);
//////////////////////////////////////////////////////////////////////////
///
///     将数据写入文件
///     @param  file_hd   文件
///     @param  *data   数据
///     @param  count   数据长度
///     @return SUCCESS FAILURE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern int sys_file_write(FILE *file_hd, void *data, DWORD count);
//////////////////////////////////////////////////////////////////////////
///
///     从文件读出数据
///     @param  file_hd   文件
///     @param  *data   数据
///     @param  count   数据长度
///     @return SUCCESS FAILURE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern int sys_file_read(FILE *file_hd, void *buf, DWORD count);
//////////////////////////////////////////////////////////////////////////
///
///     关闭文件句柄
///     @param  file_hd   文件
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
extern void sys_file_close( FILE *fp );

#endif


