#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"
#include "itscmd.h"

void Name(char *name);
void Passwd(char *passwd); 


void session_save(char* name)
{
	char lsbuf[64]="";
	sprintf(lsbuf, "/tmp/nc_%s", cgiRemoteAddr);
	int fd = open(lsbuf, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXO);
	if(fd <= 0)
	{
		perror("open");
		_exit(0);
	}
	write(fd, name, strlen(name));
	close(fd);
}

int cgiMain()
{
	
	
	char name[32]="", passwd[32]="";
	Name(name);
	Passwd(passwd);
	
	cgiHeaderContentType("text/html;charset=gb2312");
	fprintf(cgiOut, "<HTML><HEAD>\n");
	//fprintf(cgiOut, "<LINK rel=\"SHORTCUT ICON\" 	href=\"../images/favicon.ico\">\n");
	fprintf(cgiOut, "<TITLE>RTNS</TITLE></HEAD>\n");
	int ret = user_login(name, passwd);
	if(ret != LOGIN_OK)
	{
		fprintf(cgiOut, "<BODY>\n");
		fprintf(cgiOut, "<script type=\"text/javascript\">\n");
		fprintf(cgiOut, "window.alert(\"用户名或密码错误，请重新登录(错误码：%d)！\");\n", ret);
		fprintf(cgiOut, "window.location.href=\"../login.html\";\n");
		fprintf(cgiOut, "</script>\n");
		fprintf(cgiOut, "</BODY>\n");
		fprintf(cgiOut, "</HTML>\n");
		return 0;
	}
	session_save(name);	
	fprintf(cgiOut, "<frameset rows=\"64,*\"  frameborder=\"NO\" border=\"0\" framespacing=\"0\">\n");
	fprintf(cgiOut, "<frame src=\"top.cgi?%s\" noresize=\"noresize\" frameborder=\"NO\" name=\"topFrame\" scrolling=\"no\" marginwidth=\"0\" marginheight=\"0\" target=\"main\" />\n", name);
	fprintf(cgiOut, "<frameset cols=\"200,*\"  rows=\"560,*\" id=\"frame\">\n");
	fprintf(cgiOut, "<frame src=\"../left.html\" name=\"leftFrame\" noresize=\"noresize\" marginwidth=\"0\" marginheight=\"0\" frameborder=\"0\" scrolling=\"auto\" target=\"main\" />\n");
	fprintf(cgiOut, "<frame src=\"../right.html\" name=\"main\" marginwidth=\"0\" marginheight=\"0\" frameborder=\"0\" scrolling=\"auto\" target=\"_self\" />\n");
	fprintf(cgiOut, "</frameset>\n");
	fprintf(cgiOut, "<noframes>\n");
	fprintf(cgiOut, "<body></body>\n");
	fprintf(cgiOut, "</noframes>\n");
	fprintf(cgiOut, "</html>\n");
	return 0;
}


void Name(char *name) 
{
	cgiFormStringNoNewlines("username", name, 32);
}

void Passwd(char *passwd) 
{
	cgiFormStringNoNewlines("password", passwd, 32);
}

