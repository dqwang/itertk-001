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
	
	int i;
	
	for(i = 0; i < sizeof(sbit)/sizeof(sbit[0]); i++)
	{
		if(sbit[i] == con_gpio.alarm[num-1])
			fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"alarm\"  type=\"text\" value=\"%d\" size=\"30\" readonly=\"true\"/></td>\n",sbit[i]);
		
	}
	

	
	
}

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
		fprintf(cgiOut, "window.alert(\"ÓÃ»§ÉÐÎ´µÇÂ¼£¬ÇëÖØÐÂµÇÂ¼(´íÎóÂë£º%d)£¡\");\n", ret);
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
	fprintf(cgiOut, "<HEAD><meta http-equiv=\"refresh\" content=\"1\">\n");
	fprintf(cgiOut,	"</HEAD>\n");
	fprintf(cgiOut, "<body>\n");
	
	if(ret != QUERY_OK)
	{
		fprintf(cgiOut, "<script type=\"text/javascript\">\n");
		fprintf(cgiOut, "window.alert(\"(´íÎóÂë£º%d)£¡\");\n", ret);
		fprintf(cgiOut, "</script>\n");		
	}
	else if(cgiFormSubmitClicked("com") == cgiFormSuccess)
	{
		
		
	}
	
	
	fprintf(cgiOut, "<table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "  <tr>\n");
	fprintf(cgiOut, "    <td width=\"17\" height=\"29\" valign=\"top\" background=\"../images/mail_leftbg.gif\"><img src=\"../images/left-top-right.gif\" width=\"17\" height=\"29\" /></td>\n");
	fprintf(cgiOut, "    <td width=\"935\" height=\"29\" valign=\"top\" background=\"../images/content-bg.gif\"><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"left_topbg\" id=\"table2\">\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td height=\"31\"><div class=\"titlebt\">´«¸ÐÆ÷²ÉÑù</div></td>\n");
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
	fprintf(cgiOut, "                <td class=\"left_bt2\">&nbsp;&nbsp;&nbsp;&nbsp;GPIO×´Ì¬</td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	fprintf(cgiOut, "              <form name=\"form1\" method=\"POST\" action=\"");
	fprintf(cgiOut, "\">\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td width=\"20%%\" height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">Alarm1£º</td>\n");
	fprintf(cgiOut, "                  <td width=\"3%%\" bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td width=\"3%%\" height=\"30\" bgcolor=\"#f2f2f2\"><label>\n");
	show_alarm(1);
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Alarm2£º</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_alarm(2);
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Alarm3£º</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_alarm(3);
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Alarm4£º</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_alarm(4);
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Alarm5£º</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_alarm(5);
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Alarm6£º</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_alarm(6);
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Alarm7£º</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_alarm(7);
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">Alarm8£º</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_alarm(8);


	
	fprintf(cgiOut, "</body>\n");
}


