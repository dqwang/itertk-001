#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"
#include "itscmd.h"
#include "system.h"
#include <netdb.h>
#include <netinet/in.h>



char name[32]="";
CONFIG_SERVER con_server;
CONFIG_NET config_net;


#define DNS_STR_LEN 50
#define SERVER_PORT 8888

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

void show_server_ip(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"server_ip\" type=\"text\" value=\"%s\" size=\"30\"/></td>\n", sys_ip2str_static(con_server.server_ip));
}


void show_server_port(void)
{
	fprintf(cgiOut, "<td height=\"30\" bgcolor=\"#f2f2f2\"><input name=\"server_port\" type=\"text\" value=\"%d\" size=\"30\"/></td>\n", con_server.server_port);
}
void get_server_ip(char *name, char *result)
{
	bzero(result, DNS_STR_LEN);
	cgiFormStringNoNewlines(name, result, DNS_STR_LEN);
}

void get_server_port(char *name, int *result)
{
	cgiFormInteger(name, result,SERVER_PORT);
}
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
	
	memset(&con_server, 0, sizeof(CONFIG_SERVER));
	ret = its_conf_server_query(name, &con_server);
	
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
	else if(cgiFormSubmitClicked("server") == cgiFormSuccess)
	{
		char ip_str[DNS_STR_LEN]="";
		char ip_dest[14]="";
		int port=0;		
		struct hostent *h;		
		int ret_val = 0;

		ret_val = its_netinfo_query(name, &config_net);
		
		
		get_server_ip("server_ip",ip_str);

		if (ip_check(ip_str) == TRUE){
			h = gethostbyname(ip_str);	
			con_server.server_ip = sys_str2ip(inet_ntoa(*((struct in_addr *)h->h_addr_list[0])));
		}else if (its_dns(ip_str,config_net.dns[0], ip_dest) == DNS_OK){
			con_server.server_ip= sys_str2ip( ip_dest );
		}else{
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"IP - 域名格式错误，请重新输入\");\n");
			fprintf(cgiOut, "</script>\n");
			

			memset(&con_server, 0, sizeof(CONFIG_SERVER));
			its_conf_server_query(name,&con_server);

			goto SHOW;
		}
	
	
		get_server_port("server_port", &port);	

		if (port <=0 || port >=0xffff){
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"端口号错误，请重新输入\");\n");
			fprintf(cgiOut, "</script>\n");
			

			memset(&con_server, 0, sizeof(CONFIG_SERVER));
			its_conf_server_query(name,&con_server);

			goto SHOW;
		}else{
			con_server.server_port = port;
		}
			
		
		ret = its_conf_server_set(name, &con_server);
		
		if(ret != SERVER_SET_OK)
		{
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交失败(错误码：%d)！\");\n", ret);
			fprintf(cgiOut, "</script>\n");
			

			memset(&con_server, 0, sizeof(CONFIG_SERVER));
			its_conf_server_query(name,&con_server);
			
		}
		else
		{
			
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
	fprintf(cgiOut, "        <td height=\"31\"><div class=\"titlebt\">服务器设置</div></td>\n");
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
	fprintf(cgiOut, "                <td width=\"90%%\" valign=\"top\"><span class=\"left_txt2\">在这里，您可以根据您的要求，修改设置服务器的</span><span class=\"left_txt3\">基本参数</span><span class=\"left_txt2\">！</span><br>\n");
	fprintf(cgiOut, "                          <span class=\"left_txt2\">可以直接填写服务器IP地址，也可以填写域名</span></td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td>&nbsp;</td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"nowtable\">\n");
	
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	fprintf(cgiOut, "              <form name=\"form1\" method=\"POST\" action=\"");
	fprintf(cgiOut, "\">\n");
	fprintf(cgiOut, "                <tr>\n");

	fprintf(cgiOut, "                </tr>\n");	
	fprintf(cgiOut, "                <tr>\n");	
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">服务器IP或者域名</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	show_server_ip();

	fprintf(cgiOut, "                </tr>\n");	
	fprintf(cgiOut, "                <tr>\n");	
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">服务器端口号</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	show_server_port();
	
	
	fprintf(cgiOut, "                  </label></td>\n");
	fprintf(cgiOut, "                </tr>                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"17\" colspan=\"4\" align=\"right\" >&nbsp;</td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" align=\"center\" class=\"left_txt2\"><input type=\"submit\" value=\"完成以上修改\" name=\"server\" />\n");
	

	
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


