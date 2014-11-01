
#ifndef __MODE_H__
#define __MODE_H__


enum    WORK_MODE
{
    WORK_NO,
    WORK_SOCKET,
};

enum    TCP_MODE
{
    TCP_RAW,
    TCP_TELNET,
};

enum    TCP_PTL
{
    TCP_NO,
    TCP_CLI,
    TCP_SRV,
    TCP_UDP,
};

enum    CR_LF
{
    TO_CR,
    TO_LF,
};

#endif

