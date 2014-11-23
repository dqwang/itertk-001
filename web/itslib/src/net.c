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

#if 1


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


int sys_log ( char* module, LOG_Lv level, char* format, ... )
{
    char szbuf[1024*8+256] = "", szcontent[1024*8] = "";
    va_list ap;
    struct timeval tv;
    struct tm stm;
    const char * g_log_lv[] =
    {
        "NULL",
        "TRACE",
        "DBG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
    };

    const int color[] = {0, 37, 36, 32, 33, 31, 35};
    
    va_start ( ap, format );
    vsprintf ( szcontent, format, ap );
    va_end ( ap );

    gettimeofday ( &tv, NULL );
    stm = * ( localtime ( &tv.tv_sec ) );

    if(szcontent[strlen(szcontent) - 1] == '\n') 
        szcontent[strlen(szcontent) - 1] = '\0';
    
    sprintf ( szbuf, "\033[%d;40m%04d-%02d-%02d %02d:%02d:%02d:%06ld---[%s][%s] %s\033[0m\n",color[level],
                stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, tv.tv_usec,
                module, g_log_lv[level], szcontent );
        
    printf ( "%s", szbuf );
    return 1;

}


#endif



int socket_init(DWORD port, DWORD max_conn)
{
    int fd , ret = -1;
    int flag = 0;
    struct sockaddr_in server_addr;
	static int cnt =0;
	sys_log(FUNC, LOG_DBG,"%d", cnt++ );

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

//////////////////////////////////////////////////////////////////////////
///
///     关闭连接
///     @param  *conn_info  连接信息
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010－04－24
//////////////////////////////////////////////////////////////////////////
void net_conn_close(NET_CONN_INFO *conn_info)
{
    BYTE chan;
	static int cnt =0;
	sys_log(FUNC, LOG_DBG,"%d", cnt++ );

    //printf("Connection <%d> closed.\n", conn_info->client_conn);

    if (conn_info->client_conn > 0)
    {
        close(conn_info->client_conn);

        conn_info->client_conn = -1;
        conn_info->file_type = 0xff;

    }
}

int net_conn_connect(NET_CONN_INFO *conn_info)
{
        struct sockaddr_in  addr;
       static int cnt =0;

        conn_info->client_conn = socket( AF_INET, SOCK_STREAM, 0 );
    
        if (conn_info->client_conn < 0)
        {
            perror("socket");
            return FAILURE;
        }

	sys_log(FUNC, LOG_DBG,"%d", cnt++ );
       
        bzero(&addr, sizeof(addr));
        addr.sin_family     = AF_INET;
        addr.sin_addr.s_addr    = inet_addr(WEB_IP);
        addr.sin_port       = htons(WEB_PORT);
    
        //连接目标相机
    
        //printf("it will connect to serv!\n");
    
        if ( connect( conn_info->client_conn, (const struct sockaddr *)&addr, sizeof(addr)) < 0 )
        {
            perror("connect timeout");
            close(conn_info->client_conn);
            return FAILURE;
        }
        //printf(" connect to serv ok!\n");
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

        res = select(conn_info->client_conn + 1, NULL, &writefds, NULL, &tv);

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
	static int cnt =0;
	sys_log(FUNC, LOG_DBG,"%d", cnt++ );

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
	static int cnt =0;
	sys_log(FUNC, LOG_DBG,"%d", cnt++ );

	
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

    return SUCCESS;

}

