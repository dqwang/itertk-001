//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   itsip.c
/// @brief  协议库函数定义
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

#include <string.h>
#include "itsip.h"

//////////////////////////////////////////////////////////////////////////
///
///     填充协议内容
///     @param  cmd   命令字
///     @param  extlen  扩展长度
///     @param  dev_t   设备类型
///     @param  *data   扩展内容
///     @param  *its_pkt   协议结构体
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
inline void itsip_pack(BYTE cmd, WORD extlen, BYTE dev_t, void *data, ITSIP_PACKET *its_pkt)
{
	its_pkt->head.itsip_head = ITS_HEAD;
	its_pkt->head.itsip_cmd = cmd;
	its_pkt->head.itsip_ver = ITSIP_VERSION;
	its_pkt->head.itsip_thl = ITSIP_THLEN;
	its_pkt->head.itsip_extlen = extlen;

	its_pkt->head.itsip_dev_t = dev_t;

	if (extlen > 0 && data != NULL)
		memcpy(its_pkt->data, data, extlen);
	
}

