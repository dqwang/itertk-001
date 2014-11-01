#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"


int cgiMain()
{
	char name[32]="";
	strcpy(name, cgiQueryString);
	cgiHeaderContentType("text/html;charset=gb2312");
	fprintf(cgiOut, "<html>\n");
	fprintf(cgiOut, "<head>\n");
	fprintf(cgiOut, "<title><%s> - 管理页面</title>\n", name);
	fprintf(cgiOut, "<script language=JavaScript>\n");
	fprintf(cgiOut, "function logout(){\n");
	fprintf(cgiOut, "	if (confirm(\"您确定要退出吗？\"))\n");
	fprintf(cgiOut, "	top.location = \"../login.html\";\n");
	fprintf(cgiOut, "	return false;\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "<script language=JavaScript1.2>\n");
	fprintf(cgiOut, "function showsubmenu(sid) {\n");
	fprintf(cgiOut, "	var whichEl = eval(\"submenu\" + sid);\n");
	fprintf(cgiOut, "	var menuTitle = eval(\"menuTitle\" + sid);\n");
	fprintf(cgiOut, "	if (whichEl.style.display == \"none\"){\n");
	fprintf(cgiOut, "		eval(\"submenu\" + sid + \".style.display=\"\";\");\n");
	fprintf(cgiOut, "	}else{\n");
	fprintf(cgiOut, "		eval(\"submenu\" + sid + \".style.display=\"none\";\");\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "<meta http-equiv=Content-Type content=text/html;charset=gb2312>\n");
	fprintf(cgiOut, "<meta http-equiv=\"refresh\" content=\"60\">\n");
	fprintf(cgiOut, "<script language=JavaScript1.2>\n");
	fprintf(cgiOut, "function showsubmenu(sid) {\n");
	fprintf(cgiOut, "	var whichEl = eval(\"submenu\" + sid);\n");
	fprintf(cgiOut, "	var menuTitle = eval(\"menuTitle\" + sid);\n");
	fprintf(cgiOut, "	if (whichEl.style.display == \"none\"){\n");
	fprintf(cgiOut, "		eval(\"submenu\" + sid + \".style.display=\"\";\");\n");
	fprintf(cgiOut, "	}else{\n");
	fprintf(cgiOut, "		eval(\"submenu\" + sid + \".style.display=\"none\";\");\n");
	fprintf(cgiOut, "	}\n");
	fprintf(cgiOut, "}\n");
	fprintf(cgiOut, "</script>\n");
	fprintf(cgiOut, "<base target=\"main\">\n");
	fprintf(cgiOut, "<link href=\"../images/skin.css\" rel=\"stylesheet\" type=\"text/css\">\n");
	fprintf(cgiOut, "</head>\n");
	fprintf(cgiOut, "<body leftmargin=\"0\" topmargin=\"0\">\n");
	
	fprintf(cgiOut, "<table width=\"100%%\" height=\"64\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"admin_topbg\">\n");
	fprintf(cgiOut, "  <tr>\n");
	fprintf(cgiOut, "    <td width=\"61%%\" height=\"64\"><img src=\"../images/logo.gif\" width=\"262\" height=\"64\"></td>\n");
	fprintf(cgiOut, "    <td width=\"39%%\" valign=\"top\"><table width=\"100%%\" border=\"0\" cellspacing=\"0\" cellpadding=\"0\">\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td width=\"74%%\" height=\"38\" class=\"admin_txt\"><b> %s </b> 您好,感谢登陆使用！</td>\n", name);
	fprintf(cgiOut, "        <td width=\"22%%\"><a href=\"#\" target=\"_self\" onClick=\"logout();\"><img src=\"../images/out.gif\" alt=\"安全退出\" width=\"46\" height=\"20\" border=\"0\"></a></td>\n");
	fprintf(cgiOut, "        <td width=\"4%%\">&nbsp;</td>\n");
	fprintf(cgiOut, "      </tr>\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td height=\"19\" colspan=\"3\">&nbsp;</td>\n");
	fprintf(cgiOut, "        </tr>\n");
	fprintf(cgiOut, "    </table></td>\n");
	fprintf(cgiOut, "  </tr>\n");
	fprintf(cgiOut, "</table>\n");
	fprintf(cgiOut, "</body>\n");
	fprintf(cgiOut, "</html>\n");
}
