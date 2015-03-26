#include <stdio.h>
#include <stdlib.h>
#include "def.h"
#include "system.h"
#include "config.h"
#include "log.h"
#include "web_net.h"
#include "com.h"
#include "mode.h"
#include "gpio.h"
#include "client.h"

int main ( int argc, char **argv )
{
	sys_version_show ( stdout );
	config_init();
	web_init();
	mode_init();
	report_dev_info_init();
	com_init();
	init_gpio();
	
	client_init();
	client_thread();
	
	system("telnetd");

	led_ctrl(LED_D1_SYSTEM_STATUS, LED_ON);

	//dns_init(g_conf_info.con_server.dns_str);

	while(1)
	{		
		web_process();		
	}
	return 0;
}
