//
// Created by hzl on 19-6-2.
//

#ifndef __TCP_C_H__
#define __TCP_C_H__


#include "tcp_cs.h"



typedef int (*PROCESS_FRAGMENT)(struct tcpcs_head head, char *fragment);

enum tcpc_err
{
    tcpc_err_init = 0,
    tcpc_err_max
};

struct tcpc
{
    int m_port;
    int m_connect_fd;
    struct loop *lp;
    PROCESS_FRAGMENT m_process_fragment;
};

struct tcpc*tcpc_init(struct in_addr addr, int port);

int tcpc_set_process_fragment(struct tcpc *t, PROCESS_FRAGMENT pf);
int tcpc_run(struct tcpc *t);

void tcpc_fini(struct tcpc *t);


#endif
