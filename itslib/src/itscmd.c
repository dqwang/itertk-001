//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   itscmd.c
/// @brief  处理终端命令
///
///
///
/// @version    2.0
/// @author     xuliang<gxuliang@gmail.com>
/// @date       2010-08-14
///
///
///     修订说明：最初版本
//////////////////////////////////////////////////////////////////////////


#include "cmd_def.h"
#include "itscmd.h"
#include "itsip.h"
#include "net.h"
#include "config.h"






//////////////////////////////////////////////////////////////////////////
///
///     user_login
///     @param *name
///     @param *psw
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE user_login(char *name, char* psw)
{
    ITSIP_PACKET user_login_pak;
    ITSIP user_login_ack;
    NET_CONN_INFO conn;

    itsip_pack(ITS_LOGIN, 0, 0, NULL, &user_login_pak);
    strcpy((char*)user_login_pak.head.itsip_user, name);
    strcpy((char*)user_login_pak.head.itsip_data, crypt(psw, USR_SALT));

    if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

    if(net_conn_send(&conn, &user_login_pak.head, NULL, 0) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }
    if(net_conn_recv(&conn, &user_login_ack, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
    net_conn_close(&conn);
    int ret = user_login_ack.itsip_data[0];
    return ret;;
}



//////////////////////////////////////////////////////////////////////////
///
///     its_sysinfo_query
///     @param *name
///     @param *con_sys
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_sysinfo_query(char* name, CONFIG_SYS* con_sys)
{
    ITSIP_PACKET sysinfo_quer_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_SYSINFO_QUERY, 0, 0, NULL, &sysinfo_quer_pak);
	strcpy((char*)sysinfo_quer_pak.head.itsip_user, name);
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

    if(net_conn_send(&conn, &sysinfo_quer_pak.head, NULL, 0) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }
	if(net_conn_recv(&conn, &sysinfo_quer_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return FAILURE;
    }

	if(net_conn_recv(&conn, con_sys, sysinfo_quer_pak.head.itsip_extlen) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }


    net_conn_close(&conn);

    return QUERY_OK;
}

//////////////////////////////////////////////////////////////////////////
///
///     its_sysinfo_set
///     @param *name
///     @param *con_sys
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_sysinfo_set(char* name, CONFIG_SYS* con_sys)
{
	ITSIP_PACKET set_pak;
	NET_CONN_INFO conn;
	itsip_pack(ITS_SYSINFO_SET, sizeof(CONFIG_SYS), 0, NULL, &set_pak);

	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &set_pak.head, (BYTE*)con_sys, sizeof(CONFIG_SYS)) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &set_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return set_pak.head.itsip_data[0];
}
//////////////////////////////////////////////////////////////////////////
///
///     its_netinfo_query
///     @param *name
///     @param *con_net
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_netinfo_query(char* name, CONFIG_NET* con_net)
{
	ITSIP_PACKET netinfo_quer_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_NETINFO_QUERY, 0, 0, NULL, &netinfo_quer_pak);
	strcpy((char*)netinfo_quer_pak.head.itsip_user, name);
	if(net_conn_connect(&conn) == FAILURE)
		return CONN_FAILED;

	if(net_conn_send(&conn, &netinfo_quer_pak.head, NULL, 0) == FAILURE)  {
	net_conn_close(&conn);
	return SEND_FAILED;
	}
	if(net_conn_recv(&conn, &netinfo_quer_pak.head, sizeof(ITSIP)) == FAILURE){
		net_conn_close(&conn);
		return FAILURE;
	}

	if(net_conn_recv(&conn, con_net, netinfo_quer_pak.head.itsip_extlen) == FAILURE){
		net_conn_close(&conn);
		return RECV_FAILED;
	}


	net_conn_close(&conn);

	return QUERY_OK;
}

//////////////////////////////////////////////////////////////////////////
///
///     its_netinfo_set
///     @param *name
///     @param *con_sys
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_netinfo_set(char* name, CONFIG_NET* con_net)
{
	ITSIP_PACKET set_pak;
	NET_CONN_INFO conn;
	itsip_pack(ITS_NETINFO_SET, sizeof(CONFIG_SYS), 0, NULL, &set_pak);

	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &set_pak.head, (BYTE*)con_net, sizeof(CONFIG_NET)) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &set_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return set_pak.head.itsip_data[0];
}

//////////////////////////////////////////////////////////////////////////
///
///     its_vlan_query
///     @param *name
///     @param *con_vlan
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_vlan_query(char* name, CONFIG_VLAN* con_vlan)
{
    ITSIP_PACKET netinfo_quer_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_VLAN_QUERY, 0, 0, NULL, &netinfo_quer_pak);
	strcpy((char*)netinfo_quer_pak.head.itsip_user, name);
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

    if(net_conn_send(&conn, &netinfo_quer_pak.head, NULL, 0) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }
	if(net_conn_recv(&conn, &netinfo_quer_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return FAILURE;
    }

	if(net_conn_recv(&conn, con_vlan, netinfo_quer_pak.head.itsip_extlen) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }


    net_conn_close(&conn);

    return QUERY_OK;
}
//////////////////////////////////////////////////////////////////////////
///
///     its_vlan_set
///     @param *name
///     @param *con_sys
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_vlan_set(char* name, CONFIG_VLAN* con_vlan)
{
	ITSIP_PACKET set_pak;
	NET_CONN_INFO conn;
	itsip_pack(ITS_VLAN_SET, sizeof(CONFIG_VLAN), 0, NULL, &set_pak);

	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &set_pak.head, (BYTE*)con_vlan, sizeof(CONFIG_VLAN)) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &set_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return set_pak.head.itsip_data[0];
}

//////////////////////////////////////////////////////////////////////////
///
///     its_factory_set
///     @param *name
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-22
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_factory_set(char* name)
{
	ITSIP_PACKET set_pak;
	NET_CONN_INFO conn;
	itsip_pack(ITS_FACTORY_SET, 0, 0, NULL, &set_pak);

	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &set_pak.head, NULL, 0) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &set_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return set_pak.head.itsip_data[0];
}

//////////////////////////////////////////////////////////////////////////
///
///     its_user_query
///     @param *name
///     @param *con_usr
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_user_query(char* name, USR_INFO* con_usr)
{
    ITSIP_PACKET query_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_USER_QUERY, 0, 0, NULL, &query_pak);
	strcpy((char*)query_pak.head.itsip_user, name);
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &query_pak.head, NULL, 0) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &query_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }

	if(net_conn_recv(&conn, con_usr, query_pak.head.itsip_extlen) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }


    net_conn_close(&conn);

    return QUERY_OK;
}
//////////////////////////////////////////////////////////////////////////
///
///     its_user_add
///     @param *name
///     @param *con_usr
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_user_add(char* name, USR_INFO* con_usr)
{
    ITSIP_PACKET query_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_USER_SET, sizeof(USR_INFO), 0, NULL, &query_pak);
	strcpy((char*)query_pak.head.itsip_user, name);
	query_pak.head.itsip_data[0] = USER_ADD;
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &query_pak.head, (BYTE*)con_usr, sizeof(USR_INFO)) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &query_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }

	return query_pak.head.itsip_data[0];
}
//////////////////////////////////////////////////////////////////////////
///
///     its_user_del
///     @param *name
///     @param *con_usr
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_user_del(char* name, USR_INFO* con_usr)
{
    ITSIP_PACKET query_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_USER_SET, sizeof(USR_INFO), 0, NULL, &query_pak);
	strcpy((char*)query_pak.head.itsip_user, name);
	query_pak.head.itsip_data[0] = USER_DEL;
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &query_pak.head, (BYTE*)con_usr, sizeof(USR_INFO)) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &query_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }

	return query_pak.head.itsip_data[0];
}

//////////////////////////////////////////////////////////////////////////
///
///     its_user_modify
///     @param *name
///     @param *con_usr
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_user_modify(char* name, USR_INFO* con_usr)
{
    ITSIP_PACKET query_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_USER_SET, sizeof(USR_INFO)*2, 0, NULL, &query_pak);
	strcpy((char*)query_pak.head.itsip_user, name);
	query_pak.head.itsip_data[0] = USER_MODIFY;
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &query_pak.head, (BYTE*)con_usr, sizeof(USR_INFO)*2) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &query_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }

	return query_pak.head.itsip_data[0];
}
//////////////////////////////////////////////////////////////////////////
///
///     its_conf_com_query
///     @param *name
///     @param *con_com
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_com_query(char* name, CONFIG_COM* con_com)
{
	ITSIP_PACKET query_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_CONF_QUERY, 0, 0, NULL, &query_pak);
	strcpy((char*)query_pak.head.itsip_user, name);
	query_pak.head.itsip_data[0] = CONF_COM;
	query_pak.head.itsip_data[1] = con_com->id - 1;
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &query_pak.head, NULL, 0) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &query_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	if(net_conn_recv(&conn, con_com, query_pak.head.itsip_extlen) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return QUERY_OK;
}

//////////////////////////////////////////////////////////////////////////
///
///     its_conf_mode_query
///     @param *name
///     @param *con_mode
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_mode_query(char* name, CONFIG_MODE* con_mode)
{
	ITSIP_PACKET query_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_CONF_QUERY, 0, 0, NULL, &query_pak);
	strcpy((char*)query_pak.head.itsip_user, name);
	query_pak.head.itsip_data[0] = CONF_MODE;
	query_pak.head.itsip_data[1] = con_mode->id - 1;
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &query_pak.head, NULL, 0) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &query_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	if(net_conn_recv(&conn, con_mode, query_pak.head.itsip_extlen) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }

	net_conn_close(&conn);
	return QUERY_OK;
}

//////////////////////////////////////////////////////////////////////////
///
///     its_conf_lim_query
///     @param *name
///     @param *con_lim
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_lim_query(char* name, CONFIG_LIMIT* con_lim)
{
	ITSIP_PACKET query_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_CONF_QUERY, 0, 0, NULL, &query_pak);
	strcpy((char*)query_pak.head.itsip_user, name);
	query_pak.head.itsip_data[0] = CONF_LIMIT;
	query_pak.head.itsip_data[1] = con_lim->id - 1;
	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &query_pak.head, NULL, 0) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &query_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	if(net_conn_recv(&conn, con_lim, query_pak.head.itsip_extlen) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return QUERY_OK;
}

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
CMD_CODE its_conf_com_set(char* name, CONFIG_COM* con_com, BYTE flag)
{
	ITSIP_PACKET set_pak;
	NET_CONN_INFO conn;
	itsip_pack(ITS_CONF_SET, sizeof(CONFIG_COM), 0, NULL, &set_pak);
	set_pak.head.itsip_data[0] = CONF_COM;
	set_pak.head.itsip_data[1] = con_com->id - 1;
    set_pak.head.itsip_data[2] = flag;

	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &set_pak.head, (BYTE*)con_com, sizeof(CONFIG_COM)) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &set_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return set_pak.head.itsip_data[0];
}

//////////////////////////////////////////////////////////////////////////
///
///     its_conf_mode_set
///     @param *name
///     @param *con_mode
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_mode_set(char* name, CONFIG_MODE* con_mode)
{
	ITSIP_PACKET set_pak;
	NET_CONN_INFO conn;
	itsip_pack(ITS_CONF_SET, sizeof(CONFIG_MODE), 0, NULL, &set_pak);
	set_pak.head.itsip_data[0] = CONF_MODE;
	set_pak.head.itsip_data[1] = con_mode->id - 1;

	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &set_pak.head, (BYTE*)con_mode, sizeof(CONFIG_MODE)) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &set_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return set_pak.head.itsip_data[0];
}
//////////////////////////////////////////////////////////////////////////
///
///     its_conf_lim_set
///     @param *name
///     @param *con_lim
///     @return CMD_CODE
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-08-14
//////////////////////////////////////////////////////////////////////////
CMD_CODE its_conf_lim_set(char* name, CONFIG_LIMIT* con_lim)
{
	ITSIP_PACKET set_pak;
	NET_CONN_INFO conn;
	itsip_pack(ITS_CONF_SET, sizeof(CONFIG_LIMIT), 0, NULL, &set_pak);
	set_pak.head.itsip_data[0] = CONF_LIMIT;
	set_pak.head.itsip_data[1] = con_lim->id - 1;

	if(net_conn_connect(&conn) == FAILURE)
        return CONN_FAILED;

	if(net_conn_send(&conn, &set_pak.head, (BYTE*)con_lim, sizeof(CONFIG_LIMIT)) == FAILURE)
    {
        net_conn_close(&conn);
        return SEND_FAILED;
    }

	if(net_conn_recv(&conn, &set_pak.head, sizeof(ITSIP)) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return set_pak.head.itsip_data[0];
}











