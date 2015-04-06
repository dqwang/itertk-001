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
    return ret;
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

	if(net_conn_send(&conn, &sysinfo_quer_pak.head, NULL, 0) == FAILURE){
		net_conn_close(&conn);
		return SEND_FAILED;
	}
	if(net_conn_recv(&conn, &sysinfo_quer_pak.head, sizeof(ITSIP)) == FAILURE){
		net_conn_close(&conn);
		return FAILURE;
	}

	if(net_conn_recv(&conn, con_sys, sysinfo_quer_pak.head.itsip_extlen) == FAILURE){
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
	itsip_pack(ITS_NETINFO_SET, sizeof(CONFIG_NET), 0, NULL, &set_pak);//20141209

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


CMD_CODE its_conf_gpio_query(char* name, CONFIG_GPIO* con_gpio)
{
	ITSIP_PACKET query_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_CONF_QUERY, 0, 0, NULL, &query_pak);
	strcpy((char*)query_pak.head.itsip_user, name);
	query_pak.head.itsip_data[0] = CONF_GPIO;
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
	if(net_conn_recv(&conn, con_gpio, query_pak.head.itsip_extlen) == FAILURE)
    {
        net_conn_close(&conn);
        return RECV_FAILED;
    }
	net_conn_close(&conn);
	return QUERY_OK;

}

CMD_CODE its_conf_server_query(char* name, CONFIG_SERVER* con_server)
{
	ITSIP_PACKET server_quer_pak;
	NET_CONN_INFO conn;

	itsip_pack(ITS_SERVER_QUERY, 0, 0, NULL, &server_quer_pak);
	strcpy((char*)server_quer_pak.head.itsip_user, name);
	server_quer_pak.head.itsip_data[0] = CONF_SERVER;
	
	if(net_conn_connect(&conn) == FAILURE)
		return CONN_FAILED;

	if(net_conn_send(&conn, &server_quer_pak.head, NULL, 0) == FAILURE)
	{
		net_conn_close(&conn);
		return SEND_FAILED;
	}
	if(net_conn_recv(&conn, &server_quer_pak.head, sizeof(ITSIP)) == FAILURE)
	{
		net_conn_close(&conn);
		return FAILURE;
	}

	if(net_conn_recv(&conn, con_server, server_quer_pak.head.itsip_extlen) == FAILURE)
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

CMD_CODE its_conf_server_set(char* name, CONFIG_SERVER* con_server)
{
	ITSIP_PACKET set_pak;
	NET_CONN_INFO conn;
	itsip_pack(ITS_SERVER_SET, sizeof(CONFIG_SERVER), 0, NULL, &set_pak);

	if(net_conn_connect(&conn) == FAILURE)
		return CONN_FAILED;

	if(net_conn_send(&conn, &set_pak.head, (BYTE*)con_server, sizeof(CONFIG_SERVER)) == FAILURE)
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




#define MAX_SIZE_DNS 1024
#define SERVER_PORT_DNS 53

void setHead(unsigned char *buf)
{
	buf[0] = 0x00;
	buf[1] = 0;
	buf[2] = 0x01;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 1;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = buf[9] = buf[10] = buf[11] = 0;
}

void setQuery(char *name, unsigned char *buf, int len)
{
	int i;
	//strcat(buf+12,name);
	for(i=0;i<len;i++)
		buf[12+i] = name[i];
	int pos = len + 12;
	
	buf[pos] = 0;
	buf[pos+1] = 1;
	buf[pos+2] = 0;
	buf[pos+3] = 1;
}
int changeDN(char *DN,char *name)
{
	int i = strlen(DN) - 1;
	int j = i + 1;
	int k;
	
	name[j+1] = 0;
	for(k = 0; i >= 0; i--,j--) {
		if(DN[i] == '.') {
			name[j] = k;
			k = 0;
		}
		else {
			name[j] = DN[i];
			k++;
		}
	}
	name[0] = k;
	return (strlen(DN) + 2);
}
void printName(int len, char *name)
{
	  int i;
	  for(i = 0; i < len; i++) printf("%x.",name[i]);
	  printf("\n");
}

int sendDNSPacket(unsigned char *buf, int len, char *recvMsg, unsigned int dns_ip)
{
	int socket_fd;
	struct sockaddr_in sin;
	fd_set rfds;
	struct timeval tv;
	int retval;
	int ret=0;

	memset(&sin,0,sizeof(sin));
	//sin.sin_addr.s_addr = inet_addr("192.168.1.1");
	sin.sin_addr.s_addr = dns_ip;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SERVER_PORT_DNS);

	socket_fd = socket(PF_INET,SOCK_DGRAM,0);
	//sys_log(FUNC, LOG_WARN, " %s %x", "debug 2", s);
	sendto(socket_fd,buf,len,0,(struct sockaddr *)&sin,sizeof(sin));	
	FD_ZERO(&rfds);
	FD_SET(socket_fd, &rfds);
	//sys_log(FUNC, LOG_WARN, " %s %x", "debug",rfds);
	/* Wait up to 2 seconds. */
	tv.tv_sec = 2;
	tv.tv_usec = 0;

	retval = select(socket_fd+1, &rfds, NULL, NULL, &tv);
	/* Don't rely on the value of tv now! */

	if (retval == -1)
	perror("select()");
	else if (retval){		
		ret=recv(socket_fd,recvMsg,MAX_SIZE_DNS,0);			
	}		
	else{
		;//sys_log(FUNC, LOG_ERR, " %s", "dns failed");	
	}

	close(socket_fd);
	return ret;

}


int resolve(unsigned char *recvMsg, int len, int len_recvMsg, char *ip)
{
	int pos = len;
	int cnt = 12;

	while(pos < len_recvMsg) {		
		unsigned char now_pos = recvMsg[pos+1];
		unsigned char retype = recvMsg[pos+3];
		unsigned char reclass = recvMsg[pos+5];
		unsigned char offset = recvMsg[pos+11];

		if(retype == 1) {			
			if(now_pos == cnt && reclass == 1) {
				//printf("%d.%d.%d.%d\n",recvMsg[pos+12],recvMsg[pos+13],recvMsg[pos+14],recvMsg[pos+15]);

				sprintf(ip, "%d.%d.%d.%d", recvMsg[pos+12],recvMsg[pos+13],recvMsg[pos+14],recvMsg[pos+15]);

				return DNS_OK;
			}
		}
		else if(retype == 5) {			
			cnt = pos + 12 ;
		}		
		pos = pos + 12 + offset;
	}

	return -1;
}


CMD_CODE its_dns(char *www , unsigned int dns, char *ip)
{	
	unsigned char buf[MAX_SIZE_DNS]; /* socket发送的数据 */
	char *DN=www; /* 将要解析的域名(www.xxx.xxx) */
	char name[MAX_SIZE_DNS]; /* 转换为符合DNS报文格式的域名 */
	char recvMsg[MAX_SIZE_DNS]; /* 接收的数据 */
	int len; /* socket发送数据的长度 */
	int s; /* socket handler */
	int i,j;
	int len_recvMsg;
	int ret =-1;

	

	len = changeDN(DN,name);	
	setHead(buf);
	setQuery(name,buf,len);
	len += 16;
	len_recvMsg = sendDNSPacket(buf,len,recvMsg, dns);
	
	ret = resolve(recvMsg,len,len_recvMsg, ip);

	return ret;
}









