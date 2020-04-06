//
// Created by hzl on 19-6-2.
//

#ifndef __TCP_S_H__
#define __TCP_S_H__


#include "tcp_cs.h"

typedef int (*PROCESS_FRAGMENT)(struct tcpcs_head head, char *fragment);

enum tcps_err
{
    tcps_err_init = 0,
    tcps_err_max
};

struct tcps
{
    int m_port;
    int m_listen_fd;
    struct loop *lp;
    PROCESS_FRAGMENT m_process_fragment;
};

struct tcps*tcps_init(int port);

int tcps_set_process_fragment(struct tcps *t, PROCESS_FRAGMENT pf);
int tcps_run(struct tcps *t);

void tcps_fini(struct tcps *t);


#endif
