#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"
#include "itscmd.h"


CONFIG_SYS con_sys;
char name[32] = "";

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

void show_host_name(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_name\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", con_sys.host_name);
}

void show_host_pos(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_pos\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", con_sys.host_pos);
}

void show_host_sn(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_sn\" type=\"text\" value=\"%s\" size=\"30\" readonly=\"true\"/></td>\n", con_sys.host_sn);
}

void show_sw_ver(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"sw_ver\" type=\"text\" value=\"%s\" size=\"30\" readonly=\"true\"/></td>\n", con_sys.sw_ver);
}


void show_hw_ver(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"web_ver\" type=\"text\" value=\"%s\" size=\"30\" readonly=\"true\"/></td>\n", con_sys.hw_ver);
}

void show_host_id(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_id\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", con_sys.host_id);
}

void show_description(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"description\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", con_sys.description);
}

void get_host(char* str, char* dp)
{
	bzero(dp, 32);
	cgiFormStringNoNewlines(str, dp, 30);
}

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
	
	
	ret = its_sysinfo_query(name, &con_sys);
	
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
	fprintf(cgiOut, "<body>\n");
	
	if(ret != QUERY_OK)
	{
		fprintf(cgiOut, "<script type=\"text/javascript\">\n");
		fprintf(cgiOut, "window.alert(\"(错误码：%d)！\");\n", ret);
		fprintf(cgiOut, "</script>\n");		
	}
	else if(cgiFormSubmitClicked("host") == cgiFormSuccess)
	{
		get_host("host_name", con_sys.host_name);
		get_host("host_pos", con_sys.host_pos);
		get_host("host_id", con_sys.host_id);
		get_host("description", con_sys.description);
		
		ret = its_sysinfo_set(name, &con_sys);
		if(ret != SYS_SET_OK)
		{
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交失败(错误码：%d)！\");\n", ret);
			fprintf(cgiOut, "</script>\n");
			
			//提交失败后需要重新获取参数
			bzero(&con_sys, sizeof(con_sys));
			its_sysinfo_query(name, &con_sys);
		}
		else
		{
			//fprintf(cgiOut, "[%s][%s][%s][%s]\n", con_sys.host_name, con_sys.host_pos, con_sys.host_id, con_sys.description);
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交成功！\");\n");
			fprintf(cgiOut, "</script>\n");
		}
	}
	fprintf(cgiOut, "<table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "  <tr>\n");
	fprintf(cgiOut, "    <td width=\"17\" height=\"29\" valign=\"top\" background=\"../images/mail_leftbg.gif\"><img src=\"../images/left-top-right.gif\" width=\"17\" height=\"29\" /></td>\n");
	fprintf(cgiOut, "    <td width=\"935\" height=\"29\" valign=\"top\" background=\"../images/content-bg.gif\"><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"left_topbg\" id=\"table2\">\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td height=\"31\"><div class=\"titlebt\">基本设置</div></td>\n");
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
	fprintf(cgiOut, "            <td class=\"left_txt\">当前位置：基本设置</td>\n");
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
	fprintf(cgiOut, "                <td width=\"90%%\" valign=\"top\"><span class=\"left_txt2\">在这里，您可以根据您的要求，修改设置设备的</span><span class=\"left_txt3\">基本参数</span><span class=\"left_txt2\">！</span><br>\n");
	fprintf(cgiOut, "                          <span class=\"left_txt2\">包括</span><span class=\"left_txt3\">设备名称，设备位置，描述</span><span class=\"left_txt2\">以及</span><span class=\"left_txt3\">设备ID</span><span class=\"left_txt2\">。 </span></td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td>&nbsp;</td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"nowtable\">\n");
	fprintf(cgiOut, "              <tr>\n");
	fprintf(cgiOut, "                <td class=\"left_bt2\">&nbsp;&nbsp;&nbsp;&nbsp;系统参数设置</td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	fprintf(cgiOut, "              <form name=\"form1\" method=\"POST\" action=\"");
	fprintf(cgiOut, "\">\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td width=\"20%%\" height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">设备名称：</td>\n");
	fprintf(cgiOut, "                  <td width=\"3%%\" bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	show_host_name();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">设备位置：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	show_host_pos();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">设备序列号：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	show_host_sn();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">软件版本： </td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	show_sw_ver();

	/*
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">描述：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	show_description();
	*/
/*	
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">FPGA版本：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	show_fpga_ver();
*/
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F2F2F2\" class=\"left_txt2\">硬件版本：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F2F2F2\">&nbsp;</td>\n");
	show_hw_ver();
	/*
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">设备ID：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	show_host_id();
	*/
	fprintf(cgiOut, "                </tr>\n");
	//fprintf(cgiOut, "                <tr>\n");
	//fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">联系方式： </td>\n");
	//fprintf(cgiOut, "                  <td bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	//fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#f2f2f2\"><input type=\"text\" name=\"ci\" size=\"30\" /></td>\n");
	//fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"17\" colspan=\"4\" align=\"right\" >&nbsp;</td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" align=\"center\" class=\"left_txt2\"><input type=\"submit\" value=\"完成以上修改\" name=\"host\" /></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" class=\"left_txt2\"></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" class=\"left_txt\"></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "              </form>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "        </table>\n");
	fprintf(cgiOut, "          </td>\n");
	fprintf(cgiOut, "      </tr>\n");
	fprintf(cgiOut, "    </table></td>\n");
	fprintf(cgiOut, "    <td background=\"../images/mail_rightbg.gif\">&nbsp;</td>\n");
	fprintf(cgiOut, "  </tr>\n");
	fprintf(cgiOut, "  <tr>\n");
	fprintf(cgiOut, "    <td valign=\"middle\" background=\"../images/mail_leftbg.gif\"><img src=\"../images/buttom_left2.gif\" width=\"17\" height=\"17\" /></td>\n");
	fprintf(cgiOut, "      <td height=\"17\" valign=\"top\" background=\"../images/buttom_bgs.gif\"><img src=\"../images/buttom_bgs.gif\" width=\"17\" height=\"17\" /></td>\n");
	fprintf(cgiOut, "    <td background=\"../images/mail_rightbg.gif\"><img src=\"../images/buttom_right2.gif\" width=\"16\" height=\"17\" /></td>\n");
	fprintf(cgiOut, "  </tr>\n");
	fprintf(cgiOut, "</table>\n");
	fprintf(cgiOut, "</body>\n");
}