#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "file.h"
#include "com.h"
#include "mode.h"
#include "log.h"
#include "cmd_def.h"

CONFI_DATA g_conf_info;
void get_mac(void);


void config_makedefault ( CONFI_DATA *pConf )
{

    //系统参数
    bzero(pConf, sizeof(CONFI_DATA));

    
    strcpy ( pConf->con_sys.host_name, PRODUCT_NAME );
    bzero ( pConf->con_sys.host_pos, sizeof ( pConf->con_sys.host_pos ) );
    sprintf(pConf->con_sys.sw_ver, "%s  %s  %s", PROGRAM_VERSION, __DATE__, __TIME__);


    
    pConf->con_net.dev_ip = sys_str2ip ( "192.168.1.101" );
    pConf->con_net.dev_nm = sys_str2ip ( "255.255.255.0" );
    pConf->con_net.dev_gw = sys_str2ip ( "192.168.1.1" );
    pConf->con_net.dev_dhcp = OFF;
    pConf->con_net.dhcp_id = 0;
    pConf->con_net.dns[0] = pConf->con_net.dns[1] = 0;


    //串口参数
    int i, j;
    FOR ( i, MAX_COM_PORT )
    {
        pConf->con_com[i].id = i + 1;
        pConf->con_com[i].bps = 115200;
        pConf->con_com[i].chk = 3;  //无校验
        pConf->con_com[i].sbit = 1;
        pConf->con_com[i].dbit = 8;
        pConf->con_com[i].bctrl = 0;
        pConf->con_com[i].RTS = OFF;
        pConf->con_com[i].DTR = OFF;
        pConf->con_com[i].type = RS232;
    }

    //工作模式
    FOR ( i, MAX_COM_PORT )
    {
        pConf->con_mode[i].id = i + 1;
        pConf->con_mode[i].mode = WORK_NO;//
        pConf->con_mode[i].tcp_mode = TCP_RAW;
        pConf->con_mode[i].CR = TO_CR;
        pConf->con_mode[i].LF = TO_LF;
        pConf->con_mode[i].attestation = 0;
        pConf->con_mode[i].is_null = NO;
        pConf->con_mode[i].att = NO;
        pConf->con_mode[i].reg = NO;
        FOR ( j, MAX_SESSION )
        {
            pConf->con_mode[i].session[j].protocol = TCP_NO;//
         //   pConf->con_mode[i].session[j].ip =sys_str2ip ( "192.168.1.88" );
            pConf->con_mode[i].session[j].ip =0;
            pConf->con_mode[i].session[j].lport = 0	;
            pConf->con_mode[i].session[j].dport = 0;
            pConf->con_mode[i].session[j].conn = 0;
            pConf->con_mode[i].session[j].dis_conn = 0;
            pConf->con_mode[i].session[j].time_out = 0;
            pConf->con_mode[i].session[j].max_num = 0;
        }
    }

    //端口权限管理
    FOR ( i, MAX_COM_PORT )
    {
        pConf->con_lim[i].id = i + 1;
        pConf->con_lim[i].enable = NO;
        FOR(j, MAX_USR_NUM)
        {
            pConf->con_lim[i].read[j] = NO;
            pConf->con_lim[i].modify[j] = NO;
        }
    }

    //用户管理
    FOR ( i, MAX_USR_NUM)
    {
        pConf->con_usr[i].usr_id = i + 1;
        bzero(pConf->con_usr[i].usr_name, USR_NAME_LEN);
        bzero(pConf->con_usr[i].usr_passwd, USR_PSW_LEN);
        pConf->con_usr[i].usr_auth = USR_NORMAL;
    }
    strcpy((char*)pConf->con_usr[0].usr_name, "root");
    strcpy((char*)pConf->con_usr[0].usr_passwd, crypt("123456", USR_SALT));
    pConf->con_usr[0].usr_auth = USR_ROOT;

    
 
    config_save ( pConf );
}

void config_load ( CONFI_DATA *pConf )
{
    FILE *fp = sys_file_open ( CONFIG_FILE, MODE_READ_T );
    if ( fp == NULL )
    {
        sys_log ( FUNC, LOG_ERR, "日志文件打开失败！" );
        return;
    }

    if ( sys_file_read ( fp, pConf, sizeof ( CONFI_DATA ) ) != sizeof ( CONFI_DATA ) )
    {
        sys_log ( FUNC, LOG_ERR, "日志文件读出失败！" );
    }
    sys_file_close ( fp );
}


void config_net_set(CONFIG_NET *pConf)
{
	char cmd[64] = "";
	char ip[16],nm[16];

	strcpy(ip,sys_ip2str_static(pConf->dev_ip));
	strcpy(nm,sys_ip2str_static(pConf->dev_nm));
	
	sprintf(cmd, "ifconfig eth0 %s netmask %s", ip, nm);

	sys_log(FUNC, LOG_MSG, "cmd1 is [%s]", cmd);
	system(cmd);

	sprintf(cmd, "ifconfig eth0 hw ether %02x:%02x:%02x:%02x:%02x:%02x",\
			 pConf->dev_mac[0], pConf->dev_mac[1], pConf->dev_mac[2],\
		 	pConf->dev_mac[3],  pConf->dev_mac[4], pConf->dev_mac[5]);
	sys_log(FUNC, LOG_MSG, "cmd2 is [%s]", cmd);
	system(cmd);
	

	/*

	printf(" dev_ip = %s \n", sys_ip2str_static(pConf->dev_ip));
	printf(" dev_gw = %s \n", sys_ip2str_static(pConf->dev_gw));
	printf(" dev_nm = %s \n", sys_ip2str_static(pConf->dev_nm));
	printf(" 1.dns = %s \n", sys_ip2str_static(pConf->dns[0]));
	printf(" 2.dns = %s \n", sys_ip2str_static(pConf->dns[1]));
	
	printf(" dev_dhcp = %d \n", pConf->dev_dhcp);

	printf(" mac =  %02x:%02x:%02x:%02x:%02x:%02x\n", pConf->dev_mac[0],pConf->dev_mac[1],pConf->dev_mac[2],pConf->dev_mac[3],\
				pConf->dev_mac[4],pConf->dev_mac[5]);
	
	*/
		

}

void config_init ( void )
{
	if ( file_is_existed ( CONFIG_FILE ) == 0 )	{
		sys_log(FUNC, LOG_WARN, "配置文件不存在，生成默认配置！");
		config_makedefault ( &g_conf_info );
	} else {
		config_load ( &g_conf_info );
	}
	config_net_set(&g_conf_info.con_net);
	get_mac();
}

void config_save ( CONFI_DATA *pConf )
{
    FILE *fp = sys_file_open ( CONFIG_FILE, MODE_WRITE_T );
    if ( fp == NULL )
    {
        sys_log ( FUNC, LOG_ERR, "日志文件打开失败！" );
        return;
    }

    if ( sys_file_write ( fp, pConf, sizeof ( CONFI_DATA ) ) != SUCCESS )
    {
        sys_log ( FUNC, LOG_ERR, "日志文件写入失败！" );
    }
    sys_file_close ( fp );
}

USR_INFO* get_usr_info(BYTE *name)
{
    int i;
    FOR(i, MAX_USR_NUM)
    {
        if(strcmp((char*)name, (char*)g_conf_info.con_usr[i].usr_name) == 0)
            return &g_conf_info.con_usr[i];
    }
    return NULL;
}

int sys_usr_add(BYTE* name, BYTE* psw)
{
    int i;
    FOR(i, MAX_USR_NUM)
    {
        if(g_conf_info.con_usr[i].usr_name[0] == '\0')
        {
            strcpy((char*)g_conf_info.con_usr[i].usr_name, (char*)name);
            strcpy((char*)g_conf_info.con_usr[i].usr_passwd, (char*)psw);
            config_save(&g_conf_info);
            return USR_ADD_OK;
        }
    }
    return USR_ADD_FAILED;
}

int sys_usr_del(BYTE* name)
{
    int i;
    FOR(i, MAX_USR_NUM)
    {
        if(strcmp((char*)name, (char*)g_conf_info.con_usr[i].usr_name) == 0)
        {
            bzero(g_conf_info.con_usr[i].usr_name, USR_NAME_LEN);
            config_save(&g_conf_info);
            return USR_DEL_OK;
        }
    }
    return USR_DEL_FAILED;
}

int set_usr_name(BYTE *oname, BYTE *nname)
{
    int i;
    FOR(i, MAX_USR_NUM)
    {
        if(strcmp((char*)oname, (char*)g_conf_info.con_usr[i].usr_name) == 0)
        {
            bzero((char*)g_conf_info.con_usr[i].usr_name, USR_NAME_LEN);
            strcpy((char*)g_conf_info.con_usr[i].usr_name, (char*)nname);
            config_save(&g_conf_info);
            return USR_MODIFY_OK;
        }
    }
    return USR_MODIFY_FAILED;
}
int set_usr_psw(BYTE *name, BYTE *psw)
{
    int i;
    FOR(i, MAX_USR_NUM)
    {
        if(strcmp((char*)name, (char*)g_conf_info.con_usr[i].usr_name) == 0)
        {
            bzero((char*)g_conf_info.con_usr[i].usr_passwd, USR_PSW_LEN);
            strcpy((char*)g_conf_info.con_usr[i].usr_passwd, (char*)psw);
            config_save(&g_conf_info);
            return USR_MODIFY_OK;
        }
    }
    return USR_MODIFY_FAILED;
}




#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <netinet/in.h>

void get_mac(void)
{
	int sockfd;
	struct ifreq struReq;
	
	sockfd = socket(PF_INET,SOCK_STREAM,0);
	memset(&struReq,0,sizeof(struReq));

	strncpy(struReq.ifr_name, "eth0", sizeof(struReq.ifr_name));

	ioctl(sockfd,SIOCGIFHWADDR,&struReq);
	
	{
		int j;
		printf("HWADDR :	");
		for (j=0;j<6;j++)
			printf("%02x ", struReq.ifr_hwaddr.sa_data[j]);
		printf("\n");
		
		memcpy(g_conf_info.con_net.dev_mac, struReq.ifr_hwaddr.sa_data, sizeof(g_conf_info.con_net.dev_mac));

		
		printf("g_conf_info mac :	");
		for (j=0;j<6;j++)
			printf("%02x ", g_conf_info.con_net.dev_mac[j]);
		printf("\n");
	}
	
	close(sockfd);
}

