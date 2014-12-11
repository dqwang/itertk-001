//////////////////////////////////////////////////////////////////////////
///    COPYRIGHT NOTICE
///    Copyright (c) 2010, 浙江共创技术有限公司
///    All rights reserved.
///
/// @file   net.c
/// @brief  处理网络数据
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

#include "def.h"
#include "itsip.h"
#include "net.h"
#include "system.h"




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


void net_conn_close(NET_CONN_INFO *conn_info)
{
	//printf("{net_conn_close} conn_info->client_conn <%d> \n", conn_info->client_conn);

	if (conn_info->client_conn > 0){
		close(conn_info->client_conn);
		
		conn_info->client_conn = -1;
		conn_info->file_type = 0xff;
	}
}

int net_conn_connect(NET_CONN_INFO *conn_info)
{
	struct sockaddr_in  addr;
	conn_info->client_conn = socket( AF_INET, SOCK_STREAM, 0 );

	if (conn_info->client_conn < 0){
		perror("socket");
		return FAILURE;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family     = AF_INET;
	addr.sin_addr.s_addr    = inet_addr(WEB_IP);
	addr.sin_port       = htons(WEB_PORT);
	
	if ( connect( conn_info->client_conn, (const struct sockaddr *)&addr, sizeof(addr)) < 0 ){
		perror("connect timeout");
		close(conn_info->client_conn);
		return FAILURE;
	}
	//printf("{net_conn_connect}WEB_IP %s, WEB_PORT:%d \n", WEB_IP, WEB_PORT);
	return SUCCESS;
    
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

	if (conn_info->client_conn <= 0){
		return FAILURE;
	}

	conn_info->idle = 0;
	ms_cnt1 = system_mscount_get();

	ptr = (BYTE*)net_data;
	total_length = len;

	while (1){
		FD_ZERO(&writefds);
		FD_SET(conn_info->client_conn, &writefds);
		tv.tv_sec = 0;
		tv.tv_usec = 10000;

		res =-1;
		
		res = select(conn_info->client_conn + 1, NULL, &writefds, NULL, &tv);

		if (res < 0){
			
			if (errno == EINTR)
				continue;			

			perror("net_conn_send:select");
			net_conn_close(conn_info);
			return FAILURE;
		}
		else if (res > 0)	{
			if (FD_ISSET(conn_info->client_conn, &writefds))	{
				send_cnt = send(conn_info->client_conn, ptr, total_length, MSG_DONTWAIT | MSG_NOSIGNAL);
				//ms_cnt1 = system_mscount_get();
			}
		}
		else	{
			ms_cnt2 = system_mscount_get();

			//If the return value of select() is still Zero after 500ms, Close the socket
			if (ms_cnt2 > ms_cnt1 && ms_cnt2 - ms_cnt1 >= TIMEOUT_CNT)	{
				net_conn_close(conn_info);
				printf("MS_CNT1: %llu, MS_CNT2: %llu.\n", ms_cnt1, ms_cnt2);
				printf("net_conn_send: Client has no response in %llu ms, we must Close this socket.\n", ms_cnt2 - ms_cnt1);
				return FAILURE;
			}

			continue;
		}

		if (send_cnt == -1){
			perror("net_conn_send:send");
			net_conn_close(conn_info);
			return FAILURE;
		}

		if (send_cnt == 0){
			net_conn_close(conn_info);

			return FAILURE;
		}

		if (total_length - send_cnt == 0){
			return SUCCESS;
		}
		else
		{
			ms_cnt3 = system_mscount_get();

			//If the return value of select() is still Zero after 500ms, Close the socket
			if (ms_cnt3 > ms_cnt1 && ms_cnt3 - ms_cnt1 >= TIMEOUT_CNT){
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

	//printf("{net_conn_send}  %d bytes\n", len);
	ret = net_conn_send_pkt(conn_info, net_head, sizeof(ITSIP));
	if(net_data != NULL && len > 0)
		ret = net_conn_send_pkt(conn_info, net_data, MAX_LENGTH);
	return ret;
}

int net_conn_recv(NET_CONN_INFO *conn_info, void* data, DWORD len)
{
	fd_set readfds;
	struct timeval tv;
	int res;

	FD_ZERO(&readfds);
	FD_SET(conn_info->client_conn, &readfds);
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	res = select(conn_info->client_conn + 1, &readfds, NULL, NULL, &tv);
	if(res <= 0)
		return FAILURE;

	res  = recv(conn_info->client_conn, data, len, 0);
	if(res != len)
		return FAILURE;

	//printf("{net_conn_recv}  %d bytes\n", len);

	return SUCCESS;

}

