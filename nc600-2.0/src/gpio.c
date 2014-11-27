#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "gpio.h"
#include "thread.h"



int set_gpio(gpio_num gn, gpio_dir gd, gpio_status gs)
{
	FILE *fp = NULL;
	char path[64] = "";
	
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
	
	return 0;
err:
	printf(" set_gpio()  error!\n");
	return -1;
}
int get_gpio(gpio_num gn, gpio_status *gsP)
{
	FILE *fp = NULL;
	char path[64] = "";
	int ret;
	
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
	
	return 0;
err:
	printf(" get_gpio() args error!\n");
	return -1;
}

void gpio_proc(void)
{
	gpio_status gs=GS_HIGH;

	while (1)
	{
		get_gpio(ALARM_TTLIN1, &gs);
		//printf("gs =%d \n", gs);

		usleep(100);
	}
}

int init_gpio(void)
{
	TRD_t gpio_trd;
	
	set_gpio(ALARM_TTLIN1, GD_IN, GS_HIGH);
	set_gpio(ALARM_TTLIN2, GD_IN, GS_HIGH);
	//set_gpio(ALARM_O, GD_OUT, GS_HIGH);	

	// trd_create(&gpio_trd, (void*)&gpio_proc, NULL);
}

