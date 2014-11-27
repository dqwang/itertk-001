#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"
#include "itscmd.h"

char name[32]="";
CONFIG_COM con_com;

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

void show_bps(void)
{
	char* bps[] = {
		"300",
		"600",		
		"1200",
		"2400",
		"4800",
		"9600",
		"19200",
		"38400",
		"57600",
		"115200",
		"230400",
		"460800",
		"576000",
		"921600"
	};
	
	int i;
	fprintf(cgiOut, "                    <select name=\"bps\" style=\"width:80px\">\n");
	for(i = 0; i < 14; i++)
	{
		if(atoi(bps[i]) == con_com.bps)
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", bps[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", bps[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}

void show_dbit(void)
{
	int dbit[] = {
		5,
		6,
		7,
		8
	};
	
	int i;
	fprintf(cgiOut, "                    <select name=\"dbit\" style=\"width:80px\">\n");
	for(i = 0; i < sizeof(dbit)/sizeof(dbit[0]); i++)
	{
		if(dbit[i] == con_com.dbit)
			fprintf(cgiOut, "                      <option selected=\"selected\">%d</option>\n", dbit[i]);
		else
			fprintf(cgiOut, "                      <option>%d</option>\n", dbit[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}

void show_chk(void)
{
	char* chk[] = {
		NULL,
		"偶校验",
		"奇校验",
		"无校验"
	};
	
	int i;
	fprintf(cgiOut, "                    <select name=\"chk\" style=\"width:80px\">\n");
	for(i = 1; i < sizeof(chk)/sizeof(chk[0]); i++)
	{
		if(i == con_com.chk)
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", chk[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", chk[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}

void show_sbit(void)
{
	int sbit[] = {
		1,
		2
	};
	
	int i;
	fprintf(cgiOut, "                    <select name=\"sbit\" style=\"width:80px\">\n");
	for(i = 0; i < sizeof(sbit)/sizeof(sbit[0]); i++)
	{
		if(sbit[i] == con_com.sbit)
			fprintf(cgiOut, "                      <option selected=\"selected\">%d</option>\n", sbit[i]);
		else
			fprintf(cgiOut, "                      <option>%d</option>\n", sbit[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}

void show_bctrl(void)
{
	char* bctrl[] = {
		"无"
	};
	
	int i;
	fprintf(cgiOut, "                    <select name=\"bctrl\" style=\"width:80px\">\n");
	for(i = 0; i < sizeof(bctrl)/sizeof(bctrl[0]); i++)
	{
			fprintf(cgiOut, "                      <option>%s</option>\n", bctrl[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}

void show_type(void)
{
	char* type[] = {
		"RS232",
		"RS485",
		//"RS422"
	};
	
	int i;
	fprintf(cgiOut, "                    <select name=\"type\" style=\"width:80px\">\n");
	for(i = 0; i < sizeof(type)/sizeof(type[0]); i++)
	{
		if(i == con_com.type)
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", type[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", type[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}

void show_loop(void)
{
	char* loop[] = {
		"否",
		"是"
	};
	
	int i;
	fprintf(cgiOut, "                    <select name=\"loop\" style=\"width:80px\">\n");
	for(i = 0; i < sizeof(loop)/sizeof(loop[0]); i++)
	{
		if(i == con_com.isloop)
			fprintf(cgiOut, "                      <option selected=\"selected\">%s</option>\n", loop[i]);
		else
			fprintf(cgiOut, "                      <option>%s</option>\n", loop[i]);
	}
	fprintf(cgiOut, "                    </select>\n");
}


int getallYN()
{
	if(cgiFormCheckboxSingle("checkbox") == cgiFormSuccess)
	{
		con_com.all = 1;
		return 1;
	}
	con_com.all = 0;
	return 0;
}

void get_bps(void)
{
	char* bps[] = {
		"300",
		"600",		
		"1200",
		"2400",
		"4800",
		"9600",
		"19200",
		"38400",
		"57600",
		"115200",
		"230400",
		"460800",
		"576000",
		"921600"
	};
	
	int i;
	cgiFormSelectSingle("bps", bps, sizeof(bps)/sizeof(bps[0]), &i, 0);
	con_com.bps = atoi(bps[i]);
//	printf("in %s,get bps i=%d,con_com.bps=%d\n",__FUNCTION__,i,con_com.bps);
}


void get_dbit(void)
{
	char* dbit[] = {
		"5",
		"6",
		"7",
		"8",
	};
	
	int i;
	cgiFormSelectSingle("dbit", dbit, sizeof(dbit)/sizeof(dbit[0]), &i, 0);
	con_com.dbit = atoi(dbit[i]);
}

void get_chk(void)
{
	char* chk[] = {
		//NULL,
		"偶校验",
		"奇校验",
		"无校验"
	};
	
	int i;
	cgiFormSelectSingle("chk", chk, sizeof(chk)/sizeof(chk[0]), &i, 0);
	con_com.chk = i + 1;
}

void get_sbit(void)
{
	char* sbit[] = {
		"1",
		"2",
	};
	
	int i;
	cgiFormSelectSingle("sbit", sbit, sizeof(sbit)/sizeof(sbit[0]), &i, 0);
	con_com.sbit = atoi(sbit[i]);
}

void get_type(void)
{
	char* type[] = {
		"RS232",
		"RS485",
		//"RS422"
	};
	
	int i;
	cgiFormSelectSingle("type", type, sizeof(type)/sizeof(type[0]), &i, 0);
	con_com.type = i;
}

void get_loop(void)
{
	char* loop[] = {
		"否",
		"是"
	};
	
	int i;
	cgiFormSelectSingle("loop", loop, sizeof(loop)/sizeof(loop[0]), &i, 0);
	con_com.isloop = i;
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
	
	con_com.id = atoi(cgiQueryString);
	ret = its_conf_com_query(name, &con_com);
	
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
	else if(cgiFormSubmitClicked("com") == cgiFormSuccess)
	{
		get_bps();
		get_dbit();
		get_chk();
		get_sbit();
		get_type();
		get_loop();
		getallYN();
		
		ret = its_conf_com_set(name, &con_com, con_com.all);
		if(ret != COM_SET_OK)
		{
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"提交失败(错误码：%d)！\");\n", ret);
			fprintf(cgiOut, "</script>\n");
			
			//提交失败后需要重新获取参数
			bzero(&con_com, sizeof(con_com));
			its_conf_com_query(name, &con_com);
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
	fprintf(cgiOut, "        <td height=\"31\"><div class=\"titlebt\">串口设置</div></td>\n");
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
	fprintf(cgiOut, "                <td width=\"90%%\" valign=\"top\"><span class=\"left_txt2\">在这里，您可以根据您的要求，修改设置串口的</span><span class=\"left_txt3\">基本参数</span><span class=\"left_txt2\">！</span><br>\n");
	fprintf(cgiOut, "                          <span class=\"left_txt2\">包括</span><span class=\"left_txt3\">波特率，数据位，校验方式，停止位，数据流控</span><span class=\"left_txt2\">等</span><span class=\"left_txt3\">参数设置</span><span class=\"left_txt2\">。 </span></td>\n");
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td>&nbsp;</td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"nowtable\">\n");
	fprintf(cgiOut, "              <tr>\n");
	fprintf(cgiOut, "                <td class=\"left_bt2\">&nbsp;&nbsp;&nbsp;&nbsp;串口%d设置</td>\n", con_com.id);
	fprintf(cgiOut, "              </tr>\n");
	fprintf(cgiOut, "            </table></td>\n");
	fprintf(cgiOut, "          </tr>\n");
	fprintf(cgiOut, "          <tr>\n");
	fprintf(cgiOut, "            <td><table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	fprintf(cgiOut, "              <form name=\"form1\" method=\"POST\" action=\"");
	fprintf(cgiOut, "\">\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td width=\"20%%\" height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">波特率：</td>\n");
	fprintf(cgiOut, "                  <td width=\"3%%\" bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td width=\"32%%\" height=\"30\" bgcolor=\"#f2f2f2\"><label>\n");
	show_bps();
	fprintf(cgiOut, "                    </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">数据位：</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_dbit();
	fprintf(cgiOut, "                  </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">校验方式：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#f2f2f2\"><label>\n");
	show_chk();
	fprintf(cgiOut, "                  </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">停止位： </td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\"><label>\n");
	show_sbit();
	fprintf(cgiOut, "                  </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#f2f2f2\" class=\"left_txt2\">数据流控制：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#f2f2f2\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#f2f2f2\"><label>\n");
	show_bctrl();
	fprintf(cgiOut, "                  </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">类型：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_type();
	fprintf(cgiOut, "                  </label></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" bgcolor=\"#F7F8F9\" class=\"left_txt2\">是否环回：</td>\n");
	fprintf(cgiOut, "                  <td bgcolor=\"#F7F8F9\">&nbsp;</td>\n");
	fprintf(cgiOut, "                  <td height=\"30\" bgcolor=\"#F7F8F9\"><label>\n");
	show_loop();
	fprintf(cgiOut, "                  </label></td>\n");
	fprintf(cgiOut, "                </tr>                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"17\" colspan=\"4\" align=\"right\" >&nbsp;</td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" align=\"center\" class=\"left_txt2\"><input type=\"submit\" value=\"完成以上修改\" name=\"com\" />\n");
	fprintf(cgiOut, "                    <label>\n");
	fprintf(cgiOut, "                     <input type=\"checkbox\" name=\"checkbox\" value=\"checkbox\">\n");
	fprintf(cgiOut, "                     设置应用到所有串口\n");
	fprintf(cgiOut, "                    </label></td>\n");
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


