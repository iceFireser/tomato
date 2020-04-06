//
// Created by hzl on 19-6-2.
//

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


#include <tomato.h>
#include <scheme.h>
#include <libtomato.h>

#include "front_desk.h"


static struct tcps *g_front_desk_ts=NULL;

int front_frament(struct tcpcs_head head, char *fragment)
{

    return 0;
}

int front_desk_init()
{
    int ret = -1;

    g_front_desk_ts = tcps_init(8573);
    if (!g_front_desk_ts)
    {
        goto end;
    }

    tcps_set_process_fragment(g_front_desk_ts, front_frament);

    ret = tcps_run(g_front_desk_ts);

end:
    return ret;
}


void front_desk_fini()
{
    if (g_front_desk_ts)
    {
        tcps_fini(g_front_desk_ts);
        g_front_desk_ts = NULL;
    }

    return;
}