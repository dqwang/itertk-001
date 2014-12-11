#ifndef __WEB_NET_H__
#define __WEB_NET_H__

#include "def.h"
#include "user.h"
#include "thread.h"
#define MAX_TCP_CONN    9
#define WEB_PORT        32000
#define TIMEOUT_CNT     2000

///Net Connection information
typedef struct tagNET_CONN_INFO
{
	int conn_idx;		            //!< Index of connection
	int client_conn;	            //!< client socket descriptor
	USR_INFO *user;		            //!< pointer to user data in memory
	BYTE client_type;	            //!< client type
	BYTE file_type;		            //!< file type, 0 - picture, 1 - video, 2 -- break-point continue
	BYTE idle;	            		//!< idle time count
	char clientip[16];
	LOCK_t lock;
} NET_CONN_INFO, *pNET_CONN_INFO;

extern void web_init(void);

extern void web_process(void);


#endif

