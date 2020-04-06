#ifndef __TCP_CS_H__
#define __TCP_CS_H__

struct tcpcs_head
{
    int type;
    int length;
    int flag;
    int param;
};

#endif
