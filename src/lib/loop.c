
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <error.h>


#include <scheme.h>
#include <configure.h>
#include <atomic_port.h>

#include "threadm.h"
#include "map_ex.h"
#include "queue_ex.h"

#include "loop.h"

struct tag_info_timer
{
    void (*time_up) (void*);
    void *arg;
    int interval;
    uint64_t beat;

};

static struct tag_info_timer g_timer_arr[TIMER_MAX];
static atomic_t g_timer_index = (atomic_t)0;
static uint64_t g_timer_beat = 0;


static int loop_triggerCB(int fd, int iEvent, void *pData);
static void *timer_thread(void *arg);
static void loop_time_up(void *arg);


/* 定时器注册 */
static int timer_reg(void (*time_up)(void *arg), void *arg, int interval)
{
    int iRet = -1;
    long index = atomic_inc(&g_timer_index);
    struct tag_info_timer *pInfo = NULL;

    index--;

    if (index < 0 || (long)ARR_SIZE(g_timer_arr) <= index)
        goto end;

    pInfo = &g_timer_arr[index];

    pInfo->time_up = time_up;
    pInfo->interval = interval;
    pInfo->beat = time(NULL);
    pInfo->arg = arg;

    iRet = 0;
end:
   return iRet;
}

static unsigned long tm_s_count = 0;


static void timer_schedule()
{
    long index = atomic_read(&g_timer_index);
    uint64_t beat = 0;
    int i;
    struct tag_info_timer *pInfo = NULL;

    tm_s_count++;

    g_timer_beat = time(NULL);

    for (i = 0; i < index; i++)
    {
        pInfo = &g_timer_arr[i];
        beat = g_timer_beat - pInfo->beat;

        if (beat && (beat % (pInfo->interval) == 0))
        {
            if (pInfo->time_up)
            {
                pInfo->time_up(pInfo->arg);
            }
        }
    }


}


void *timer_thread(void *arg)
{

    struct epoll_event evs[1];
    int num;

    int fd = epoll_create(4);
    if (-1 == fd)
    {
        goto end;
    }


    while(1)
    {

        num = epoll_wait(fd, evs, 1, 1000);

        if ((0 == num) || (EINTR == errno))
        {
            timer_schedule();
        }

        //if (0)(void)
    }

end:
    return NULL;
}

static void timer_once_cb(void)
{
    thread_reg("loop_timer", timer_thread, NULL);
}

static int timer_init_once()
{
    static pthread_once_t once = PTHREAD_ONCE_INIT;

    pthread_once(&once, timer_once_cb);

    return 0;
}

static void loop_clear(struct loop *lp)
{
    if (lp)
    {
        if (-1 != lp->m_fd)
        {
            close(lp->m_fd);
        }

        if (-1 != lp->m_iEventFd[0])
        {
            close(lp->m_iEventFd[0]);
        }

        if (-1 != lp->m_iEventFd[1])
        {
            close(lp->m_iEventFd[1]);
        }

        if (lp->m_hashmap)
        {
            map_fini(lp->m_hashmap);
            lp->m_hashmap = NULL;
        }

        if (lp->m_que)
        {
            queue_fini(lp->m_que);
            lp->m_que = NULL;
        }

        pthread_mutex_destroy(&lp->m_lock);
        pthread_mutex_destroy(&lp->m_quelock);

        bzero(lp, sizeof(struct loop));
    }

    return;
}


struct loop *loop_init()
{
    int ret;
    int iEventFd[2];
    struct loop *lp = malloc(sizeof(struct loop));
    if (!lp)
    {
        goto err;
    }
    bzero(lp, sizeof(struct loop));


    lp->m_fd = epoll_create(1);
    if (-1 == lp->m_fd)
    {
        goto err;
    }
    lp->m_iTimerFd = -1;
    lp->m_iCanExit = 0;
    lp->m_data = NULL;
    lp->m_iEventFd[0] = -1;
    lp->m_iEventFd[1] = -1;

    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, iEventFd);
    if (ret)
    {
        goto err;
    }

    lp->m_iEventFd[0] = iEventFd[0];
    lp->m_iEventFd[1] = iEventFd[1];

    lp->m_timer_cb = NULL;
    lp->m_event_cb = NULL;

    lp->m_hashmap = map_init(NULL, NULL);
    if (!lp->m_hashmap)
    {
        goto err;
    }

    lp->m_que = queue_init();
    if (!lp->m_hashmap)
    {
        goto err;
    }

    pthread_mutex_init(&lp->m_lock, NULL);
    pthread_mutex_init(&lp->m_quelock, NULL);

    timer_init_once();

    return lp;

err:

    loop_clear(lp);

    if (lp)
    {
        free(lp);
        lp = NULL;
    }

    return lp;
}


void loop_fini(struct loop * lp)
{
    loop_clear(lp);

    if (lp)
    {
        free(lp);
        lp = NULL;
    }

    return;
}


int loop_insert(struct loop * lp, int fd, EVENTCB_P pCB)
{
    int iRet = -1;

    struct epoll_event event;


    pthread_mutex_lock(&lp->m_lock);
    iRet = map_insert(lp->m_hashmap, (void *)(long)fd, pCB);
    pthread_mutex_unlock(&lp->m_lock);

    if (iRet)
    {
        iRet = -1;
        goto end;
    }

    event.events = EPOLLIN | EPOLLHUP | EPOLLERR;
    event.data.fd = fd;
    iRet = epoll_ctl(lp->m_fd, EPOLL_CTL_ADD, fd, &event);
    if (0 != iRet)
    {

        pthread_mutex_lock(&lp->m_lock);
        map_erase(lp->m_hashmap, (void *)(long)fd);
        pthread_mutex_unlock(&lp->m_lock);
        goto end;
    }



    iRet = 0;
end:
    return iRet;
}

void loop_erase(struct loop * lp, int fd)
{
    epoll_ctl(lp->m_fd, EPOLL_CTL_DEL, fd, NULL);

    pthread_mutex_lock(&lp->m_lock);
    map_erase(lp->m_hashmap, (void *)(long)fd);
    pthread_mutex_unlock(&lp->m_lock);

    return;
}


int loop_running(struct loop * lp)
{
    int iRet = -1;
    int fd = -1;
    int iTmpFd = -1;
    int iEvents = 0;
    EVENTCB_P pcb = NULL;
    struct epoll_event astEvent[1];
    struct map_curse *curse;


    int iNum = 0;

    if ((-1 == lp->m_fd)
        || (-1 == lp->m_iEventFd[0]) || (-1 == lp->m_iEventFd[0]))
    {
        iRet = -1;
        goto end;
    }

    iRet = map_insert(lp->m_hashmap, (void *)(long)lp->m_iEventFd[1], loop_triggerCB);
    if (0 != iRet)
    {
        goto end;
    }


    for (;;)
    {
        iNum = epoll_wait(lp->m_fd, astEvent, 1, -1);
        if (iNum > 0)
        {
            iTmpFd = astEvent[0].data.fd;
            iEvents = astEvent[0].events;

            if (0 != (iEvents & (EPOLLIN)))
            {
                pcb = NULL;
                pthread_mutex_lock(&lp->m_lock);

                curse = map_find(lp->m_hashmap, (void *)(long)iTmpFd);
                if (curse)
                {
                    pcb = (EVENTCB_P)curse;
                }
                pthread_mutex_unlock(&lp->m_lock);

                if (pcb)
                {
                    pcb(iTmpFd, iEvents, (void *)lp);
                }

            }

            if (0 != (iEvents & (EPOLLERR | EPOLLHUP)))
            {

                if (iTmpFd == lp->m_iEventFd[0])
                {
                    map_erase(lp->m_hashmap, (void *)(long)iTmpFd);
                    close(iTmpFd);
                    lp->m_iEventFd[0] = -1;
                }

                if (iTmpFd == lp->m_iEventFd[1])
                {
                    map_erase(lp->m_hashmap, (void *)(long)iTmpFd);
                    close(iTmpFd);
                    lp->m_iEventFd[1] = -1;
                }


            }

            if (lp->m_iCanExit)
            {
                break;
            }
        }

    }



    pthread_mutex_lock(&lp->m_lock);
    for (curse = map_begin(lp->m_hashmap); curse !=map_end(lp->m_hashmap);
         curse = map_next(lp->m_hashmap, curse))
    {
        fd = (int)(long)curse->key;
        epoll_ctl(lp->m_fd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
    }

    map_clear(lp->m_hashmap);
    pthread_mutex_unlock(&lp->m_lock);

    iRet = 0;
end:

    return iRet;
}

static int loop_triggerCB(int fd, int iEvent, void *pData)
{
    long e, l_timer;
    int iRet = -1;
    enum trigger_type enType = TRIGGER_INVALID;
    struct tag_trigger *ptrig = NULL;
    struct loop* lp = (struct loop*)pData;
    struct tag_trigger stTrigger;
    bzero(&stTrigger, sizeof(stTrigger));

    iRet = read(fd, &e, sizeof(e));
    if (sizeof(e) == iRet)
    {
        do
        {
            l_timer = 0;
            enType = TRIGGER_INVALID;

            pthread_mutex_lock(&lp->m_quelock);
            if(queue_size(lp->m_que))
            {
                ptrig = queue_pop(lp->m_que);
                if (ptrig)
                {
                    enType = (enum trigger_type)ptrig->iType;
                    memcpy(&stTrigger, ptrig, sizeof(stTrigger));
                    free(ptrig);
                    ptrig = NULL;
                }

            }
            pthread_mutex_unlock(&lp->m_quelock);

            if (TRIGGER_INVALID == enType)
            {
                break;
            }

            switch(enType)
            {
                case TRIGGER_EXIT:
                {
                    lp->m_iCanExit = 1;
                    break;
                }
                case TRIGGER_CUSTOM:
                {
                    if (lp->m_event_cb)
                    {
                        lp->m_event_cb(fd, &stTrigger.e);
                    }
                    break;
                }
                case TRIGGER_TIMER:
                {
                    /* 定时器回调中阻塞后丢弃之前的消息 */
                    if ((!l_timer) && (lp->m_timer_cb))
                    {
                        lp->m_timer_cb(fd, NULL);
                    }

                    l_timer++;

                    break;
                }

                default:
                {
                    break;
                }
            }

        }while(1);




    }


    return iRet;
}

int loop_trigger(struct loop* lp, struct tag_trigger stTrigger)
{
    int iRet = -1;
    ssize_t lLen = 0;
    long e = 1;
    struct tag_trigger * ptrig;

    pthread_mutex_lock(&lp->m_quelock);
    ptrig = malloc(sizeof(struct tag_trigger));
    if (ptrig)
    {
        memcpy(ptrig, &stTrigger, sizeof(struct tag_trigger));
        queue_push(lp->m_que, ptrig);
    }
    pthread_mutex_unlock(&lp->m_quelock);

    lLen = write(lp->m_iEventFd[0], &e, sizeof(e));

    if (sizeof(e) == lLen)
        iRet = 0;

    return iRet;
}

int loop_write_event(struct loop* lp, struct tag_loop_event e)
{
    struct tag_trigger stTrigger;

    stTrigger.iType = TRIGGER_CUSTOM;
    stTrigger.e = e;

    return loop_trigger(lp, stTrigger);
}

int loop_exit_loop(struct loop* lp)
{
    struct tag_trigger stTrigger;

    stTrigger.iType = TRIGGER_EXIT;
    //stTrigger.e = e;

    return loop_trigger(lp, stTrigger);
}




int loop_set_timer_cb(struct loop* lp, int interval, LOOP_CB_P cb)
{
    lp->m_timer_cb = cb;

    return timer_reg(loop_time_up, (void*)lp, interval);
}


int loop_set_event_cb(struct loop* lp, LOOP_CB_P cb)
{
    lp->m_event_cb = cb;

    return 0;
}

static void loop_time_up(void *arg)
{
    struct loop* lp = (struct loop*)arg;
    struct tag_trigger tri;


    if (lp && lp->m_timer_cb)
    {
        bzero(&tri, sizeof(tri));
        tri.iType =  TRIGGER_TIMER;

        loop_trigger(lp, tri);
    }

}

void loop_set_ex_data(struct loop* lp, void *data)
{
    lp->m_data = data;
    return;
}

void *loop_get_ex_data(struct loop* lp)
{
    return lp->m_data;
}

