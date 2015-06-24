//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   console.h
/// @brief  处理终端命令
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

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "def.h"
#include "config.h"
#include "cmd_def.h"


#define USR_SALT        "NC616"







//////////////////////////////////////////////////////////////////////////
///
///     user_login
///     @param *name
///     @param *psw
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE user_login(char *name, char* psw);
//////////////////////////////////////////////////////////////////////////
///
///     its_sysinfo_query
///     @param *name
///     @param *con_sys
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_sysinfo_query(char* name, CONFIG_SYS* con_sys);
//////////////////////////////////////////////////////////////////////////
///
///     its_sysinfo_set
///     @param *name
///     @param *con_sys
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_sysinfo_set(char* name, CONFIG_SYS* con_sys);
//////////////////////////////////////////////////////////////////////////
///
///     its_netinfo_query
///     @param *name
///     @param *con_net
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_netinfo_query(char* name, CONFIG_NET* con_net);
//////////////////////////////////////////////////////////////////////////
///
///     its_netinfo_set
///     @param *name
///     @param *con_sys
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_netinfo_set(char* name, CONFIG_NET* con_net);
//////////////////////////////////////////////////////////////////////////
///
///     its_vlan_query
///     @param *name
///     @param *con_vlan
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_vlan_query(char* name, CONFIG_VLAN* con_vlan);
//////////////////////////////////////////////////////////////////////////
///
///     its_vlan_set
///     @param *name
///     @param *con_sys
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_vlan_set(char* name, CONFIG_VLAN* con_vlan);//////////////////////////////////////////////////////////////////////////
///
///     its_factory_set
///     @param *name
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_factory_set(char* name);

//////////////////////////////////////////////////////////////////////////
///
///     its_user_query
///     @param *name
///     @param *con_usr
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_user_query(char* name, USR_INFO* con_usr);
//////////////////////////////////////////////////////////////////////////
///
///     its_user_add
///     @param *name
///     @param *con_usr
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_user_add(char* name, USR_INFO* con_usr);
//////////////////////////////////////////////////////////////////////////
///
///     its_user_del
///     @param *name
///     @param *con_usr
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_user_del(char* name, USR_INFO* con_usr);
//////////////////////////////////////////////////////////////////////////
///
///     its_user_modify
///     @param *name
///     @param *con_usr
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_user_modify(char* name, USR_INFO* con_usr);
//////////////////////////////////////////////////////////////////////////
///
///     its_conf_com_query
///     @param *name
///     @param *con_com
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_com_query(char* name, CONFIG_COM* con_com);
//////////////////////////////////////////////////////////////////////////
///
///     its_conf_mode_query
///     @param *name
///     @param *con_mode
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_mode_query(char* name, CONFIG_MODE* con_mode);
//////////////////////////////////////////////////////////////////////////
///
///     its_conf_lim_query
///     @param *name
///     @param *con_lim
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_lim_query(char* name, CONFIG_LIMIT* con_lim);
//////////////////////////////////////////////////////////////////////////
///
///     its_conf_com_set
///     @param *name
///     @param *con_com
///     @param flag 0-只应用到当前端口1-应用到所有端口
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_com_set(char* name, CONFIG_COM* con_com, BYTE flag);
//////////////////////////////////////////////////////////////////////////
///
///     its_conf_mode_set
///     @param *name
///     @param *con_mode
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_mode_set(char* name, CONFIG_MODE* con_mode);
//////////////////////////////////////////////////////////////////////////
///
///     its_conf_lim_set
///     @param *name
///     @param *con_lim
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_lim_set(char* name, CONFIG_LIMIT* con_lim);

CMD_CODE its_conf_server_query(char* name, CONFIG_SERVER* con_server);
CMD_CODE its_conf_server_set(char* name, CONFIG_SERVER* con_server);

CMD_CODE its_dns(char *www , unsigned int dns, char *ip);

CMD_CODE its_conf_gpio_query(char* name, CONFIG_GPIO* con_gpio);
CMD_CODE its_conf_gpio_set(char* name, CONFIG_GPIO* con_gpio);


#endif

