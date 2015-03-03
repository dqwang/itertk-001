/*
*
*/
#include <string.h>
#include "web_net.h"
#include "log.h"
#include "itsip.h"
#include "config.h"
#include "cmd_def.h"
#include "gpio.h"
#include "client.h"

NET_CONN_INFO g_client_conn[MAX_TCP_CONN];
static int g_listen_conn = -1;	//! server listening socket
static int m_max_sock = -1;		//! max socket handle

extern int g_reconnect_flag;


int net_conn_send(NET_CONN_INFO *conn_info, ITSIP *net_head, BYTE* net_data, DWORD len);

static void web_svr_start(void)
{
	    int flag = 0;
	    int ret = -1;
	    struct sockaddr_in server_addr;
	    int sndlen;

	g_listen_conn = socket(AF_INET, SOCK_STREAM, 0);
	sys_log(FUNC, LOG_DBG, "[sock_fd %d]socket\n",g_listen_conn);
	if (g_listen_conn == -1){
		perror("web_svr_start: socket");
		return;
	}

	// Enable address reuse and keeplive
	flag = ON;
	ret = setsockopt(g_listen_conn, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	if (ret == -1){
		perror("net_server_start: setsockopt()");
		return;
	}

	//	Enable keeplive
	ret = setsockopt(g_listen_conn, SOL_SOCKET, SO_KEEPALIVE, &flag, sizeof(flag));

	if (ret == -1){
		perror("net_server_start: setsockopt()");
		return;
	}

	sndlen = 16 * 1024;
	ret = setsockopt(g_listen_conn, SOL_SOCKET, SO_SNDBUF, &sndlen, sizeof(int));

	if (ret == -1){
		perror("net_server_start: setsockopt()");
		return;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(WEB_PORT);
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	while (bind(g_listen_conn, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)  {
		perror("net_server_start: bind");
		//close(g_listen_conn);

		//return;
		sleep(3);
	}
	sys_log(FUNC, LOG_DBG, "[sock_fd %d]bind...\n",g_listen_conn);
	
	if (listen(g_listen_conn, MAX_TCP_CONN) == -1){
		perror("web_svr_start: listen");
		close(g_listen_conn);

		return;
	}
	sys_log(FUNC, LOG_DBG, "[sock_fd %d]listen...\n",g_listen_conn);
	m_max_sock = g_listen_conn;
	
	sys_log(FUNC, LOG_DBG, "APP Server start ...WEB_PORT :%d\n", WEB_PORT);
}

void web_init(void)
{
	int i = -1;

	FOR (i , MAX_TCP_CONN){
		memset(&g_client_conn[i], 0, sizeof(NET_CONN_INFO));
		g_client_conn[i].conn_idx = -1;
		g_client_conn[i].client_conn = -1;
		g_client_conn[i].user = NULL;
		g_client_conn[i].idle = 0;
		g_client_conn[i].file_type = 0xff;
		trd_lock_init(&g_client_conn[i].lock);
	}

	web_svr_start();
}

void net_conn_close(NET_CONN_INFO *conn_info)
{
    
	if (conn_info->client_conn > 0){
		close(conn_info->client_conn);
		sys_log(FUNC, LOG_WARN, "[sock_fd %d]close\n", conn_info->client_conn);
		conn_info->client_conn = -1;
		conn_info->file_type = 0xff;
	}
}

static int web_conn_recv(NET_CONN_INFO *conn_info, void *net_data, DWORD len)
{
	BYTE *p_net_data = NULL;
	int recv_cnt = 0;
	int total_length = 0;
	fd_set readfds;
	int res = -1;
	struct timeval tv;
	UQWORD ms_cnt1 = 0LLU;
	UQWORD ms_cnt2 = 0LLU;
	UQWORD ms_cnt3 = 0LLU;

	if (conn_info->client_conn <= 0)	
		return FAILURE;	

	p_net_data = net_data;
	total_length = len;

	ms_cnt1 = system_mscount_get();

	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(conn_info->client_conn, &readfds);
		tv.tv_sec = 0;
		tv.tv_usec = 1000;

		res =-1;
		sys_log(FUNC,  LOG_DBG,  "selecting\n");
		res = select(conn_info->client_conn + 1, &readfds, NULL, NULL, &tv);
		sys_log(FUNC, LOG_DBG, "[sock_fd %d]select %d\n", conn_info->client_conn, res);
		if (res < 0){
		
			if (errno == EINTR)
			{
				continue;/*for signal interrupt, do not close the socket, please continue.*/
			}
			perror("web_conn_recv:select");
			net_conn_close(conn_info);
			return FAILURE;
		}
		else if (res > 0)
		{
			if (FD_ISSET(conn_info->client_conn, &readfds)){
				sys_log(FUNC, LOG_DBG, "[sock_fd %d]recving\n", conn_info->client_conn);
				recv_cnt  = recv(conn_info->client_conn, p_net_data, total_length, MSG_DONTWAIT | MSG_NOSIGNAL);
				sys_log(FUNC, LOG_DBG, "[sock_fd %d]recv %d bytes\n", conn_info->client_conn, recv_cnt);
				ms_cnt1 = system_mscount_get();
			}
		}
		else
		{
			ms_cnt2 = system_mscount_get();

			//If the return value of select() is still Zero after 500ms, Close the socket
			//if ((ms_cnt2 > ms_cnt1) && ((ms_cnt2 - ms_cnt1 )>= TIMEOUT_CNT)){

			if ((ms_cnt2 > ms_cnt1) && ((ms_cnt2 - ms_cnt1 )>= TIMEOUT_CNT)){
				net_conn_close(conn_info);
				sys_log(FUNC, LOG_ERR, "web_conn_recv: Client no response in %llu ms, we must Close this socket.\n", ms_cnt2 - ms_cnt1);
				return FAILURE;
			}

			continue;
		}

		if (recv_cnt == -1)
		{
			perror("web_conn_recv:recv");

			if (errno == ECONNRESET)
			{
				net_conn_close(conn_info);
			}

			return FAILURE;
		}

		if (recv_cnt == 0)
		{
			net_conn_close(conn_info);

			return FAILURE;
		}

		if (total_length - recv_cnt == 0)
		{
			return SUCCESS;
		}
		else
		{
			ms_cnt3 = system_mscount_get();

			//If the return value of select() is still Zero after 500ms, Close the socket
			if (ms_cnt3 > ms_cnt1 && ms_cnt3 - ms_cnt1 >= TIMEOUT_CNT)
			{
				//We must close this socket and return FAILURE when the client is in problem.
				//Otherwise system will stop to capture picture and video
				net_conn_close(conn_info);
				sys_log(FUNC, LOG_ERR, "net_conn_recv: Client has something wrong, we must Close this socket.\n");
				return FAILURE;
			}

			p_net_data += recv_cnt;
			total_length -= recv_cnt;
		}
	}
}

int sys_usr_login(char *name, char* psw)
{
	int i;

	sys_log(FUNC, LOG_MSG, "name %s, passwd %d", name, psw);

	FOR(i, MAX_USR_NUM)
	{
		if(strcmp(name, (char*)g_conf_info.con_usr[i].usr_name) == 0)
		{
			if(strcmp(psw, (char*)g_conf_info.con_usr[i].usr_passwd) == 0)
				return LOGIN_OK;
			else
				return ERROR_PSW;
		}        
	}
	return NO_USER;
}

void sys_usr_query(NET_CONN_INFO *conn_info, BYTE *usr_name)
{
    USR_INFO* usr_info = g_conf_info.con_usr;
    ITSIP_PACKET its_ack_pak;

    itsip_pack(ITS_ACK_USER_QUERY, sizeof(g_conf_info.con_usr), 0, NULL, &its_ack_pak);
    net_conn_send(conn_info, (ITSIP *)&its_ack_pak.head, (BYTE *)usr_info, sizeof(g_conf_info.con_usr));
}

extern int sys_usr_del(BYTE* name);

int sys_usr_set(NET_CONN_INFO *conn_info, ITSIP *p_net_head)
{
    USR_INFO* usr_info;
    BYTE set_flag = p_net_head->itsip_data[0];
    BYTE *usr_name = p_net_head->itsip_user;
    USR_INFO extdat[2];
//    BYTE *nusr = &extdat[0], *npsw = &extdat[USR_NAME_LEN];
//    BYTE *nnusr = &extdat[USR_NAME_LEN*2], *nnpsw = &extdat[USR_NAME_LEN*3];
    
    sys_log(FUNC, LOG_DBG, "set flag [%d]!", set_flag);
    usr_info = get_usr_info(usr_name);
    if(usr_info == NULL)
        return RC_NOUSR;
    if(usr_info->usr_auth != USR_ROOT)
        return RC_NORIGHT;
    
    web_conn_recv(conn_info, extdat, p_net_head->itsip_extlen);
    switch(set_flag)
    {
    case USER_ADD:
        if(get_usr_info(extdat[0].usr_name) == NULL)
            return sys_usr_add(extdat[0].usr_name, extdat[0].usr_passwd);
        else
            return RC_USREXIST;
        break;
    case USER_DEL:
        if(get_usr_info(extdat[0].usr_name) == NULL)
            return RC_NOUSR;
        if((strcmp((char*)extdat[0].usr_name, "root") == 0))
            return USR_DEL_FAILED;
        return sys_usr_del((BYTE *)(extdat[0].usr_name));
        break;
    case USER_MODIFY:
        if(extdat[1].usr_name[0] != '\0')
        {//如果没有新名字，则不改名字
            if(set_usr_name(extdat[0].usr_name, extdat[1].usr_name) == FAILURE)
                return USR_MODIFY_FAILED;
            if(set_usr_psw(extdat[1].usr_name, extdat[1].usr_passwd) == FAILURE)
                return USR_MODIFY_FAILED;
            return USR_MODIFY_OK;
        }
        else
        {
            if(extdat[1].usr_passwd != '\0')
            {
                if(set_usr_psw(extdat[0].usr_name, extdat[1].usr_passwd) == FAILURE)
                    return USR_MODIFY_FAILED;
            }
            else
            {
                return USR_MODIFY_FAILED;
            }
            return USR_MODIFY_OK;
        }
        break;
    default:
        break;
    }

	return RC_NOUSR;//WANG
}

BYTE get_usr_id(BYTE* name)
{
    int i;
    FOR(i, MAX_USR_NUM)
    {
        if(strcmp((char*)g_conf_info.con_usr[i].usr_name, (char*)name) == 0)
            return i;
    }
    return i;
}


void sys_conf_query(NET_CONN_INFO *conn_info, ITSIP *p_net_head)
{
    ITSIP_PACKET its_ack_pak;
    BYTE id;
    
    switch(p_net_head->itsip_data[0])
    {
    case CONF_COM:
        if(g_conf_info.con_lim[p_net_head->itsip_data[1]].enable == DISABLED ||\
            (g_conf_info.con_lim[p_net_head->itsip_data[1]].enable == ENABLED && \
            (id = get_usr_id(p_net_head->itsip_user)) < MAX_USR_NUM &&\
            g_conf_info.con_lim[p_net_head->itsip_data[1]].read[id] == ENABLED))
        {
            itsip_pack(ITS_ACK_CONF_QUERY, sizeof(CONFIG_COM), 0, NULL, &its_ack_pak);
            its_ack_pak.head.itsip_data[0] = 0;
            net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&g_conf_info.con_com[p_net_head->itsip_data[1]], sizeof(CONFIG_COM));
        }
        else
        {
            itsip_pack(ITS_ACK_CONF_QUERY, 0, 0, NULL, &its_ack_pak);
            its_ack_pak.head.itsip_data[0] = 1;
            net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
        }
        break;
    case CONF_MODE:
        sys_log(FUNC, LOG_MSG, "CONF_MODE, id = %d", p_net_head->itsip_data[1]);
        if(g_conf_info.con_lim[p_net_head->itsip_data[1]].enable == DISABLED ||\
            (g_conf_info.con_lim[p_net_head->itsip_data[1]].enable == ENABLED && \
            (id = get_usr_id(p_net_head->itsip_user)) < MAX_USR_NUM &&\
            g_conf_info.con_lim[p_net_head->itsip_data[1]].read[id] == ENABLED))
        {
            itsip_pack(ITS_ACK_CONF_QUERY, sizeof(CONFIG_MODE), 0, NULL, &its_ack_pak);
            its_ack_pak.head.itsip_data[0] = 0;
            net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&g_conf_info.con_mode[p_net_head->itsip_data[1]], sizeof(CONFIG_MODE));
        }
        else
        {
            itsip_pack(ITS_ACK_CONF_QUERY, 0, 0, NULL, &its_ack_pak);
            its_ack_pak.head.itsip_data[0] = 1;
            net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
        }
        break;
    case CONF_LIMIT:
        id = get_usr_id(p_net_head->itsip_user);
        if(g_conf_info.con_usr[id].usr_auth == USR_ROOT)
        {
            itsip_pack(ITS_ACK_CONF_QUERY, sizeof(CONFIG_LIMIT), 0, NULL, &its_ack_pak);
            net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&g_conf_info.con_lim[p_net_head->itsip_data[1]], sizeof(CONFIG_LIMIT));
        }
        else
        {
            itsip_pack(ITS_ACK_CONF_QUERY, 0, 0, NULL, &its_ack_pak);
            its_ack_pak.head.itsip_data[0] = 1;
            net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
        }
        break;
  case CONF_GPIO:
	{
	  	CONFIG_GPIO mygpio;
		gpio_status alarm;
		get_gpio(ALARM_IN1,&alarm);
		mygpio.alarm[0] = alarm;
		get_gpio(ALARM_IN2,&alarm);
		mygpio.alarm[1] = alarm;		
		get_gpio(ALARM_IN3,&alarm);
		mygpio.alarm[2] = alarm;
		get_gpio(ALARM_IN4,&alarm);
		mygpio.alarm[3] = alarm;
		get_gpio(ALARM_IN5,&alarm);
		mygpio.alarm[4] = alarm;
		get_gpio(ALARM_IN6,&alarm);
		mygpio.alarm[5] = alarm;
		get_gpio(ALARM_IN7,&alarm);
		mygpio.alarm[6] = alarm;
		get_gpio(ALARM_IN8,&alarm);
		mygpio.alarm[7] = alarm;
		
		itsip_pack(ITS_ACK_CONF_QUERY, sizeof(CONFIG_GPIO), 0, NULL, &its_ack_pak);
		
		its_ack_pak.head.itsip_data[0] = 0;
	            net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&mygpio, sizeof(CONFIG_GPIO));
	
  	}
	break;

	case CONF_SERVER:{
		CONFIG_SERVER config_server;
		itsip_pack(ITS_ACK_CONF_QUERY, sizeof(CONFIG_SERVER), 0, NULL, &its_ack_pak);

		its_ack_pak.head.itsip_data[0] = 0;
		net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&config_server, sizeof(CONFIG_SERVER));
		
	}break;


    default:
        break;
    }
}

int sys_sysinfo_set(NET_CONN_INFO *conn_info, ITSIP *p_net_head)
{
    CONFIG_SYS conf_sys;
    if(web_conn_recv(conn_info, &conf_sys, sizeof(CONFIG_SYS)) == FAILURE)
        return SYS_SET_FAILED;
    memcpy(&g_conf_info.con_sys, &conf_sys, sizeof(CONFIG_SYS));
    config_save(&g_conf_info);
    return SYS_SET_OK;
}

int sys_server_set(NET_CONN_INFO *conn_info, ITSIP *p_net_head)
{
	CONFIG_SERVER conf_server;
	if(web_conn_recv(conn_info, &conf_server, sizeof(CONFIG_SERVER)) == FAILURE)
		return SERVER_SET_FAILED;
	memcpy(&g_conf_info.con_server, &conf_server, sizeof(CONFIG_SERVER));
	sys_log(FUNC, LOG_MSG, "server  updated  ip: %s, port:%d", sys_ip2str_static(g_conf_info.con_server.server_ip), g_conf_info.con_server.server_port);
	
	config_save(&g_conf_info);
	return SERVER_SET_OK;
}


extern void config_net_set(CONFIG_NET *pConf);

int sys_netinfo_set(NET_CONN_INFO *conn_info, ITSIP *p_net_head)
{
	CONFIG_NET conf_net;
	if(web_conn_recv(conn_info, &conf_net, sizeof(CONFIG_NET)) == FAILURE)
		return NET_SET_FAILED;
	memcpy(&g_conf_info.con_net, &conf_net, sizeof(CONFIG_NET));
	config_save(&g_conf_info);
	config_net_set(&g_conf_info.con_net);

	
	return NET_SET_OK;
}

int sys_vlan_set(NET_CONN_INFO *conn_info, ITSIP *p_net_head)
{
    CONFIG_VLAN conf_vlan;
    if(web_conn_recv(conn_info, &conf_vlan, sizeof(CONFIG_VLAN)) == FAILURE)
        return VLAN_SET_FAILED;
    memcpy(&g_conf_info.con_vlan, &conf_vlan, sizeof(CONFIG_VLAN));
    config_save(&g_conf_info);
    return VLAN_SET_OK;
}

int sys_factory_set(NET_CONN_INFO *conn_info, ITSIP *p_net_head)
{
	char strcmd[64]="";
	sprintf(strcmd, "rm %s", CONFIG_FILE);
	sys_log(FUNC, LOG_MSG, "[%s]\n", strcmd);
	system(strcmd);
	config_init();
	return FACTORY_SET_OK;
}

extern void com_set(CONFIG_COM *con_com);

int sys_conf_set(NET_CONN_INFO *conn_info, ITSIP *p_net_head)
{
    CONFIG_COM conf_com;
    CONFIG_MODE conf_mode;
    CONFIG_LIMIT conf_lim;
    BYTE id;
    int i;
    switch(p_net_head->itsip_data[0])
    {
        case CONF_COM:
            if(web_conn_recv(conn_info, &conf_com, sizeof(CONFIG_COM)) == FAILURE)
                return RECV_FAILED;
            if(g_conf_info.con_lim[p_net_head->itsip_data[1]].enable == DISABLED ||\
                (g_conf_info.con_lim[p_net_head->itsip_data[1]].enable == ENABLED && \
                (id = get_usr_id(p_net_head->itsip_user)) < MAX_USR_NUM &&\
                g_conf_info.con_lim[p_net_head->itsip_data[1]].modify[id] == ENABLED))
            {
            	  if(conf_com.all != 1)
                	memcpy(&g_conf_info.con_com[p_net_head->itsip_data[1]], &conf_com, sizeof(CONFIG_COM));
                else
                {
			for(i = 0; i < MAX_COM_PORT; i++)
			{
				g_conf_info.con_com[i].bps = conf_com.bps;
				g_conf_info.con_com[i].dbit = conf_com.dbit;
				g_conf_info.con_com[i].chk= conf_com.chk;
				g_conf_info.con_com[i].sbit = conf_com.sbit;
				g_conf_info.con_com[i].type = conf_com.type;
				g_conf_info.con_com[i].isloop = conf_com.isloop;

			}
                }
                config_save(&g_conf_info);
                com_set(&g_conf_info.con_com[p_net_head->itsip_data[1]]);
                return COM_SET_OK;
            }
            else
                return COM_SET_OK;
            break;
        case CONF_MODE:
            if(web_conn_recv(conn_info, &conf_mode, sizeof(CONFIG_MODE)) == FAILURE)
                return RECV_FAILED;
            if(g_conf_info.con_lim[p_net_head->itsip_data[1]].enable == DISABLED ||\
                (g_conf_info.con_lim[p_net_head->itsip_data[1]].enable == ENABLED && \
                (id = get_usr_id(p_net_head->itsip_user)) < MAX_USR_NUM &&\
                g_conf_info.con_lim[p_net_head->itsip_data[1]].modify[id] == ENABLED))
            {
                memcpy(&g_conf_info.con_mode[p_net_head->itsip_data[1]], &conf_mode, sizeof(CONFIG_MODE));
                config_save(&g_conf_info);
                return MODE_SET_OK;
            }
            else
                return MODE_SET_FAILED;
            break;
        case CONF_LIMIT:
            if(web_conn_recv(conn_info, &conf_lim, sizeof(CONFIG_LIMIT)) == FAILURE)
                return RECV_FAILED;
            id = get_usr_id(p_net_head->itsip_user);
            if(g_conf_info.con_usr[id].usr_auth == USR_ROOT)
            {
                memcpy(&g_conf_info.con_lim[p_net_head->itsip_data[1]], &conf_lim, sizeof(CONFIG_LIMIT));
                config_save(&g_conf_info);
                return LIMIT_SET_OK;
            }
            else
                return LIMIT_SET_FAILED;
            break;
        default:
            break;
    }

	return COM_SET_FAILED;//WANG
}
int net_conn_send_pkt(NET_CONN_INFO *conn_info, void *net_data, int len)
{
	BYTE *ptr = NULL;
	int send_cnt = 0;
	int total_length = 0;
	fd_set writefds;
	struct timeval tv;
	int res = -1;
	UQWORD ms_cnt1 = 0LLU;
	UQWORD ms_cnt2 = 0LLU;
	UQWORD ms_cnt3 = 0LLU;

	if (conn_info->client_conn <= 0)
	{
		return FAILURE;

	}

	conn_info->idle = 0;
	ms_cnt1 = system_mscount_get();

	ptr = (BYTE*)net_data;
	total_length = len;

	while (1)
	{
		FD_ZERO(&writefds);
		FD_SET(conn_info->client_conn, &writefds);
		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		res =-1;
		
		sys_log(FUNC,  LOG_DBG,  "selecting\n");
		res = select(conn_info->client_conn + 1, NULL, &writefds, NULL, &tv);
		sys_log(FUNC,  LOG_DBG,  "select %d\n", res);
		if (res < 0)
		{
			/*
			* note by xuw 2007/03/02: for signal interrupt, do not close the socket, only continue.
			*/
			if (errno == EINTR)
			{
				continue;
			}

			perror("net_conn_send:select");
			net_conn_close(conn_info);
			return FAILURE;
		}
		else if (res > 0)
		{
			if (FD_ISSET(conn_info->client_conn, &writefds))
			{
				send_cnt = send(conn_info->client_conn, ptr, total_length, MSG_DONTWAIT | MSG_NOSIGNAL);
				//				ms_cnt1 = system_mscount_get();
				sys_log(FUNC,  LOG_DBG,  "send %d bytes\n", send_cnt);
			}
		}
		else
		{
			ms_cnt2 = system_mscount_get();

			//If the return value of select() is still Zero after 500ms, Close the socket
			if (ms_cnt2 > ms_cnt1 && ms_cnt2 - ms_cnt1 >= TIMEOUT_CNT)
			{
				net_conn_close(conn_info);
				printf("MS_CNT1: %llu, MS_CNT2: %llu.\n", ms_cnt1, ms_cnt2);
				printf("net_conn_send: Client has no response in %llu ms, we must Close this socket.\n", ms_cnt2 - ms_cnt1);
				return FAILURE;
			}

			continue;
		}

		if (send_cnt == -1)
		{
			perror("net_conn_send:send");
			net_conn_close(conn_info);
			return FAILURE;
		}

		if (send_cnt == 0)
		{
			net_conn_close(conn_info);

			return FAILURE;
		}

		if (total_length - send_cnt == 0)
		{
			return SUCCESS;
		}
		else
		{
			ms_cnt3 = system_mscount_get();

			//If the return value of select() is still Zero after 500ms, Close the socket
			if (ms_cnt3 > ms_cnt1 && ms_cnt3 - ms_cnt1 >= TIMEOUT_CNT)
			{
				//We must close this socket and return FAILURE when the client is in problem.
				//Otherwise system will stop to capture picture and video
				net_conn_close(conn_info);
				printf("net_conn_send: Client has something wrong, we must Close this socket.\n");
				return FAILURE;
			}

			ptr += send_cnt;
			total_length -= send_cnt;
		}
	}
}

int net_conn_send(NET_CONN_INFO *conn_info, ITSIP *net_head, BYTE* net_data, DWORD len)
{
	int ret;

	TS_LOCK(&conn_info->lock);
	sys_log(FUNC,  LOG_DBG,  "header packet\n");
	ret = net_conn_send_pkt(conn_info, net_head, sizeof(ITSIP));
	if(net_data != NULL && len > 0){
		sys_log(FUNC,  LOG_DBG,  "data packet\n");
		ret = net_conn_send_pkt(conn_info, net_data, MAX_LENGTH);
	}		
	TS_UNLOCK(&&conn_info->lock);
	return ret;
}


static void web_cmd_proc(ITSIP *p_net_head, NET_CONN_INFO *conn_info)
{
	int ret;
	ITSIP_PACKET its_ack_pak;
	
	if (conn_info->client_conn < 1)	
		return;
	
	if (p_net_head->itsip_head == ITS_HEAD)
	{
		switch (p_net_head->itsip_cmd)
		{
			case ITS_LOGIN:{
				sys_log(FUNC, LOG_MSG, "+++ITS_LOGIN+++");
				ret = sys_usr_login((char*)p_net_head->itsip_user, (char*)p_net_head->itsip_data);
				itsip_pack(ITS_ACK_LOGIN, 0, 0, NULL, &its_ack_pak);
				its_ack_pak.head.itsip_data[0] = ret;
				net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
			}break;
			
			case ITS_USER_QUERY:{
				
				sys_log(FUNC, LOG_MSG, "+++ITS_USER_QUERY+++");
				sys_usr_query(conn_info, p_net_head->itsip_user);
			}break;
			
			case ITS_USER_SET:{
				sys_log(FUNC, LOG_MSG, "+++ITS_USER_SET+++");
				ret = sys_usr_set(conn_info, p_net_head);
				its_ack_pak.head.itsip_data[0] = ret;
				net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
			}break;
			
			case ITS_SYSINFO_QUERY:{
				sys_log(FUNC, LOG_MSG, "+++ITS_SYSINFO_QUERY+++");
				sprintf(g_conf_info.con_sys.sw_ver, "%s", PROGRAM_VERSION);
				itsip_pack(ITS_ACK_SYSINFO_QUERY, sizeof(g_conf_info.con_sys), 0, NULL, &its_ack_pak);
				net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&g_conf_info.con_sys, sizeof(g_conf_info.con_sys));
			}break;
			
			case ITS_SYSINFO_SET:{
				sys_log(FUNC, LOG_MSG, "+++ITS_SYSINFO_SET+++");
				ret = sys_sysinfo_set(conn_info, p_net_head);
				itsip_pack(ITS_ACK_SYSINFO_SET, 0, 0, NULL, &its_ack_pak);
				its_ack_pak.head.itsip_data[0] = ret;
				net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
			}break;
			case ITS_CONF_QUERY:{
				sys_log(FUNC, LOG_MSG, "+++ITS_CONF_QUERY+++");
				sys_conf_query(conn_info, p_net_head);
			}break;
			case ITS_CONF_SET:{
				sys_log(FUNC, LOG_MSG, "+++ITS_CONF_SET+++");
				ret = sys_conf_set(conn_info, p_net_head);
				itsip_pack(ITS_ACK_CONF_SET, 0, 0, NULL, &its_ack_pak);
				its_ack_pak.head.itsip_data[0] = ret;
				net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
			}break;
			
			case ITS_NETINFO_QUERY:{
				sys_log(FUNC, LOG_MSG, "+++ITS_NETINFO_QUERY+++");
				itsip_pack(ITS_ACK_NETINFO_QUERY, sizeof(g_conf_info.con_net), 0, NULL, &its_ack_pak);
				net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&g_conf_info.con_net, sizeof(g_conf_info.con_net));
			}break;
			
			case ITS_NETINFO_SET:{
				sys_log(FUNC, LOG_MSG, "+++ITS_NETINFO_SET+++");
				ret = sys_netinfo_set(conn_info, p_net_head);
				itsip_pack(ITS_ACK_NETINFO_SET, 0, 0, NULL, &its_ack_pak);
				its_ack_pak.head.itsip_data[0] = ret;
				net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
			}break;
			
			case ITS_VLAN_QUERY:{
				sys_log(FUNC, LOG_MSG, "+++ITS_ACK_VLAN_QUERY+++");
				itsip_pack(ITS_ACK_VLAN_QUERY, sizeof(g_conf_info.con_vlan), 0, NULL, &its_ack_pak);
				net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&g_conf_info.con_vlan, sizeof(g_conf_info.con_vlan));
			}break;
			
			case ITS_VLAN_SET:{
				sys_log(FUNC, LOG_MSG, "+++ITS_VLAN_SET+++");
				ret = sys_vlan_set(conn_info, p_net_head);
				itsip_pack(ITS_ACK_VLAN_SET, 0, 0, NULL, &its_ack_pak);
				its_ack_pak.head.itsip_data[0] = ret;
				net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
			}break;
			
			case ITS_FACTORY_SET:{
				sys_log(FUNC, LOG_MSG, "+++ITS_FACTORY_SET+++");
				ret = sys_factory_set(conn_info, p_net_head);
				itsip_pack(ITS_ACK_FACTORY_SET, 0, 0, NULL, &its_ack_pak);
				its_ack_pak.head.itsip_data[0] = ret;
				net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);
			}break;

			case ITS_SERVER_QUERY:{
				sys_log(FUNC, LOG_MSG, "---ITS_SERVER_QUERY---%s %s %d", g_conf_info.con_server.dns_str,\
					sys_ip2str_static(g_conf_info.con_server.server_ip), g_conf_info.con_server.server_port);
						
				itsip_pack(ITS_ACK_SERVER_QUERY, sizeof(g_conf_info.con_server), 0, NULL, &its_ack_pak);
				net_conn_send(conn_info, &its_ack_pak.head, (BYTE*)&g_conf_info.con_server, sizeof(g_conf_info.con_server));
			}break;

			case ITS_SERVER_SET:{
				sys_log(FUNC, LOG_MSG, "---ITS_SERVER_SET---");
				ret = sys_server_set(conn_info, p_net_head);
				itsip_pack(ITS_ACK_SERVER_SET, 0, 0, NULL, &its_ack_pak);
				its_ack_pak.head.itsip_data[0] = ret;
				net_conn_send(conn_info, &its_ack_pak.head, NULL, 0);	

				if (ret == SERVER_SET_OK){
					//client_reconnect();
					g_reconnect_flag = RECONNECT_ON;
				}
			}break;		


			default:{
			sys_log(FUNC, LOG_WARN, "+++Unknown cmd[0x%x]+++", p_net_head->itsip_cmd);
			}break;
		}
	}
}


static void web_svr_proc(NET_CONN_INFO *conn_info)
{
	int net_recv_ret = -1;
	ITSIP net_data;

	if (conn_info->client_conn < 1){
		return;
	}

	memset(&net_data, 0, sizeof(ITSIP));

	if ((net_recv_ret = web_conn_recv(conn_info, &net_data, sizeof(ITSIP))) == FAILURE)	{
		return;
	}
	sys_log(FUNC, LOG_DBG, "[sock_fd %d] Is  ITSIP Header\n", conn_info->client_conn);

	web_cmd_proc(&net_data, conn_info);

}


/*APP TCP SERVER For HTTP client with boa & cgi*/
void web_process(void)
{
	int i;
	//int flag = 0;
	int addr_size = 0;
	int res = 0;
	int client_sock = -1;
	struct timeval tv;
	struct sockaddr_in client_addr;
	static int m_max_i = 0;         //! max client connection number
	static fd_set m_readfds;        //! set of read socket handles
	// int sendlen;


	FD_ZERO(&m_readfds);

	if (g_listen_conn > 0){
		FD_SET(g_listen_conn, &m_readfds);
	}

	for ( i = 0; i < MAX_TCP_CONN; i++)
	{
		if (g_client_conn[i].client_conn < 0)
		{
			continue;
		}

		FD_SET(g_client_conn[i].client_conn, &m_readfds);
	}

	tv.tv_sec = 0;
	tv.tv_usec = 10000;
	res =-1;
	sys_log(FUNC,  LOG_DBG,  "selecting\n");
	res = select(m_max_sock + 1, &m_readfds, NULL, NULL, NULL);
	sys_log(FUNC,  LOG_DBG,  "[m_max_sock %d]select %d\n",m_max_sock, res);
	if (res == 0)
	{
		return;
	}
	else if (res < 0)
	{
		if (errno == EINTR)
		{
			return;
		}

		perror("web_process: select");

		//Close all client connections
		for (i = 0; i < MAX_TCP_CONN; i++)
		{
			if (g_client_conn[i].client_conn > 0)
			{
				net_conn_close(&g_client_conn[i]);
			}
		}

		return;
	}
	else if (res > 0)
	{
		if (FD_ISSET(g_listen_conn, &m_readfds))
		{
			addr_size = sizeof(client_addr);
			sys_log(FUNC, LOG_DBG, "[sock_fd %d]accepting...\n",g_listen_conn);
			client_sock = accept(g_listen_conn, (struct sockaddr *)&client_addr, (socklen_t*)&addr_size);
			sys_log(FUNC, LOG_DBG, "[sock_fd %d]accepted %s\n", g_listen_conn,inet_ntoa(client_addr.sin_addr));

			if (client_sock > 0)
			{
				for (i = 0; i < MAX_TCP_CONN; i++)
				{
					if (g_client_conn[i].client_conn < 0)
					{
						g_client_conn[i].conn_idx = i;
						g_client_conn[i].client_conn = client_sock;
						g_client_conn[i].idle = 0;
						strcpy(g_client_conn[i].clientip, inet_ntoa(client_addr.sin_addr));


						if (i > m_max_i)
						{
							m_max_i = i;
						}

						break;
					}
				}

				if (i == MAX_TCP_CONN)
				{
					close(client_sock);
					sys_log(FUNC, LOG_WARN, "[sock_fd %d] close", client_sock);
					
					client_sock = -1;
				}
			}

			if (client_sock > m_max_sock)
			{
				m_max_sock = client_sock;
			}
		}

		for (i = 0; i <= m_max_i; i++)
		{
			if (g_client_conn[i].client_conn < 0)
			{
				continue;
			}

			if (FD_ISSET(g_client_conn[i].client_conn, &m_readfds))
			{
				g_client_conn[i].idle = 0;				
				web_svr_proc(&g_client_conn[i]);				
				//sys_log(FUNC, LOG_TRACE, "[sock_fd %d]  game over", g_client_conn[i].client_conn);
			}
		}
	}
}


