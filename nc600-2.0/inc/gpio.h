#ifndef _GPIO_H_
#define _GPIO_H_
#include "def.h"
typedef enum GPIO_NUM{
	ALARM_IN1 =64, //GPC0
	ALARM_IN2 =38,//GPB6
	ALARM_IN3 =37, //GPB5
	ALARM_IN4 =65,//GPC1
	ALARM_IN5= 66,//GPC2
	ALARM_IN6=67,//GPC3
	ALARM_IN7=32, //GPB0
	ALARM_IN8=33, //GPB1

	CFG_KEY = 40,//GPB8

	LED_D1_SYSTEM_STATUS=36,//GPB4
	LED_D2_ALARM_STATUS=35,//GPB3
	LED_D3_ALARM_SERVER_STATUS=34,//GPB2

	PHY_RESET =167 //GPG7   	

}gpio_num;

typedef enum GPIO_STATUS{
	GS_LOW =0,
	GS_HIGH	
}gpio_status;

typedef enum GPIO_DIRECTION{
	GD_IN = 0,
	GD_OUT
}gpio_dir;



int set_gpio(gpio_num gn, gpio_dir gd, gpio_status gs);
int get_gpio(gpio_num gn, gpio_status *gsP);
int init_gpio(void);
void get_alarm(u8 alarm_in[]);


#define LED_ON GS_LOW
#define LED_OFF GS_HIGH
void led_ctrl(gpio_num led, unsigned char on_off);

#define ALARM_ON 1 /**/
#define ALARM_OFF 0

#define ALARM_LED_FLAG_ON 1
#define ALARM_LED_FLAG_OFF 0

enum {
	ALARM_SUFFIX0 = 0,
	ALARM_SUFFIX1,
	ALARM_SUFFIX2,
	ALARM_SUFFIX3,
	ALARM_SUFFIX4,
	ALARM_SUFFIX5,
	ALARM_SUFFIX6,
	ALARM_SUFFIX7,

	ALARM_MAX = 8
	
};

#define ALARM_DURATION 3



#endif/*_GPIO_H_*/
