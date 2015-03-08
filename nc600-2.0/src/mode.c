//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   mode.c
/// @brief  网络处理
///
///
///
/// @version    2.0
/// @author     xuliang<gxuliang@gmail.com>
/// @date       2010-05-05
///
///
///     修订说明：最初版本
//////////////////////////////////////////////////////////////////////////

#include "config.h"
#include "mode.h"
#include "com.h"
#include "thread.h"
#include "log.h"


MODECONFIG mode[MAX_COM_PORT][MAX_SESSION];

//////////////////////////////////////////////////////////////////////////
///
///     建立客户端连接
///     @param id 网络序号
///     @param *data 数据首地址
///     @param len 数据长度
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
int socket_cli_init(DWORD ip, DWORD port)
{
    int fd;
    struct sockaddr_in	addr;
    fd = socket( AF_INET, SOCK_STREAM, 0 );

    if (fd < 0)
    {
        perror("socket");
        return fd;
    }

	int rcvlen = 1024*1024;
    int ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvlen, sizeof(int));

    if (ret == -1)
    {
        perror("net_server_start: setsockopt()");
        return ret;
    }

    bzero(&addr, sizeof(addr));
    addr.sin_family		= AF_INET;
    addr.sin_addr.s_addr	= (ip);
    addr.sin_port		= htons(port);

    //连接目标相机

    printf("it will connect to serv!ip is %s, port = %d\n", sys_ip2str_static(ip), port);

    if ( connect( fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0 )
    {
        perror("connect timeout");
        close(fd);
        return -1;
    }

    printf(" connect to serv ok!\n");
    return fd;

}
//////////////////////////////////////////////////////////////////////////
///
///     建立客户端连接
///     @param id 网络序号
///     @param *data 数据首地址
///     @param len 数据长度
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
int socket_init(DWORD port, DWORD max_conn)
{
    int fd , ret = -1;
    int flag = 0;
    struct sockaddr_in server_addr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("socket");
        return -1;
    }

    // Enable address reuse and keeplive
    flag = ON;
    ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

    if (ret == -1)
    {
        perror("socket_init: setsockopt");
        return ret;
    }

	int rcvlen = 1024*1024;
    ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvlen, sizeof(int));

    if (ret == -1)
    {
        perror("net_server_start: setsockopt()");
        return ret;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));


    if (bind(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("socket_init: bind");
        close(fd);

        return -1;
    }

    if (listen(fd, max_conn) == -1)
    {
        perror("socket_init: listen");
        close(fd);

        return -1;
    }

    return fd;
}
//////////////////////////////////////////////////////////////////////////
///
///     建立客户端连接
///     @param id 网络序号
///     @param *data 数据首地址
///     @param len 数据长度
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
int init_udp_socket(int port)
{
	int ret = 0;
    struct sockaddr_in addr;

	ret = socket(AF_INET, SOCK_DGRAM, 0);
	if(ret < 0)
	{
		perror("socket");
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(ret, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
///
///     网络数据发送
///     @param id 网络序号
///     @param *data 数据首地址
///     @param len 数据长度
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
inline int SendComDataToNet(int id, void *data, int len)
{
    MODECONFIG *md = mode[id];
    int i, j;
    struct sockaddr_in my_addr;
    FOR(i, MAX_SESSION)
    {
    printf("md[i].session->protocol =%d\n", md[i].session->protocol);
        switch(md[i].session->protocol)
        {
        case TCP_CLI:
            if(md[i].fd)
                send(md[i].fd, data, len, 0);
            break;
        case TCP_SRV:
            FOR(j, MAX_SESSION)
            {
                if(md[i].sfd[j])
                    send(md[i].sfd[j], data, len, 0);
            }
            break;
        case TCP_UDP:
            if(md[i].fd)
            {
                my_addr.sin_family = AF_INET;
                my_addr.sin_port = htons(md[i].session->dport);
                my_addr.sin_addr.s_addr = md[i].session->ip;
                bzero(&(my_addr.sin_zero), 8);
                sendto(md[i].fd, data, len, 0, (struct sockaddr *)&my_addr, sizeof(my_addr));
            }
            break;
        }
    }

	return 0;//WANG
}

//////////////////////////////////////////////////////////////////////////
///
///     网络数据发送
///     @param id 网络序号
///     @param *data 数据首地址
///     @param len 数据长度
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////

#define NET_RECV_BUF_MAX (1024*64L)
static void mode_proc(void* arg)
{
    MODECONFIG *md = (MODECONFIG*)arg;
    const CONFIG_MODE* con_mode = &g_conf_info.con_mode[md->id];
    fd_set m_readfds;
    struct timeval tv;
    int ret, len, i, max_fd, client_sock, addr_size;
    CONFIG_SESSION tmp;
    char lsbuf[NET_RECV_BUF_MAX];
    struct sockaddr_in addr;
    struct sockaddr_in client_addr;
    socklen_t addr_len;
static int cnt =0;

AGAIN:
    if(con_mode->mode == WORK_NO || md->session->protocol == TCP_NO)
    {
        sleep(1);
        goto AGAIN;
    }
    else if(md->session->protocol == TCP_CLI)
    {
        if((md->fd = socket_cli_init(md->session->ip, md->session->dport)) <= 0)
        {
            {
                sys_log(FUNC, LOG_WARN, "[%d]connect ip is %s, port = %d failed", \
                md->id, sys_ip2str_static(md->session->ip), md->session->dport);
            
                sleep(5);
                goto AGAIN;
            }
        }
    }
    else if(md->session->protocol == TCP_SRV)
    {
        if((md->fd = socket_init(md->session->lport, md->session->max_num)) <= 0)
        {    
            sys_log(FUNC, LOG_WARN, "creat server socket failed");
        
            sleep(5);
            goto AGAIN;
        }
    }
    else if(md->session->protocol == TCP_UDP)
    {
        if((md->fd = init_udp_socket(md->session->lport)) <= 0)
        {    
            sys_log(FUNC, LOG_WARN, "creat udp socket failed");
        
            sleep(5);
            goto AGAIN;
        }
    }
    else
    {
        sleep(1);
        goto AGAIN;
    }
    //==============================================================
    memcpy(&tmp, md->session, sizeof(CONFIG_SESSION));
    while(1)
    {
	sys_log(FUNC, LOG_DBG,"%d\n",cnt++);
	if(con_mode->mode == WORK_NO || memcmp(md->session, &tmp, sizeof(CONFIG_SESSION)) != 0)
        {
            close(md->fd);
            md->fd = 0;
            FOR(i, MAX_SESSION)
            {
                if(md->sfd[i])
                {
                    close(md->sfd[i]);
                    md->sfd[i] = 0;
                }
            }
            goto AGAIN;
        }
        
        FD_ZERO(&m_readfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        switch(md->session->protocol)
        {
        case TCP_CLI:
            FD_SET(md->fd, &m_readfds);
            ret = select(md->fd + 1, &m_readfds, NULL, NULL, &tv);
            if(ret <= 0)
                continue;
            else
            {
                len = recv(md->fd, lsbuf, NET_RECV_BUF_MAX, 0);
                if(len == 0)
                {
                    close(md->fd);
                    md->fd = 0;
                    goto AGAIN;
                }
                SendNetDataToCom(md->id, lsbuf, len);
            }
            break;
        case TCP_SRV:
            FD_SET(md->fd, &m_readfds);
            max_fd = md->fd;
            FOR(i, MAX_SESSION)
            {
                if(md->sfd[i] > 0)
                {
                    FD_SET(md->sfd[i], &m_readfds);
                    max_fd = max_fd > md->sfd[i] ? max_fd : md->sfd[i];
                }
            }
            ret = select(max_fd + 1, &m_readfds, NULL, NULL, &tv);
            if (FD_ISSET(md->fd, &m_readfds))
            {
                client_sock = accept(md->fd, (struct sockaddr *) & client_addr, (socklen_t*)&addr_size);
                FOR(i, MAX_SESSION)
                {
                    if(md->sfd[i] == 0)
                        break;
                }
                if(i == MAX_SESSION)
                {
                    sys_log(FUNC, LOG_WARN, "reach max connection!");
                    close(client_sock);
                }
                else
                {
                    sys_log(FUNC, LOG_MSG, "++++++++++++++++++++新连接进入 %s.++++++++++++++++++++\n", inet_ntoa(client_addr.sin_addr));
                    md->sfd[i] = client_sock;
                }
            }
            FOR(i, MAX_SESSION)
            {
                if (md->sfd[i] > 0 && FD_ISSET(md->sfd[i], &m_readfds))
                {
                    len = recv(md->sfd[i], lsbuf, NET_RECV_BUF_MAX, 0);
                    if(len == 0)
                    {
                        close(md->sfd[i]);
                        md->sfd[i] = 0;
                        continue;
                    }
                    SendNetDataToCom(md->id, lsbuf, len);
                }
            }
            break;
        case TCP_UDP:
            FD_SET(md->fd, &m_readfds);
            ret = select(md->fd + 1, &m_readfds, NULL, NULL, &tv);
            if(ret <= 0)
                continue;
            else
            {
                len = recvfrom(md->fd, lsbuf, NET_RECV_BUF_MAX, 0, (struct sockaddr *)&addr, &addr_len);
                SendNetDataToCom(md->id, lsbuf, len);
            }
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
///
///     网络初始化
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
void mode_init(void)
{
    int i, j;
    TRD_t mode_trd;
    
    memset(mode, 0, sizeof(mode));
    FOR(i, MAX_COM_PORT)
    {
        FOR(j, MAX_SESSION)
        {
            mode[i][j].id = i;
            mode[i][j].session = &g_conf_info.con_mode[i].session[j];
            trd_create(&mode_trd, (void*)&mode_proc, &mode[i][j]);
        }
    }
}



#define BUFLEN 255
//#define GROUP_IP "224.0.0.200"
#define GROUP_IP "239.0.0.100"

//#define GROUP_IP "255.255.255.255"
#define GROUP_PORT "31000"
#define PC_TOOL_MSG1 "hello"
//#define PC_TOOL_ACK "fucking"
#define PC_TOOL_ACK "192.168.1.101|255.255.255.0|192.168.1.1|192.168.1.1|0.0.0.0|192.168.1.100|8888"
int g_report_sfd=0;

//DWORD sys_str2ip ( char *str )


//char* sys_ip2str_static ( DWORD ip )

extern void config_net_set(CONFIG_NET *pConf);
extern void dns_init(char * dns_str);
void pc_set_dev_info(char *recv_str)
{
	char* p = strtok(recv_str, "|");
	CONFIG_NET conf_net;
	CONFIG_SERVER con_server;
	
	//g_conf_info.con_net
	//g_conf_info.con_server
	
	if(p){
		printf("%s\n",p);
		
		if (g_conf_info.con_net.dev_ip != sys_str2ip(p)){
			sys_log(FUNC,LOG_ERR, "%s","NOT ME");
			return;
		}
		
	}

	memcpy(&conf_net, &g_conf_info.con_net, sizeof(CONFIG_NET));
	memcpy(&con_server, &g_conf_info.con_server, sizeof(CONFIG_SERVER));
	
	p = strtok(NULL, "|");
	if(p){
		printf("%s\n",p);
		conf_net.dev_ip = sys_str2ip(p);
	}

	p = strtok(NULL, "|");
	if(p){
		printf("%s\n",p);
		conf_net.dev_nm =sys_str2ip(p);
	}

	p = strtok(NULL, "|");
	if(p){
		printf("%s\n",p);
		conf_net.dev_gw =sys_str2ip(p);
	}

	p = strtok(NULL, "|");
	if(p){
		printf("%s\n",p);
		conf_net.dns[0] = sys_str2ip(p);
	}

	p = strtok(NULL, "|");
	if(p){
		printf("%s\n",p);
		conf_net.dns[1] = sys_str2ip(p);
	}

	p = strtok(NULL, "|");
	if(p){
		printf("%s\n",p);
		strcpy(con_server.dns_str, p);
		//con_server.server_ip = sys_str2ip(p);
	}

	p = strtok(NULL, "|");
	if(p){
		printf("%s\n",p);
		con_server.server_port = atoi(p);
	}	
	memcpy(&g_conf_info.con_net, &conf_net, sizeof(CONFIG_NET));	
	memcpy(&g_conf_info.con_server, &con_server, sizeof(CONFIG_SERVER));	
	config_net_set(&g_conf_info.con_net);
	
#if 1//if run this code, ....PC Search Tool may be failed...
	dns_init(con_server.dns_str);//
#endif	
	config_save(&g_conf_info);
	

	

}



void pc_get_dev_info(char *info)
{
	char dev_ip[16]="";
	char dev_nm[16]="";
	char dev_gw[16]="";
	char dev_dns1[16]="";
	char dev_dns2[16]="";
//	char dev_serverip[16]="";
	char dev_dns_str[50]="";

	strcpy(dev_ip, sys_ip2str_static(g_conf_info.con_net.dev_ip));
	strcpy(dev_nm, sys_ip2str_static(g_conf_info.con_net.dev_nm));
	strcpy(dev_gw, sys_ip2str_static(g_conf_info.con_net.dev_gw));
	strcpy(dev_dns1, sys_ip2str_static(g_conf_info.con_net.dns[0]));
	strcpy(dev_dns2, sys_ip2str_static(g_conf_info.con_net.dns[1]));
	//strcpy(dev_serverip, sys_ip2str_static(g_conf_info.con_server.server_ip));
	strcpy(dev_dns_str, g_conf_info.con_server.dns_str);
	
	
	sprintf(info, "%s|%s|%s|%s|%s|%s|%d", dev_ip, dev_nm, dev_gw, dev_dns1, dev_dns2, dev_dns_str, g_conf_info.con_server.server_port);
	
}


	

void report_proc(void)
{
	char recmsg[BUFLEN + 1];
	char sendmsg[BUFLEN + 1]=PC_TOOL_ACK;
	unsigned int n=0;
	struct sockaddr_in peeraddr;
	struct in_addr ia;	
	unsigned int socklen;
	struct hostent *group;
	struct ip_mreq mreq;
	int so_broadcast = 1;
	int optval = 0;
	char tmp[16]="";
	
	struct sockaddr_in servaddr;

	g_report_sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (g_report_sfd < 0) {
		sys_log(FUNC, LOG_ERR,"report socket err"); 
		return ;
	}
	bzero(&mreq, sizeof(struct ip_mreq));
	group = gethostbyname(GROUP_IP) ;
	if (group == (struct hostent *) 0) {
		perror("gethostbyname");
		return ;
	}	

	bcopy((void *) group->h_addr, (void *) &ia, group->h_length);	
	bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	
	if (setsockopt(g_report_sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq,sizeof(struct ip_mreq)) == -1) {
		perror("setsockopt");
		return ;
	}	
	setsockopt(g_report_sfd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&optval, sizeof(int));
	
	
	//setsockopt(g_report_sfd, SOL_SOCKET, SO_BROADCAST, (char *)&so_broadcast, sizeof(so_broadcast));
	
	socklen = sizeof(struct sockaddr_in);
	memset(&peeraddr, 0, socklen);
	peeraddr.sin_family = AF_INET;
	inet_pton(AF_INET, GROUP_IP, &peeraddr.sin_addr);
	peeraddr.sin_port = htons(atoi(GROUP_PORT));


	memset(&servaddr, 0, socklen);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(GROUP_PORT));	
	if (bind(g_report_sfd, (struct sockaddr *) &servaddr,sizeof(struct sockaddr_in)) == -1) {
		sys_log(FUNC, LOG_ERR,"bind"); 
		return ;
	}
	
	sys_log(FUNC, LOG_DBG,"OK: GROUP_IP=%s, GROUP_PORT=%s", GROUP_IP, GROUP_PORT); 
	for(;;){
		struct sockaddr_in sb_addr;
		unsigned int sb_len;		
		bzero(recmsg, BUFLEN + 1);
		n = recvfrom(g_report_sfd, recmsg, BUFLEN, 0,(struct sockaddr *) &sb_addr, &sb_len);
		
		if (n <= 0) {
			sys_log(FUNC, LOG_ERR,"recvfrom"); 			
		} else {			
			recmsg[n] = 0;
			sys_log(FUNC, LOG_DBG,"recvfrom %s", recmsg); 

			if (0 == strcmp(recmsg , PC_TOOL_MSG1)){
				
				/*TODO: 获取IP 等信息*/	
				pc_get_dev_info(sendmsg);

				//inet_ntop(AF_INET, peeraddr.sin_addr.s_addr, tmp, 16);
				//sys_log(FUNC, LOG_DBG,"sendto addr: %s", tmp); 
				
				if (sendto	(g_report_sfd, sendmsg, strlen(sendmsg), 0,	(struct sockaddr *) &peeraddr,sizeof(struct sockaddr_in)) <=0) {
					//sys_log(FUNC, LOG_ERR,"can't sendto %s", sendmsg); 
					perror("sendto");
				}else{
					sys_log(FUNC, LOG_DBG,"send ack packet %s", sendmsg); 
				}				
			}else{
				/*TODO: 解析，比对，设置*/

				pc_set_dev_info(recmsg);
			}			
		}
	}
}

void report_dev_info_init(void)
{
	TRD_t report_trd;
	trd_create(&report_trd, (void*)&report_proc, NULL);
}

	
