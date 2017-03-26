#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "gpio.h"
#include "thread.h"
#include "def.h"
#include "client.h"
#include "log.h"
#include "config.h"

u8 g_alarm_led_flag = ALARM_LED_FLAG_OFF;
pthread_mutex_t gpio_mutex;
u8 g_alarm_in[ALARM_MAX];

extern pthread_t thread_id[MAX_THREAD_ID];


#define GPIO_FLAG 1

#if GPIO_FLAG
#define GPIO_TOTAL 18
unsigned char g_gpio_num[GPIO_TOTAL]={ALARM_IN1, ALARM_IN2, ALARM_IN3, ALARM_IN4, ALARM_IN5, ALARM_IN6,\
					     ALARM_IN7, ALARM_IN8, CFG_KEY, LED_D1_SYSTEM_STATUS, LED_D2_ALARM_STATUS, \
					     LED_D3_ALARM_SERVER_STATUS, PHY_RESET, OUTPUT_1, OUTPUT_2, OUTPUT_3,OUTPUT_4, TEMP_SENSOR_POWER};
unsigned char g_gpio_dir[GPIO_TOTAL]={GD_IN, GD_IN, GD_IN, GD_IN, GD_IN, GD_IN, GD_IN, GD_IN, \
						                   GD_IN, GD_OUT,GD_OUT,GD_OUT,GD_OUT,GD_OUT,GD_OUT,GD_OUT, GD_OUT,GD_OUT};

unsigned char g_gpio_status[GPIO_TOTAL]={GS_HIGH, GS_HIGH, GS_HIGH,GS_HIGH,GS_HIGH,GS_HIGH,\
								        GS_HIGH,GS_HIGH,GS_HIGH,GS_HIGH,GS_HIGH,GS_HIGH,GS_HIGH, GS_HIGH,GS_HIGH,GS_HIGH,GS_HIGH, GS_HIGH};
int open_gpio(void)
{
	FILE *fp = NULL;
	char path[64] = "";
	unsigned char i=0;

	sys_log(FUNC, LOG_MSG, "start");
	
	pthread_mutex_lock(&gpio_mutex);
	
	for (i=0; i<GPIO_TOTAL; i++){
		fp = fopen("/sys/class/gpio/export","w");
		if (NULL == fp)
			goto err;
		fprintf(fp, "%d", g_gpio_num[i]);
		fclose(fp);
	}
	
	for (i=0; i<GPIO_TOTAL; i++){
		sprintf(path, "/sys/class/gpio/gpio%d/direction", g_gpio_num[i]);
		fp = fopen(path,"w");
		if (NULL == fp)
			goto err;
		fprintf(fp, ((g_gpio_dir[i]== GD_IN)? "in":"out"));	
		fclose(fp);
	}
	sys_log(FUNC, LOG_MSG, "ok");
	pthread_mutex_unlock(&gpio_mutex);
	return 0;
err:
	printf(" open_gpio()  error!\n");
	pthread_mutex_unlock(&gpio_mutex);
	return -1;	
}
int close_gpio(void)
{
	FILE *fp = NULL;
	unsigned char i=0;

	
	pthread_mutex_lock(&gpio_mutex);
	
	for (i=0;i<GPIO_TOTAL;i++){
		fp = fopen("/sys/class/gpio/unexport","w");
		if (NULL == fp)
			goto err;
		fprintf(fp, "%d", g_gpio_num[i]);
		fclose(fp);
	}	
	pthread_mutex_unlock(&gpio_mutex);
err:
	printf(" close_gpio()  error!\n");
	pthread_mutex_unlock(&gpio_mutex);
	return -1;	
}

int set_gpio(gpio_num gn, gpio_dir gd, gpio_status gs)
{
	FILE *fp = NULL;
	char path[64] = "";

	pthread_mutex_lock(&gpio_mutex);
		
	sprintf(path, "/sys/class/gpio/gpio%d/value", gn);
	fp = fopen(path, "w");
	if (NULL == fp)
		goto err;
	fprintf(fp, "%d", gs);
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

	sprintf(path, "/sys/class/gpio/gpio%d/value", gn);
	fp = fopen(path, "r");
	if (NULL == fp)
		goto err;
#define SIZE 1
#define CNT 1
	ret = fread(gsP, SIZE, CNT, fp);
	if (ret != CNT)
		goto err;
	*gsP -= 0x30;/*ascii to int*/
	fclose(fp);
	
	pthread_mutex_unlock(&gpio_mutex);
	return 0;
err:
	printf(" get_gpio() args error!\n");
	if (NULL != fp){
		fclose(fp);
	}
	pthread_mutex_unlock(&gpio_mutex);
	return -1;
}

#else

int set_gpio(gpio_num gn, gpio_dir gd, gpio_status gs)
{
	FILE *fp = NULL;
	char path[64] = "";

	pthread_mutex_lock(&gpio_mutex);
	
	fp = fopen("/sys/class/gpio/export","w");
	if (NULL == fp)
		goto err;
	fprintf(fp, "%d", gn);
	fclose(fp);

	sprintf(path, "/sys/class/gpio/gpio%d/direction", gn);
	fp = fopen(path,"w");
	if (NULL == fp)
		goto err;
	fprintf(fp, ((gd == GD_IN)? "in":"out"));
	fclose(fp);

	
	sprintf(path, "/sys/class/gpio/gpio%d/value", gn);
	fp = fopen(path, "w");
	if (NULL == fp)
		goto err;
	fprintf(fp, "%d", gs);
	fclose(fp);	

	fp = fopen("/sys/class/gpio/unexport","w");
	if (NULL == fp)
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
	if (NULL == fp)
		goto err;
	fprintf(fp, "%d", gn);
	fclose(fp);

	sprintf(path, "/sys/class/gpio/gpio%d/value", gn);
	fp = fopen(path, "r");
	if (NULL == fp)
		goto err;
#define SIZE 1
#define CNT 1
	ret = fread(gsP, SIZE, CNT, fp);
	if (ret != CNT)
		goto err;
	*gsP -= 0x30;/*ascii to int*/
	fclose(fp);

	fp = fopen("/sys/class/gpio/unexport","w");
	if (NULL == fp)
		goto err;
	fprintf(fp, "%d", gn);
	fclose(fp);
	pthread_mutex_unlock(&gpio_mutex);
	return 0;
err:
	printf(" get_gpio() args error!\n");
	if (NULL != fp){
		fclose(fp);
	}
	pthread_mutex_unlock(&gpio_mutex);
	return -1;
}

#endif

void led_ctrl(gpio_num led, unsigned char on_off)
{	
	set_gpio(led, GD_OUT, on_off);	
}

void init_alarm(void)
{
	memset(g_alarm_in, ALARM_OFF, sizeof(g_alarm_in));

	set_gpio(ALARM_IN1, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN2, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN3, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN4, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN5, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN6, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN7, GD_IN, GS_HIGH);
	set_gpio(ALARM_IN8, GD_IN, GS_HIGH);
}

#if 0
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
#else
void get_alarm(u8 alarm_in[])
{
	gpio_status alarm;
	
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

#endif

#define ALARM_CYCLE 100//ms
#define ALARM_OFF_TIMEOUT 300/*30s*/
#if 1
void alarm_proc(void)
{

	u8 time[ALARM_MAX];
	u8 alarm_init[ALARM_MAX] = {0};
	u8 alarm_update[ALARM_MAX]={0};
	u8 i=0;
	u32 alarm_off_count[ALARM_MAX]={0};

	memset(time, 0, sizeof(time));
	get_alarm(alarm_init);	
	usleep(ALARM_CYCLE*1000);
	while (1)
	{
		get_alarm(alarm_update);
				
		for (i=0;i<ALARM_MAX;i++){			
			if (alarm_update[i] != alarm_init[i]){
				g_alarm_in[i] = ALARM_ON;
				alarm_off_count[i]=0;
			}else{
				alarm_off_count[i]++;
			}
			if (g_alarm_in[i] == ALARM_ON){
				if (alarm_off_count[i]>=ALARM_OFF_TIMEOUT){
					g_alarm_in[i] = ALARM_OFF;
				}
			}	
			//printf("%d ", g_alarm_in[i]);
		}
		//printf("\n");

		//get_alarm(alarm_init);
		memcpy(alarm_init, alarm_update, sizeof(alarm_update));
		usleep(ALARM_CYCLE*1000);
	}
}

#else
void alarm_proc(void)
{
	u8 t=0;
	u8 time[ALARM_MAX]=0;
	u8 alarm_init[ALARM_MAX];
	u8 alarm_update[ALARM_MAX];
	u8 i;
	
		
	while (1)
	{
		get_alarm(alarm_init);
		t=0;
		memset(g_alarm_in, ALARM_OFF, sizeof(g_alarm_in));

		while (t<ALARM_TIME){
			get_alarm(alarm_update);
			for (i=0;i<ALARM_MAX;i++){			
				if (alarm_update[i] != alarm_init[i]){
					g_alarm_in[i] = ALARM_ON;				
				}
				t++;
				usleep(100*1000);
					
			}	
			
		}		
	}
	
	
}
#endif
void system_run_proc(void)
{
	sys_log(FUNC, LOG_MSG, "start");
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

	sys_log(FUNC, LOG_MSG, "start");
	while (1)
	{
		get_gpio(CFG_KEY, &cfg_key_val);
		if (GS_LOW == cfg_key_val){
			sys_log(FUNC,LOG_WARN, "%s","3S 2S 1S....");
			sleep(3);
			get_gpio(CFG_KEY, &cfg_key_val);
			if (GS_LOW == cfg_key_val){
			
				sys_log(FUNC,LOG_WARN, "%s","Recovery default config ! reboot now");
				
				reconfig(&g_conf_info);
				
			}
		}
		usleep(100*1000);
	}
}

int init_gpio(void)
{
	TRD_t gpio_trd;

	pthread_mutex_init (&gpio_mutex, NULL);

#if GPIO_FLAG	
 	open_gpio();
#endif	

	init_alarm();	

 	set_gpio(PHY_RESET,  GD_OUT,  GS_HIGH);	

	led_ctrl(LED_D1_SYSTEM_STATUS, LED_OFF);
	led_ctrl(LED_D2_ALARM_STATUS, LED_OFF);
	led_ctrl(LED_D3_ALARM_SERVER_STATUS, LED_OFF);

   	set_gpio(CFG_KEY, GD_IN, GS_HIGH);

	set_gpio(OUTPUT_1, GD_OUT,GS_LOW);
	set_gpio(OUTPUT_2, GD_OUT,GS_LOW);
	set_gpio(OUTPUT_3, GD_OUT,GS_LOW);
    set_gpio(OUTPUT_4, GD_OUT,GS_LOW);

    set_gpio(TEMP_SENSOR_POWER, GD_OUT, GS_HIGH);

	trd_create(&gpio_trd, (void*)&alarm_proc, NULL);
	thread_id[0] = gpio_trd;
	trd_create(&gpio_trd, (void*)&system_run_proc, NULL);
	thread_id[1] = gpio_trd;
	trd_create(&gpio_trd, (void*)&cfg_key_proc, NULL);
	thread_id[2] = gpio_trd;
	
	return 0;
}

#define SENSOR_PROC "/proc/am2301"
void get_sensor(char *p)
{
	int ret=-1;
	int fd;
	
	fd=open(SENSOR_PROC, O_RDONLY, NULL);
	if (fd <0){
		sys_log(FUNC, LOG_ERR, "%s","open");
	}

	ret=read(fd, p, 30);
	
	if (ret <0){
		sys_log(FUNC, LOG_ERR, "%s","read");
	}else{
		sys_log(FUNC, LOG_DBG, "%s",p);
	}
	
	close(fd);
}



