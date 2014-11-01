#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"
#include "itscmd.h"

char name[32] = "";
CONFIG_VLAN con_vlan;

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

void show_vlanid(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"id\" type=\"text\" value=\"%d\" size=\"30\"/></td>\n", con_vlan.id);
}

void show_enable(void)
{
	char *str[2] = {"否","是"};
	int i;
	fprintf(cgiOut, "                    <select name=\"enable\" style=\"width:80px\">\n");
	for(i = 0; i < 2; i++)
	{
		if(i == con_vlan.enable)
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", str[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", str[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
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
	
	ret = its_vlan_query(name, &con_vlan);
	
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
	else if(cgiFormSubmitClicked("vlan") == cgiFormSuccess)
	{
		
		char *str1[2] = {"否","是"};
		int i;
		cgiFormSelectSingle("enable", str1, sizeof(str1)/sizeof(str1[0]), &i, 0);
		con_vlan.enable = i;
		
		char str[32]="";
		get_host("id", str);
		con_vlan.id = atoi(str);
		
		ret = its_vlan_set(name, &con_vlan);
		if(ret != VLAN_SET_OK)
		{
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交失败(错误码：%d)！\");\n", ret);
			fprintf(cgiOut, "</script>\n");
			
			//提交失败后需要重新获取参数
			bzero(&con_vlan, sizeof(con_vlan));
			its_vlan_query(name, &con_vlan);
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
	fprintf(cgiOut, "        <td height=\"31\"><div class=\"titlebt\">Vlan设置</div></td>\n");
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
	fprintf(cgiOut, "                <td width=\"90%%\" valign=\"top\"><span class=\"left_txt2\">在这里，您可以根据您的网站要求，修改设置网站的</span><span class=\"left_txt3\">基本参数</span><span class=\"left_txt2\">！</span><br>\n");
	fprintf(cgiOut, "                          <span class=\"left_txt2\">包括</span><span class=\"left_txt3\">网站名称，网址，网站备案号，联系方式，网站公告，关键词，风格</span><span class=\"left_txt2\">等以及网站</span><span class=\"left_txt3\">会员及等级积分设置</span><span class=\"left_txt2\">。 </span></td>\n");
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
	fprintf(cgiOut, "                  <td width=\"20%%\" height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">启用VlanTag：</td>\n");
	fprintf(cgiOut, "                  <td width=\"3%%\" bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td width=\"32%%\" height=\"30\" bgcolor=\"#f2f2f2\"><label>\n");
	show_enable();
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">VlanID：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	show_vlanid();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"17\" colspan=\"4\" align=\"right\" >&nbsp;</td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" align=\"center\" class=\"left_txt2\"><input type=\"submit\" value=\"完成以上修改\" name=\"vlan\" /></td>\n");
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