#include "com.h"
#include "config.h"
#include "mode.h"
#include "thread.h"
#include "log.h"


static int gcomfd[MAX_COM_PORT] = {0};
BYTE  rs_type = RS232;//default
pthread_mutex_t rs485_mutex;		/*rs485半双工锁*/

static void com_dev_open(void)
{
    int i=0;
    char name[32] = "";

    FOR(i, MAX_COM_PORT)
    {
        sprintf(name, "%s%d", TTY_DEV, i);
         gcomfd[i] =  open(name,O_RDWR|O_NONBLOCK|O_NOCTTY);
        printf("name is [%s], gcomfd[%d] = %d\n", name, i, gcomfd[i]);
        if(gcomfd[i]>0)
        {
        	fcntl(gcomfd[i],F_SETFL,0);//add by dege	
        }
    }
}

//////////////////////////////////////////////////////////////////////////
///
///     设置串口波特率
///     @param fd 串口文件描述符
///     @param speed 波特率
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-11
//////////////////////////////////////////////////////////////////////////
#if 0
static int setTTYSpeed(int i, int fd, int speed)
{
    if (fd <= 0)
        return -1;
    char val[2] = {i+0x20, 1};
    struct termios ntm;
    bzero(&ntm, sizeof(ntm));
    tcgetattr(fd, &ntm);
    printf("fd = %d, speed is %d\n", fd, speed);
    switch (speed)
    {
    case 1200:
        //val[1] = 191;
	cfsetispeed( &ntm, B1200 );
	cfsetospeed( &ntm, B1200 );
        break;
    case 2400:
        //val[1] = 95;
	cfsetispeed( &ntm, B2400 );
	cfsetospeed( &ntm, B2400 );
        break;
    case 4800:
        //val[1] = 47;
	cfsetispeed( &ntm, B4800 );
	cfsetospeed( &ntm, B4800 );
        break;
    case 9600:
        //val[1] = 23;
	cfsetispeed( &ntm, B9600 );
	cfsetospeed( &ntm, B9600 );
        break;
    case 19200:
        //val[1] = 11;
	cfsetispeed( &ntm, B19200 );
	cfsetospeed( &ntm, B19200 );
        break;
    case 38400:
        //val[1] = 5;
	cfsetispeed( &ntm, B38400 );
	cfsetospeed( &ntm, B38400 );
        break;
    case 57600:
        //val[1] = 3;
	cfsetispeed( &ntm, B57600 );
	cfsetospeed( &ntm, B57600 );
        break;
    case 115200:
        //val[1] = 1;
	cfsetispeed( &ntm, B115200 );
	cfsetospeed( &ntm, B115200 );
        break;
    default:
        //val[1] = 1;
        break;
    }
    int ret = tcsetattr(fd, TCSANOW, &ntm);
	printf("ret = %d\n", ret);
	tcflush( fd, TCIOFLUSH );
   // ioctl(fd, 0x02, val);
    return 1;
}

#endif

//////////////////////////////////////////////////////////////////////////
///
///     设置串口波特率
///     @param fd 串口文件描述符
///     @param databits 数据位
///     @param parity 校验位
///     @param stopbits 停止位
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-11
//////////////////////////////////////////////////////////////////////////
static int setTTY(int i, int fd, int speed,int databits, int parity, int stopbits)
{
	if (fd <= 0)
		return -1;

	printf("in %s,i=%d,fd=%d,speed=%d,databits=%d,parity=%d,stopbits=%d\n", \
		__FUNCTION__,i,  fd,  speed, databits,  parity,  stopbits);
	
	struct termios ntm;
	bzero(&ntm, sizeof(ntm));
	tcgetattr(fd, &ntm);
	ntm.c_cflag &= ~CSIZE;

	switch (speed)
	{
		case 300:
			cfsetispeed( &ntm, B300 );
			cfsetospeed( &ntm, B300 );
		break;   
		
		case 600:
			cfsetispeed( &ntm, B600 );
			cfsetospeed( &ntm, B600 );
		break;
		
		case 1200:
			cfsetispeed( &ntm, B1200 );
			cfsetospeed( &ntm, B1200 );
		break;
		
		case 2400:
			cfsetispeed( &ntm, B2400 );
			cfsetospeed( &ntm, B2400 );
		break;
		
		case 4800:
			cfsetispeed( &ntm, B4800 );
			cfsetospeed( &ntm, B4800 );
		break;
		
		case 9600:
			cfsetispeed( &ntm, B9600 );
			cfsetospeed( &ntm, B9600 );
		break;
		
		case 19200:
			cfsetispeed( &ntm, B19200 );
			cfsetospeed( &ntm, B19200 );
		break;
		
		case 38400:
			cfsetispeed( &ntm, B38400 );
			cfsetospeed( &ntm, B38400 );
		break;
		
		case 57600:
			cfsetispeed( &ntm, B57600 );
			cfsetospeed( &ntm, B57600 );
		break;
		
		case 115200:
			cfsetispeed( &ntm, B115200 );
			cfsetospeed( &ntm, B115200 );
		break;
		
		case 230400:
			cfsetispeed( &ntm, B230400 );
			cfsetospeed( &ntm, B230400 );
		break;	
		
		case 460800:
			cfsetispeed( &ntm, B460800 );
			cfsetospeed( &ntm, B460800 );
		break;
		
		case 576000:
			cfsetispeed( &ntm, B576000 );
			cfsetospeed( &ntm, B576000 );
		break;
		
		case 921600:
			cfsetispeed( &ntm, B921600 );
			cfsetospeed( &ntm, B921600 );
		break;
		
		default:
			cfsetispeed( &ntm, B115200 );
			cfsetospeed( &ntm, B115200 );
		break;
	}
	switch (databits)
	{
		case 5:
			ntm.c_cflag |= CS5;
		break;
		
		case 6:
			ntm.c_cflag |= CS6;
		break;
		
		case 7:
			ntm.c_cflag |= CS7;
		break;
		
		case 8:
			ntm.c_cflag |= CS8;
		break;

		default:
			printf("com_params_set: data bits value[%d] is illegal!\n", databits);
		return -1;
	}

	switch (parity)
	{
		case 3:
			ntm.c_cflag &= ~PARENB;
			ntm.c_cflag &= ~PARODD;
			ntm.c_iflag &= ~(INPCK | ISTRIP);
		break;
		
		case 2://奇
			ntm.c_cflag  |= PARENB;
			ntm.c_cflag  |= PARODD;
			ntm.c_iflag  |=(INPCK | ISTRIP);
		break;
		
		case 1://偶
			ntm.c_iflag  |=(INPCK | ISTRIP);
			ntm.c_cflag  |=PARENB;
			ntm.c_cflag  &=~ PARODD;
		break;
		
		default:
			printf("com_params_set: parity value[%d] is illegal!\n", parity);
		return -2;
	}

	//set stop bits
	switch (stopbits)
	{
		case 1:
			ntm.c_cflag &=~CSTOPB;
		break;
		
		case 2:
			ntm.c_cflag |=CSTOPB;
		break;
		
		default:
			printf("com_params_set: stop bits value[%d] is illegal!\n", stopbits);
		return -3;
	}
	ntm.c_cflag |= CLOCAL | CREAD;    
	ntm.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	ntm.c_oflag &= ~OPOST;
	ntm.c_cc[VTIME]=0;
	ntm.c_cc[VMIN]=0;
	tcflush(fd, TCIOFLUSH);

	if (tcsetattr(fd, TCSANOW, &ntm) != 0)
	{
		perror("com_params_set: tcsetattr");
		return -4;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
///
///     串口参数设置
///     @param fd 串口文件描述符
///     @param con_com 参数结构体
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
void com_para_set(int fd, CONFIG_COM *con_com)
{
	if(fd > 0)
	{
		setTTY(con_com->id - 1, fd, con_com->bps,con_com->dbit, con_com->chk, con_com->sbit);
		rs_type = con_com->type;

		sys_log(FUNC, LOG_DBG,"UART fd =%d, rs_type=%d",fd, rs_type);
		
	}
}

//////////////////////////////////////////////////////////////////////////
///
///     串口收数据线程
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
#define COM_RECV_BUF_MAX (1024*128L)

static void com_proc(void* arg)
{
	CONFIG_COM *con_com = (CONFIG_COM*)arg;	
	char lsbuf[COM_RECV_BUF_MAX];
	int i=0;
	int len;
	sys_log(FUNC, LOG_MSG, "start fd=%d", gcomfd[con_com->id - 1]);
	
	while(1){
				
		if (rs_type  == RS232){			
			len = read(gcomfd[con_com->id - 1], lsbuf, COM_RECV_BUF_MAX);				
			if(len > 0)	{
				#if 0//just for testing uart
				write(gcomfd[con_com->id - 1], lsbuf, len);
				#endif				
				SendComDataToNet(con_com->id - 1, lsbuf, len);
			}
			
		}else if (rs_type == RS485){

			/*LOCK*/
			pthread_mutex_lock(&rs485_mutex);
			
			len = read(gcomfd[con_com->id - 1], lsbuf, COM_RECV_BUF_MAX);
			if(len > 0)	{
				//printf("i = %d,len = %d\n", con_com->id, len);
				SendComDataToNet(con_com->id - 1, lsbuf, len);
			}

			/*UNLOCK*/
			pthread_mutex_unlock(&rs485_mutex);
		}
		usleep(1000);
		
	}


}

//////////////////////////////////////////////////////////////////////////
///
///     串口初始化
///     @param argc 参数个数
///     @param *argv[] 参数内容
///     @return always 0
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
void com_init(void)
{
    TRD_t com_trd[MAX_COM_PORT];
    int i;
    CONFIG_COM *con_com = g_conf_info.con_com;
    com_dev_open();

    pthread_mutex_init (&rs485_mutex, NULL);

    printf("rs485_mutex init\n");
	

    FOR(i, MAX_COM_PORT)
    {
        com_para_set(gcomfd[i], &con_com[i]);
    }	
    FOR(i, MAX_COM_PORT)
        trd_create(&com_trd[i], (void*)&com_proc, &con_com[i]);
}

void com_set(CONFIG_COM *con_com)
{
    
    com_para_set(gcomfd[con_com->id - 1], con_com);
}

//////////////////////////////////////////////////////////////////////////
///
///     串口数据发送
///     @param id 串口序号
///     @param *data 数据首地址
///     @param len 数据长度
///     @author     xuliang<gxuliang@gmail.com>
///     @date       2010-05-05
//////////////////////////////////////////////////////////////////////////
inline int SendNetDataToCom(int id, void *data, int len)
{		
	if (rs_type == RS232)
		return writen(gcomfd[id], data, len);
	else if (rs_type == RS485) 
	 	return writen_rs485(gcomfd[id], data, len);	
	
	return -1;
	
}


