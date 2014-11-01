//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   log.h
/// @brief  日志相关声明
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

#ifndef __LOG_H__
#define __LOG_H__

#define FUNC    (char*)__FUNCTION__

typedef enum tagLOG_Lv
{
    LOG_NONE,
    LOG_TRACE,    
    LOG_DBG,
    LOG_MSG,
    LOG_WARN,
    LOG_ERR,
    LOG_FATAL,
}LOG_Lv;



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
extern  int sys_log(char* module, LOG_Lv level, char* format, ...);

#endif
