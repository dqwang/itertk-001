#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "gpio.h"
#include "thread.h"
#include "def.h"
#include "client.h"
#include "log.h"

u8 g_alarm_led_flag = ALARM_LED_FLAG_OFF;
pthread_mutex_t gpio_mutex;

int set_gpio(gpio_num gn, gpio_dir gd, gpio_status gs)
{
	FILE *fp = NULL;
	char path[64] = "";

	pthread_mutex_lock(&gpio_mutex);
	
	fp = fopen("/sys/class/gpio/export","w");
	if (fp < 0)
		goto err;
	fprintf(fp, "%d", gn);
	fclose(fp);

	sprintf(path, "/sys/class/gpio/gpio%d/direction", gn);
	fp = fopen(path,"w");
	if (fp < 0)
		goto err;
	fprintf(fp, ((gd == GD_IN)? "in":"out"));
	fclose(fp);

	
	sprintf(path, "/sys/class/gpio/gpio%d/value", gn);
	fp = fopen(path, "w");
	if (fp < 0)
		goto err;
	fprintf(fp, "%d", gs);
	fclose(fp);	

	fp = fopen("/sys/class/gpio/unexport","w");
	if (fp < 0)
		goto err;
	fprintf(fp, "%d", gn);
	fclose(fp);
	
	pthread_mutex_unlock(&gpio_mutex);
	return 0;
err:
	printf(" set_gpio()  error!\n");
	pthread_mutex_unlock(&gpio_mutex);
	return -1;
}
int get_gpio(gpio_num gn, gpio_status *gsP)
{
	FILE *fp = NULL;
	char path[64] = "";
	int ret;
	
	pthread_mutex_lock(&gpio_mutex);
	if (NULL == gsP)
		goto err;
	
	fp = fopen("/sys/class/gpio/export","w");
	if (fp < 0)
		goto err;
	fprintf(fp, "%d", gn);
	fclose(fp);

	sprintf(path, "/sys/class/gpio/gpio%d/value", gn);
	fp = fopen(path, "r");
	if (fp < 0)
		goto err;
#define SIZE 1
#define CNT 1
	ret = fread(gsP, SIZE, CNT, fp);
	if (ret != CNT)
		goto err;
	*gsP -= 0x30;/*ascii to int*/
	fclose(fp);

	fp = fopen("/sys/class/gpio/unexport","w");
	if (fp < 0)
		goto err;
	fprintf(fp, "%d", gn);
	fclose(fp);
	pthread_mutex_unlock(&gpio_mutex);
	return 0;
err:
	printf(" get_gpio() args error!\n");
	pthread_mutex_unlock(&gpio_mutex);
	return -1;
}



void led_ctrl(gpio_num led, unsigned char on_off)
{	
	set_gpio(led, GD_OUT, on_off);	
}

void get_alarm(u8 alarm_in[])
{
	gpio_status alarm;

	memset(alarm_in, ALARM_OFF, sizeof(alarm_in));

	get_gpio(ALARM_IN1, &alarm);
	alarm_in[ALARM_SUFFIX0] = (u8) alarm;

	get_gpio(ALARM_IN2, &alarm);
	alarm_in[ALARM_SUFFIX1] = (u8) alarm;

	get_gpio(ALARM_IN3, &alarm);
	alarm_in[ALARM_SUFFIX2] = (u8) alarm;

	get_gpio(ALARM_IN4, &alarm);
	alarm_in[ALARM_SUFFIX3] = (u8) alarm;

	get_gpio(ALARM_IN5, &alarm);
	alarm_in[ALARM_SUFFIX4] = (u8) alarm;

	get_gpio(ALARM_IN6, &alarm);
	alarm_in[ALARM_SUFFIX5] = (u8) alarm;

	get_gpio(ALARM_IN7, &alarm);
	alarm_in[ALARM_SUFFIX6] = (u8) alarm;

	get_gpio(ALARM_IN8, &alarm);
	alarm_in[ALARM_SUFFIX7] = (u8) alarm;	
	
}

u8 is_alarm(u8 alarm_in[])
{
	u8 i=0;
	u8 alarm_state = ALARM_OFF;

	memset(alarm_in, ALARM_OFF, sizeof(alarm_in));
	get_alarm(alarm_in);
		
	for (i=0; i<ALARM_MAX;i++){
		if (alarm_in[i] == ALARM_ON){			
			alarm_state = ALARM_ON;
			led_ctrl(LED_D2_ALARM_STATUS, LED_ON);			
			g_alarm_led_flag = ALARM_LED_FLAG_ON;
			return alarm_state;
		}
	}
	
	if (g_alarm_led_flag == ALARM_LED_FLAG_ON){
		led_ctrl(LED_D2_ALARM_STATUS, LED_OFF);
		g_alarm_led_flag = ALARM_LED_FLAG_OFF;
	}	
	return alarm_state;
}

extern int g_sockfd_client;
extern int g_sockfd_client_status;
extern u8 buf_send[1024];
extern int make_ack_get_alarm_status(u8 * _buf, u8 alarm_in[]);

void alarm_proc(void)
{
	u8 alarm_in[ALARM_MAX];
	u8 num_to_send=0;	
	while (1)
	{
		if (ALARM_ON == is_alarm(alarm_in)){			
			if (SOCKFD_CLIENT_OK == g_sockfd_client_status){
				write(g_sockfd_client, buf_send, num_to_send);
				num_to_send = make_ack_get_alarm_status(buf_send, alarm_in);
			}
		}		
		usleep(100*1000);
	}
}

void system_run_proc(void)
{
	
	while (1)
	{
		led_ctrl(LED_D1_SYSTEM_STATUS, LED_ON);
		usleep(500*1000);
		led_ctrl(LED_D1_SYSTEM_STATUS, LED_OFF);		
		usleep(500*1000);
	}
	
}


void cfg_key_proc(void)
{
   
	gpio_status cfg_key_val;

	sys_log(FUNC,LOG_WARN, "%s","start");
	while (1)
	{
		get_gpio(CFG_KEY, &cfg_key_val);
		if (GS_LOW == cfg_key_val){
			sys_log(FUNC,LOG_WARN, "%s","3S 2S 1S....");
			sleep(3);
			get_gpio(CFG_KEY, &cfg_key_val);
			if (GS_LOW == cfg_key_val){
			
				sys_log(FUNC,LOG_WARN, "%s","Recovery default config ! reboot now");
				
				system("rm -f /mnt/nand1-2/data/SENSER.cfg");
				system("reboot");
			}
		}
		usleep(100*1000);
	}
}

int init_gpio(void)
{
	TRD_t gpio_trd;

	pthread_mutex_init (&gpio_mutex, NULL);

	
	set_gpio(ALARM_IN1, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN2, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN3, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN4, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN5, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN6, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN7, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN8, GD_IN, GS_HIGH);

	set_gpio(PHY_RESET,  GD_OUT,  GS_HIGH);	

	led_ctrl(LED_D1_SYSTEM_STATUS, LED_OFF);
	led_ctrl(LED_D2_ALARM_STATUS, LED_OFF);
	led_ctrl(LED_D3_ALARM_SERVER_STATUS, LED_OFF);

   	set_gpio(CFG_KEY, GD_IN, GS_HIGH);
   
	//trd_create(&gpio_trd, (void*)&alarm_proc, NULL);
	trd_create(&gpio_trd, (void*)&system_run_proc, NULL);
	trd_create(&gpio_trd, (void*)&cfg_key_proc, NULL);
	
	
	return 0;
}





