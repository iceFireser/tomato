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

int tcps_common_listen_cb(int fd, int event, void *data);
int tcps_common_event_cb(int fd, int event, void *data);


void tcps_recycle(struct tcps *t)
{

    if (t)
    {
        if (-1 != t->m_listen_fd)
        {
            close(t->m_listen_fd);
            t->m_listen_fd = -1;
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

struct tcps *tcps_init(int port)
{
    int ret = -1;
    int opt = 1;
    struct tcps *t = NULL;
    struct sockaddr_in stAddr;

    t = (struct tcps *)malloc(sizeof(struct tcps));
    if (!t)
    {
        goto err;
    }

    t->lp = loop_init();
    if (!t->lp)
    {
        goto err;
    }

    t->m_port = port;
    t->m_listen_fd = -1;
    t->m_process_fragment = NULL;

    loop_set_ex_data(t->lp, (void*)t);

    t->m_listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == t->m_listen_fd)
    {
        goto err;
    }

    ret = setsockopt(t->m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret)
    {
        log_error("setsockopt: %s", strerror(errno));
        goto err;
    }

    bzero(&stAddr, sizeof(struct sockaddr_in));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    stAddr.sin_port = htons(t->m_port);
    ret = bind(t->m_listen_fd, (struct sockaddr *)&stAddr, sizeof(stAddr));
    if (0 != ret)
    {
        goto err;
    }

    ret = listen(t->m_listen_fd, 256);
    if (0 != ret)
    {
        goto err;
    }

    ret = loop_insert(t->lp, t->m_listen_fd, tcps_common_listen_cb);
    if (ret)
    {
        goto err;
    }

    return t;

err:
    tcps_recycle(t);

    if (t)
    {
        free(t);
        t = NULL;
    }

    return t;
}

void tcps_fini(struct tcps *t)
{
    if (t)
    {
        tcps_recycle(t);
        free(t);
        t = NULL;
    }

    return;
}

int tcps_set_process_fragment(struct tcps *t, PROCESS_FRAGMENT pf)
{
    t->m_process_fragment = pf;
    return 0;
}

void *tcps_thread_running(void *data)
{
    struct tcps *t = (struct tcps *)data;
    loop_running(t->lp);

    return NULL;
}


int tcps_run(struct tcps *t)
{
    char buf[64];

    snprintf(buf, sizeof(buf), "tcps_port:%d", t->m_port);

    return thread_reg(buf, tcps_thread_running, (void *)t);
}



int tcps_event_cb(struct tcps *t, int fd, int event, void *data)
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

int tcps_listen_cb(struct tcps *t, int fd, int event, void *data)
{

    if (0 != (event & (EPOLLIN)) )
    {
        if (t->m_listen_fd == fd)
        {
            struct sockaddr_in client_addr;
            socklen_t sock_len = sizeof(client_addr);
            int accept_fd = accept(fd, (struct sockaddr*)&client_addr, &sock_len);
            if (-1 != accept_fd)
            {
                loop_insert(t->lp, accept_fd, tcps_common_event_cb);
            }
            else
            {
                log_error("accept: fd=%d %s", fd, strerror(errno));
            }
        }
    }


    if (0 != (event & (EPOLLHUP & EPOLLERR)) )
    {
        if (-1 != fd)
        {
            loop_erase(t->lp, fd);
            close(fd);
        }
    }

    return 0;
}

int tcps_common_listen_cb(int fd, int event, void *data)
{
    struct loop *this_lp = (struct loop *)data;

    struct tcps *_this = (struct tcps *)loop_get_ex_data(this_lp);
    return tcps_listen_cb(_this, fd, event, NULL);
}

int tcps_common_event_cb(int fd, int event, void *data)
{
    struct loop *this_lp = (struct loop *)data;
    struct tcps *_this = (struct tcps *)loop_get_ex_data(this_lp);

    return tcps_event_cb(_this, fd, event, NULL);
}
