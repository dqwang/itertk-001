#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include <stdio.h>
#include "def.h"


#define PRODUCT_NAME    "itertk_sensor"
#define PROGRAM_VERSION "v1.0"

extern  void sys_version_show ( FILE * stream );
extern  DWORD sys_str2ip(char *str);

extern  char* sys_ip2str_static ( DWORD ip );

extern UQWORD system_mscount_get(void);
extern int  writen( int fd, void *ptr, int nbytes);
extern int  writen_rs485( int fd, void *ptr, int nbytes);

#endif/*__SYSTEM_H__*/

