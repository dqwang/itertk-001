#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"
#include "itscmd.h"

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

char name[32] = "";
CONFIG_NET con_net;


void show_host_ip(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_ip\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", sys_ip2str_static(con_net.dev_ip));
}

void show_host_netmask(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_netmask\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", sys_ip2str_static(con_net.dev_nm));
}

void show_host_dev_gw(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_dev_gw\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", sys_ip2str_static(con_net.dev_gw));
}

void show_host_dns0(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_dns0\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", sys_ip2str_static(con_net.dns[0]));
}

void show_host_dns1(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"host_dns1\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", sys_ip2str_static(con_net.dns[1]));
}
void show_dev_mac(void)
{
//	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"dev_mac\" type=\"text\" value=\"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\" size=\"30\" readonly=\"true\"/></td>\n", \
	//	con_net.dev_mac[0], con_net.dev_mac[1], con_net.dev_mac[2], con_net.dev_mac[3], con_net.dev_mac[4], con_net.dev_mac[5]);
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"dev_mac\" type=\"text\" value=\"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\" size=\"30\"/></td>\n", \
		con_net.dev_mac[0], con_net.dev_mac[1], con_net.dev_mac[2], con_net.dev_mac[3], con_net.dev_mac[4], con_net.dev_mac[5]);
}

void show_dev_dhcp(void)
{
	char *str[2] = {"否","是"};
	int i;
	fprintf(cgiOut, "                    <select name=\"dev_dhcp\" style=\"width:80px\">\n");
	for(i = 0; i < 2; i++)
	{
		if(i == con_net.dev_dhcp)
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


/*
 * IP String Check
 */
#define TRUE 1
#define FALSE 0

int ip_check(char * ip)
{
	if(NULL == ip)	
		return FALSE;
	
	unsigned int result = 0;
	unsigned int d = 0;  
	int dotNum = 0;  
	int dNum = 0;  
	char c = 0;
	int char_exit=0;
	int i;
	for(i = 0; ; ++i){
		c = ip[i];
		if(c >= '0' && c <= '9')   {
			++ dNum;
			char_exit++;
			d = 10 * d + (c - '0');
			if(d > 255 || d < 0) 
				return FALSE;
		}else if(c == '.') {
			++ dotNum;
			if(dotNum > dNum)  
			{
				return FALSE;
			}else {
				result = (result<<8) | d;
				d = 0;
			}
			if (char_exit == 0)
				return -1;
			if (char_exit > 0)
				char_exit = 0;
		}else if(c == '\0')  {
			if(dotNum != 3 || char_exit == 0){
				return FALSE;
			}else
			{
				result = (result<<8) + d;
				return TRUE;
			}
		}else  {
			return FALSE;
		}
	}
}


#define is_mac(c)	((c>='0' && c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F'))
#define is_colon(c)	(c == ':')
/*
 * MAC String Check
 */
#define MAC_STR_LEN 17 
int mac_str_check(char *mac_str)
{
	int i;
	char c;
	
	//printf("check: %s\n", mac_str);
	if (NULL == mac_str)
		return -1;
	if (strlen(mac_str) != MAC_STR_LEN)
		return -1;
	for (i = 0;i<MAC_STR_LEN;i++){
		c=mac_str[i];
		//printf("%c\n", c);
		switch(i){
			case 0 ... 1:
			case 3 ... 4:
			case 6 ... 7:
			case 9 ... 10:
			case 12 ... 13:
			case 15 ... 16:
				if ( !is_mac(c) )
					return -1;
			break;

			case 2:
			case 5:
			case 8:
			case 11:
			case 14:
				if (!is_colon(c))
					return -1;
			break;
			default:
				break;
		}
	}

	return 0;
	
}

/* MAC FROM STRING TO HEX
  * WANG 20140508
  */

int str2hex(const char *str)
{
	int i=0, tmp, result=0;

	if (str  == NULL)
		return -1;
	
	for(i=0; i<strlen(str); i++){
		if((str[i]>='0')&&(str[i]<='9'))
			tmp = str[i]-'0';
		else if((str[i]>='A')&&(str[i]<='F'))
			tmp = str[i]-'A'+10;
		else if((str[i]>='a')&&(str[i]<='f'))
			tmp = str[i]-'a'+10;
		else
			return -1;

		result = result*16+tmp;
	}
	return result;
 }
/*
  *
  */
int get_mac(char *str)
{
	char * delim = ":";
	char *p = strtok(str, delim);
	int i;
		
	con_net.dev_mac[0]=str2hex(p);
	if (str2hex(p)<0)
		return -1;

	for (i=1;i<6;i++){
		p = strtok(NULL, delim);
		if (str2hex(p)<0)
			return -1;
		con_net.dev_mac[i] = str2hex(p);
	}
	
	//printf("%02x, %02x,%02x,%02x,%02x,%02x\n", con_net.dev_mac[0],con_net.dev_mac[1],con_net.dev_mac[2],con_net.dev_mac[3],con_net.dev_mac[4],con_net.dev_mac[5]);
		

	return 0;
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
	
	ret = its_netinfo_query(name, &con_net);
	
	
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
	else if(cgiFormSubmitClicked("net") == cgiFormSuccess)
	{
		char str[32]="";
		get_host("host_ip", str);

		if (ip_check(str) == FALSE){
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"IP参数格式错误,请重新设置！\");\n", ret);
			fprintf(cgiOut, "</script>\n");			
			bzero(&con_net, sizeof(con_net));
			its_netinfo_query(name, &con_net);

			goto SHOW;
		}
		
		con_net.dev_ip = sys_str2ip(str);
		
		bzero(str, sizeof(str));
		get_host("host_netmask", str);
		if (ip_check(str) == FALSE){
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"子网掩码参数格式错误,请重新设置！\");\n", ret);
			fprintf(cgiOut, "</script>\n");			
			bzero(&con_net, sizeof(con_net));
			its_netinfo_query(name, &con_net);

			goto SHOW;
		}
		con_net.dev_nm = sys_str2ip(str);
		
		bzero(str, sizeof(str));
		get_host("host_dev_gw", str);
		if (ip_check(str) == FALSE){
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"默认网关参数格式错误,请重新设置！\");\n", ret);
			fprintf(cgiOut, "</script>\n");			
			bzero(&con_net, sizeof(con_net));
			its_netinfo_query(name, &con_net);

			goto SHOW;
		}
		con_net.dev_gw = sys_str2ip(str);
		
		bzero(str, sizeof(str));
		get_host("host_dns0", str);
		if (ip_check(str) == FALSE){
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"主DNS参数格式错误,请重新设置！\");\n", ret);
			fprintf(cgiOut, "</script>\n");			
			bzero(&con_net, sizeof(con_net));
			its_netinfo_query(name, &con_net);

			goto SHOW;
		}
		con_net.dns[0] = sys_str2ip(str);
		
		bzero(str, sizeof(str));
		get_host("host_dns1", str);
		if (ip_check(str) == FALSE){
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"次DNS参数格式错误,请重新设置！\");\n", ret);
			fprintf(cgiOut, "</script>\n");			
			bzero(&con_net, sizeof(con_net));
			its_netinfo_query(name, &con_net);

			goto SHOW;
		}
		con_net.dns[1] = sys_str2ip(str);
		
		char *str1[2] = {"否","是"};
		int i;
		cgiFormSelectSingle("dev_dhcp", str1, sizeof(str1)/sizeof(str1[0]), &i, 0);
		con_net.dev_dhcp = i;

		/*XXX 
		  *get mac from web
		  */
		bzero(str, sizeof(str));
		get_host("dev_mac", str);
		
		
		if (mac_str_check(str)<0 || get_mac(str)<0 ){
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"物理地址错误,请重新设置！\");\n", ret);
			fprintf(cgiOut, "</script>\n");			
			bzero(&con_net, sizeof(con_net));
			its_netinfo_query(name, &con_net);

			goto SHOW;
			
		}

		
		
		ret = its_netinfo_set(name, &con_net);
		if(ret != NET_SET_OK)
		{
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交失败(错误码：%d)！\");\n", ret);
			fprintf(cgiOut, "</script>\n");
			
			//提交失败后需要重新获取参数
			bzero(&con_net, sizeof(con_net));
			its_netinfo_query(name, &con_net);
		}
		else
		{
			//fprintf(cgiOut, "[%s][%s][%s][%s]\n", con_sys.host_name, con_sys.host_pos, con_sys.host_id, con_sys.description);
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交成功！\");\n");
			fprintf(cgiOut, "</script>\n");
		}
	}

SHOW:

	fprintf(cgiOut, "<table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "  <tr>\n");
	fprintf(cgiOut, "    <td width=\"17\" height=\"29\" valign=\"top\" background=\"../images/mail_leftbg.gif\"><img src=\"../images/left-top-right.gif\" width=\"17\" height=\"29\" /></td>\n");
	fprintf(cgiOut, "    <td width=\"935\" height=\"29\" valign=\"top\" background=\"../images/content-bg.gif\"><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"left_topbg\" id=\"table2\">\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td height=\"31\"><div class=\"titlebt\">网络设置</div></td>\n");
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
	fprintf(cgiOut, "            <td class=\"left_txt\">当前位置：网络设置</td>\n");
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
	fprintf(cgiOut, "                <td width=\"90%%\" valign=\"top\"><span class=\"left_txt2\">在这里，您可以根据您的要求，修改设置设备的</span><span class=\"left_txt3\">网络参数</span><span class=\"left_txt2\">！</span><br>\n");
	fprintf(cgiOut, "                          <span class=\"left_txt2\">包括</span><span class=\"left_txt3\">IP地址，子网掩码，默认网关，DSN服务器</span><span class=\"left_txt2\">以及设备</span><span class=\"left_txt3\">MAC地址设置</span><span class=\"left_txt2\">。 </span></td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td>&nbsp;</td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"nowtable\">\n");
	fprintf(cgiOut, "              <tr>\n");
	fprintf(cgiOut, "                <td class=\"left_bt2\">&nbsp;&nbsp;&nbsp;&nbsp;网络参数设置</td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	fprintf(cgiOut, "              <form name=\"form1\" method=\"POST\" action=\"");
	fprintf(cgiOut , "\">\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td width=\"20%%\" height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">以太网IP地址：</td>\n");
	fprintf(cgiOut, "                  <td width=\"3%%\" bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	show_host_ip();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">以太网子网掩码：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	show_host_netmask();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">默认网关：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	show_host_dev_gw();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">主DNS服务器： </td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	show_host_dns0();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">次DNS服务器：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	show_host_dns1();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">启用DHCP：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_dev_dhcp();
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F2F2F2\" class=\"left_txt2\">MAC地址：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F2F2F2\">&nbsp;</td>\n");
	show_dev_mac();
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"17\" colspan=\"4\" align=\"right\" >&nbsp;</td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" align=\"center\" class=\"left_txt2\"><input type=\"submit\" value=\"完成以上修改\" name=\"net\" /></td>\n");
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
