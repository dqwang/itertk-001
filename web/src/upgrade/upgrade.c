#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cgic.h"


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <linux/compiler.h>
#include <mtd/mtd-user.h>
#include <string.h>

#include <linux/reboot.h>
#include <signal.h>

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


#define UPDATE_FILE_LEN_MAX (8*1024*1024)
int writeFile(char* file)
{
	cgiFilePtr pfile;
	char name[128]="";
	char buffer[1024]="";
	int size, got;

	

	if(cgiFormFileName("upfile", name, sizeof(name)) != cgiFormSuccess)
	{
		printf("<p>No file was uploaded.<p>\n");
		return -1;
	}

	if (strcmp(name, "nand1-2.tar.gz") != 0   ){
		return -1;
	}
	
	
	cgiFormFileSize("upfile", &size);

	if (size > UPDATE_FILE_LEN_MAX){
		return -1;
	}
	

	if(cgiFormFileOpen("upfile", &pfile) != cgiFormSuccess)
	{
		fprintf(cgiOut, "Could not open the file.<p>\n");
		return -1;
	}

	int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC);
	
	while(cgiFormFileRead(pfile, buffer, sizeof(buffer), &got) == cgiFormSuccess)
	{
		if(size < sizeof(buffer))
		{
			write(fd, buffer, (size));
			break;	
		}
		else
		{
			write(fd, buffer, sizeof(buffer));
			size -= sizeof(buffer);
		}
	}

	close(fd);

	return 0;

#if 0

	cgiFormFileClose(pfile);
	if(access("/tmp/NC616.bin", F_OK) == 0)
	{
		unlink("/tmp/NC616.bin");
	}
	char cmd[128]="";
	sprintf(cmd, "/usr/app/bin/unzip -P qwfyx %s -d /tmp/", file);
	int res = system(cmd);

	if(access("/tmp/NC616.bin", F_OK) != 0)
	{
		
		unlink(file);
		return -1;
	}



	int ret = mtdinfo_read("/dev/mtd1");
	if(ret < 0)
	{
		printf("read mtd info failed!\n");
		return -1;
	}
	ret = non_region_erase("/dev/mtd1", 0, (meminfo.size/meminfo.erasesize));
	if(ret < 0)
	{
		return ret;
	}
	
	return mtd_write("/dev/mtd1", "/tmp/NC616.bin");

#endif	
	
}

void handler()
{
	reboot(LINUX_REBOOT_CMD_RESTART); 
}

void showmain(int flag);
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
		fprintf(cgiOut, "top.window.location.href=\"../login.html\";\n");
		fprintf(cgiOut, "</script>\n");
		fprintf(cgiOut, "</BODY>\n");
		fprintf(cgiOut, "</HTML>\n");
		return 0;
	}

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
	

	if(cgiFormSubmitClicked("upgrade") == cgiFormSuccess)
	{
		showmain(0);
		
		
		//fprintf(cgiOut, "<script type=\"text/javascript\">\n");
		//fprintf(cgiOut, "window.alert(\"开始升级，升级完将自动重启系统\");\n");
		//fprintf(cgiOut, "</script>\n");
		
		
		ret = writeFile("/mnt/nand1-2/update.tar.gz");
		if(ret == 0)
		{
			
			system("/sbin/reboot -d 30 &");
			system("/bin/tar -xzf  /mnt/nand1-2/update.tar.gz  -C /mnt/nand1-2/ &");
			
			
			
			system("rm -f /mnt/nand1-2/update.tar.gz");
			
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			//fprintf(cgiOut, "window.setTimeout(function(){top.window.location.href=\"../login.html\"},000)\n");
			fprintf(cgiOut, "window.alert(\"升级成功，重启系统\");\n");
			fprintf(cgiOut, "top.window.location.href=\"../login.html\";\n");
			
		
			fprintf(cgiOut, "</script>\n");
			fprintf(cgiOut, "</body>\n");
			
						
			//return 1;
		}
		else
		{
			fprintf(cgiOut, "<script type=\"text/javascript\">\n");
			fprintf(cgiOut, "window.alert(\"升级失败\");\n");
			fprintf(cgiOut, "window.location.href=\"upgrade.cgi\";\n");
			fprintf(cgiOut, "</script>\n");
			fprintf(cgiOut, "</body>\n");
			return 1;
		}

		
	}
	else if(cgiFormSubmitClicked("reboot") == cgiFormSuccess)
	{
		reboot(LINUX_REBOOT_CMD_RESTART); 
	}

	showmain(1);
}

void showmain(int flag)
{
	fprintf(cgiOut, "<table width=\"100%%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	fprintf(cgiOut, "  <tr>\n");
	fprintf(cgiOut, "    <td width=\"17\" height=\"29\" valign=\"top\" background=\"../images/mail_leftbg.gif\"><img src=\"../images/left-top-right.gif\" width=\"17\" height=\"29\" /></td>\n");
	fprintf(cgiOut, "    <td width=\"935\" height=\"29\" valign=\"top\" background=\"../images/content-bg.gif\"><table width=\"100%%\" height=\"31\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\" class=\"left_topbg\" id=\"table2\">\n");
	fprintf(cgiOut, "      <tr>\n");
	fprintf(cgiOut, "        <td height=\"31\"><div class=\"titlebt\">版本升级</div></td>\n");
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
	fprintf(cgiOut, "            <td class=\"left_txt\">当前位置：版本升级</td>\n");
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
	fprintf(cgiOut, "                <td width=\"10%%\" height=\"55\" valign=\"middle\"><img src=\"../images/usercontrol.gif\" width=\"54\" height=\"55\"></td>\n");
	fprintf(cgiOut, "                <td width=\"90%%\" valign=\"top\"><span class=\"left_txt2\">在这里，您可以对本产品进行软件升级！</span><span class=\"left_txt3\"></span><span class=\"left_txt2\">！</span><br>\n");
	fprintf(cgiOut, "                          <span class=\"left_txt2\"></span><span class=\"left_txt3\">注意：只有本公司提供的升级包才能成功升级！通过升级本设备的软件，您将获得新的功能。注意:升级过程中不能关闭设备电源，否则将导致设备损坏无法使用。升级过程约30秒，当升级结束后，设备将自动重启。点击重启按钮，设备也将重启。</span><span class=\"left_txt2\"></span><span class=\"left_txt3\"></span><span class=\"left_txt2\">。 </span></td>\n");
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
	fprintf(cgiOut, "              <form name=\"form1\" method=\"POST\" enctype=\"multipart/form-data\" action=\"");
	fprintf(cgiOut , "\">\n");
	fprintf(cgiOut, "                <tr>\n");
	if(flag == 1)		
		fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">升级文件名称：</td>\n");
	else
		fprintf(cgiOut, "                  <td height=\"30\" align=\"right\" class=\"left_txt2\">正在升级</td>\n");
	fprintf(cgiOut, "                  <td>&nbsp;</td>\n");
	if(flag == 0)
		fprintf(cgiOut, "                  <td height=\"30\"><img src=\"../images/loading11.gif\"></td>\n");
	else
		fprintf(cgiOut, "                  <td height=\"30\"><input type=\"file\" name=\"upfile\" /></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"17\" colspan=\"4\" align=\"middle\" ></td>\n");
	fprintf(cgiOut, "                </tr>\n");
	fprintf(cgiOut, "                <tr>\n");
	fprintf(cgiOut, "                  <td height=\"30\" colspan=\"4\" align=\"center\" class=\"left_txt2\"><input type=\"submit\" value=\"升级\" name=\"upgrade\" />&nbsp;&nbsp;<input type=\"submit\" value=\"重启\" name=\"reboot\" /></td>\n");
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
	if(flag == 1)
		fprintf(cgiOut, "</body>\n");
	
}







