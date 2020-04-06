//
// Created by hzl on 19-6-2.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <errno.h>
#include <string.h>


#include <scheme.h>

#include "loop.h"
#include "queue_ex.h"
#include "threadm.h"
#include "tcps.h"
#include "log.h"

/*============================================================*/

int tcpc_common_listen_cb(int fd, int event, void *data);
int tcpc_common_event_cb(int fd, int event, void *data);


void tcpc_recycle(struct tcpc *t)
{

    if (t)
    {
        if (-1 != t->m_connect_fd)
        {
            close(t->m_connect_fd);
            t->m_connect_fd = -1;
        }

        if (t->lp)
        {
            loop_fini(t->lp);
            t->lp = NULL;
        }

        bzero(t, sizeof(*t));
    }

    return;
}

struct tcpc *tcpc_init(struct in_addr addr, int port)
{
    int ret = -1;
    int opt = 1;
    struct tcpc *t = NULL;
    struct sockaddr_in stAddr;

    t = (struct tcpc *)malloc(sizeof(struct tcpc));
    if (!t)
    {
        goto err;
    }

    t->m_port = port;
    t->m_connect_fd = -1;
    t->m_process_fragment = NULL;

    t->m_connect_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == t->m_connect_fd)
    {
        goto err;
    }


    bzero(&stAddr, sizeof(struct sockaddr_in));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr.s_addr = htonl(addr.s_addr);
    stAddr.sin_port = htons(t->m_port);

    ret = connect(t->m_connect_fd, (struct sockaddr *)&stAddr,
                  sizeof(struct sockaddr_in));
    if (0 != ret)
    {
        goto err;
    }

    t->lp = loop_init();
    if (!t->lp)
    {
        goto err;
    }



    loop_set_ex_data(t->lp, (void*)t);


    ret = loop_insert(t->lp, t->m_connect_fd, tcpc_common_event_cb);
    if (ret)
    {
        goto err;
    }

    return t;

err:
    tcpc_recycle(t);

    if (t)
    {
        free(t);
        t = NULL;
    }

    return t;
}

void tcpc_fini(struct tcpc *t)
{
    if (t)
    {
        tcpc_recycle(t);
        free(t);
        t = NULL;
    }

    return;
}

int tcpc_set_process_fragment(struct tcpc *t, PROCESS_FRAGMENT pf)
{
    t->m_process_fragment = pf;
    return 0;
}

void *tcpc_thread_running(void *data)
{
    struct tcpc *t = (struct tcpc *)data;
    loop_running(t->lp);

    return NULL;
}


int tcpc_run(struct tcpc *t)
{
    char buf[64];

    snprintf(buf, sizeof(buf), "tcpc_port:%d", t->m_port);

    return thread_reg(buf, tcpc_thread_running, (void *)t);
}



int tcpc_event_cb(struct tcpc *t, int fd, int event, void *data)
{
    if (0 != (event & (EPOLLIN)) )
    {
        struct tcpcs_head head;
        char buf[4096];
        ssize_t len = 0;
        len = recv(fd, &head, sizeof(head), MSG_DONTWAIT);
        if (len != (ssize_t)sizeof(head))
            goto recv_err;

        head.type = htonl(head.type);
        head.length = htonl(head.length);

        len = recv(fd, buf, MIN(head.length, (int)sizeof(buf)), MSG_DONTWAIT);
        if (len > 0)
        {
            buf[head.length - 1] = '\0';
            if (t->m_process_fragment)
            {
                t->m_process_fragment(head, buf);
            }
        }
        else if (!len)
        {

            goto recv_err;
        }
        else
        {
            log_error("recv: fd=%d %s", fd, strerror(errno));
        }

        goto recv_end;
        recv_err:
            loop_erase(t->lp , fd);
            close(fd);
            fd = -1;
        recv_end:
            buf[0] = '\0';

    }

    if (0 != (event & (EPOLLHUP & EPOLLERR)) )
    {
        if (-1 != fd)
        {
            loop_erase(t->lp , fd);
            close(fd);
            (fd) = -1;
        }
    }

    return 0;
}

int tcpc_common_event_cb(int fd, int event, void *data)
{
    struct loop *this_lp = (struct loop *)data;
    struct tcpc *_this = (struct tcpc *)loop_get_ex_data(this_lp);

    return tcpc_event_cb(_this, fd, event, NULL);
}
