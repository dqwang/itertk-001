#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"
#include "itscmd.h"

char name[32]="";
CONFIG_GPIO con_gpio;


int session_read(char* name)
{
	char lsbuf[32]="";
	int ret = 0;
	sprintf(lsbuf, "/tmp/nc_%s", cgiRemoteAddr);
	int fd = open(lsbuf, O_RDONLY); 
	if(fd <= 0)
		return 0;
	if(read(fd, name, 32) > 0)
		ret = 1;
	close(fd);
	return ret;
}
#if 0
void show_alarm(int num)
{
	int sbit[] = {
		0,
		1
	};
	
	int i;
	fprintf(cgiOut, "                    <select style=\"width:80px\">\n");
	for(i = 0; i < sizeof(sbit)/sizeof(sbit[0]); i++)
	{
		if(sbit[i] == con_gpio.alarm[num-1])
			fprintf(cgiOut, "                      <option selected=\"selected\">%d</option>\n", sbit[i]);
		else
			fprintf(cgiOut, "                      <option>%d</option>\n", sbit[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}
#else


void show_alarm(int num)
{	
	int sbit[] = {
		0,
		1
	};
	char *str[2] = {"开启","关闭"};
	int i;
	char alarm_str[16];
	
	sprintf(alarm_str, "alarm%d", num);
	fprintf(cgiOut, "                    <select name=\"%s\" style=\"width:80px\">\n", alarm_str);
	for(i = 0; i < sizeof(sbit)/sizeof(sbit[0]); i++)
	{
		
		if(sbit[i] == con_gpio.alarm_on_off[num-1]){
			if (1 == con_gpio.alarm_on_off[num-1])
				fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", str[0]);
			else
				fprintf(cgiOut, "                      <option>%s</option>\n", str[1]);
		}
		
	}
	fprintf(cgiOut, "                    </select>\n");

	
	
}

#endif

void show_output1(void)
{	

	char *str[2] = {"0","1"};
	int i;
	fprintf(cgiOut, "                    <select name=\"output1\" style=\"width:80px\">\n");
	for(i = 0; i < 2; i++)
	{
		if(i == con_gpio.output[0])
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", str[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", str[i]);
	}
	fprintf(cgiOut, "                    </select>\n");

}

void show_output2(void)
{	

	char *str[2] = {"0","1"};
	int i;
	fprintf(cgiOut, "                    <select name=\"output2\" style=\"width:80px\">\n");
	for(i = 0; i < 2; i++)
	{
		if(i == con_gpio.output[1])
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", str[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", str[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}

void show_output3(void)
{	

	char *str[2] = {"0","1"};
	int i;
	fprintf(cgiOut, "                    <select name=\"output3\" style=\"width:80px\">\n");
	for(i = 0; i < 2; i++)
	{
		if(i == con_gpio.output[2])
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", str[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", str[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}

void show_output4(void)
{	

	char *str[2] = {"0","1"};
	int i;
	fprintf(cgiOut, "                    <select name=\"output4\" style=\"width:80px\">\n");
	for(i = 0; i < 2; i++)
	{
		if(i == con_gpio.output[3])
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", str[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", str[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}



#if 0



#endif
int cgiMain()
{
	
	
	int ret = session_read(name);
	if(ret != 1)
	{
		cgiHeaderContentType("text/html;charset=gb2312");
		fprintf(cgiOut, "<HTML><HEAD>\n");
		fprintf(cgiOut,	"</HEAD><BODY>\n");
		fprintf(cgiOut, "<script type=\"text/javascript\">\n");
		fprintf(cgiOut, "window.alert(\"用户尚未登录，请重新登录(错误码：%d)！\");\n", ret);
		fprintf(cgiOut, "window.location.href=\"../login.html\";\n");
		fprintf(cgiOut, "</script>\n");
		fprintf(cgiOut, "</BODY>\n");
		fprintf(cgiOut, "</HTML>\n");
		return 0;
	}
	
	ret = its_conf_gpio_query(name, &con_gpio);
	
	
	
	cgiHeaderContentType("text/html;charset=gb2312");
	fprintf(cgiOut, "<style type=\"text/css\">\n");
	fprintf(cgiOut, "<!--\n");
	fprintf(cgiOut, "body {\n");
	fprintf(cgiOut, "	margin-left: 0px;\n");
	fprintf(cgiOut, "	margin-top: 0px;\n");
	fprintf(cgiOut, "	margin-right: 0px;\n");
	fprintf(cgiOut, "	margin-bottom: 0px;\n");
	fprintf(cgiOut, "	background-color: #F8F9FA;\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "-->\n");
	fprintf(cgiOut, "</style>\n");
	fprintf(cgiOut, "<link href=\"../images/skin.css\" rel=\"stylesheet\" type=\"text/css\" />\n");
	//fprintf(cgiOut, "<HEAD><meta http-equiv=\"refresh\" content=\"1\">\n");
	fprintf(cgiOut,	"</HEAD>\n");
	fprintf(cgiOut, "<body>\n");
	
	if(ret != QUERY_OK)
	{
		fprintf(cgiOut, "<script type=\"text/javascript\">\n");
		fprintf(cgiOut, "window.alert(\"(错误码：%d)！\");\n", ret);
		fprintf(cgiOut, "</script>\n");		
	}else if (cgiFormSubmitClicked("setting") == cgiFormSuccess){


		char *str1[2] = {"0","1"};
		int  i;/*must be int, fuck*/
		cgiFormSelectSingle("output1", str1, sizeof(str1)/sizeof(str1[0]), &i, 0);
		con_gpio.output[0]= i;

		cgiFormSelectSingle("output2", str1, sizeof(str1)/sizeof(str1[0]), &i, 0);
		con_gpio.output[1]= i;

		cgiFormSelectSingle("output3", str1, sizeof(str1)/sizeof(str1[0]), &i, 0);
		con_gpio.output[2]= i;

      cgiFormSelectSingle("output4", str1, sizeof(str1)/sizeof(str1[0]), &i, 0);
		con_gpio.output[3]= i;
      

		

#if 0
		its_conf_gpio_set(name,&con_gpio);
#else
		if (GPIO_SET_OK!=its_conf_gpio_set(name,&con_gpio)){
	
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交失败(错误码：%d)！\");\n", ret);
			fprintf(cgiOut, "</script>\n");
			
			//提交失败后需要重新获取参数
			bzero(&con_gpio, sizeof(con_gpio));
			its_conf_gpio_query(name, &con_gpio);
		}
		else
		{
			//fprintf(cgiOut, "[%s][%s][%s][%s]\n", con_sys.host_name, con_sys.host_pos, con_sys.host_id, con_sys.description);
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交成功！\");\n");
			fprintf(cgiOut, "</script>\n");
		}

#endif
	}
		
	
	
SHOW:	
	fprintf(cgiOut, "<table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "  <tr>\n");
	fprintf(cgiOut, "    <td width=\"17\" height=\"29\" valign=\"top\" background=\"../images/mail_leftbg.gif\"><img src=\"../images/left-top-right.gif\" width=\"17\" height=\"29\" /></td>\n");
	fprintf(cgiOut, "    <td width=\"935\" height=\"29\" valign=\"top\" background=\"../images/content-bg.gif\"><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"left_topbg\" id=\"table2\">\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td height=\"31\"><div class=\"titlebt\">传感器设置</div></td>\n");
	fprintf(cgiOut, "      </tr>\n");
	fprintf(cgiOut, "    </table></td>\n");
	fprintf(cgiOut, "    <td width=\"16\" valign=\"top\" background=\"../images/mail_rightbg.gif\"><img src=\"../images/nav-right-bg.gif\" width=\"16\" height=\"29\" /></td>\n");
	fprintf(cgiOut, "  </tr>\n");
	fprintf(cgiOut, "  <tr>\n");
	fprintf(cgiOut, "    <td height=\"71\" valign=\"middle\" background=\"../images/mail_leftbg.gif\">&nbsp;</td>\n");
	fprintf(cgiOut, "    <td valign=\"top\" bgcolor=\"#F7F8F9\"><table width=\"100%%\" height=\"138\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td height=\"13\" valign=\"top\">&nbsp;</td>\n");
	fprintf(cgiOut, "      </tr>\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td valign=\"top\"><table width=\"98%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td class=\"left_txt\"></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td height=\"20\"><table width=\"100%%\" height=\"1\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#CCCCCC\">\n");
	fprintf(cgiOut, "              <tr>\n");
	fprintf(cgiOut, "                <td></td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" height=\"55\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "              <tr>\n");
	fprintf(cgiOut, "                <td width=\"10%%\" height=\"55\" valign=\"middle\"><img src=\"../images/title.gif\" width=\"54\" height=\"55\"></td>\n");
	fprintf(cgiOut, "                <td width=\"90%%\" valign=\"top\"><span class=\"left_txt2\"></span><span class=\"left_txt3\"></span><span class=\"left_txt2\"></span><br>\n");
	fprintf(cgiOut, "                          <span class=\"left_txt2\"></span><span class=\"left_txt3\"></span><span class=\"left_txt2\"></span><span class=\"left_txt3\"></span><span class=\"left_txt2\"> </span></td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td>&nbsp;</td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"nowtable\">\n");
	fprintf(cgiOut, "              <tr>\n");
	fprintf(cgiOut, "                <td class=\"left_bt2\">&nbsp;&nbsp;&nbsp;&nbsp;报警信号输入状态</td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	fprintf(cgiOut, "              <form name=\"form1\" method=\"POST\" action=\"");
	fprintf(cgiOut, "\">\n");
	

	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">Alarm1</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	
	show_alarm(1);
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">Alarm2</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_alarm(2);
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">Alarm3</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_alarm(3);
   /*
   fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">Alarm4</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_alarm(4);
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">Alarm5</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_alarm(5);
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">Alarm6</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_alarm(6);
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">Alarm7</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_alarm(7);
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">Alarm8</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_alarm(8);
   */

	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Output1：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_output1();

	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Output2：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_output2();

	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Output3：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_output3();

   fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Output4：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_output4();

	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" align=\"center\" class=\"left_txt2\"><input type=\"submit\" value=\"设置\" name=\"setting\" /></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	
	fprintf(cgiOut, "</body>\n");
}


