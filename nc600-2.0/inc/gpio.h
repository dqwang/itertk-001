#ifndef _GPIO_H_
#define _GPIO_H_

typedef enum GPIO_NUM{
	ALARM_O = 99,//GPD3
	//ALARM_TTLIN1 = 163,//GPG3
	//ALARM_TTLIN2 = 162//GPG2

	ALARM_TTLIN1 = 38,//GPB6
	ALARM_TTLIN2 = 37,//GPB5

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

	


#endif/*_GPIO_H_*/
