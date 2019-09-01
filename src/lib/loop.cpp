
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/eventfd.h>

#include <netinet/in.h>
#include <netinet/tcp.h>


#include <new>


#include "loop.h"

using namespace std;

loop::loop()
{

    m_fd = epoll_create(1);
    m_iEventFd = eventfd(0, 0);
    m_iTimerFd = -1;
    m_iCanExit = 0;

    m_pCBtrigger = nullptr;

    pthread_mutex_init(&m_lock, nullptr);
    pthread_mutex_init(&m_quelock, nullptr);

}

loop::~loop()
{
    if (-1 != m_fd)
    {
        close(m_fd);
        m_fd = -1;
    }

    /* m_iEventFd 由looping回收 */

    pthread_mutex_destroy(&m_lock);
    pthread_mutex_destroy(&m_quelock);
}

int loop::insert(int fd, EVENTCB_P pCB)
{
    int iRet = -1;

    struct epoll_event event;
    pair<unordered_map<int, EVENTCB_P>::iterator, bool> ret;


    pthread_mutex_lock(&m_lock);
    ret = m_hashmap.insert(pair<int,EVENTCB_P>(fd, pCB));
    pthread_mutex_unlock(&m_lock);


    if (true != ret.second)
    {
        iRet = -1;
        goto end;
    }

    event.events = EPOLLIN | EPOLLHUP | EPOLLERR;
    event.data.fd = fd;
    iRet = epoll_ctl(m_fd, EPOLL_CTL_ADD, fd, &event);
    if (0 != iRet)
    {

        pthread_mutex_lock(&m_lock);
        m_hashmap.erase(ret.first);
        pthread_mutex_unlock(&m_lock);
        goto end;
    }



    iRet = 0;
end:
    return iRet;
}

int loop::erase(int fd)
{
    int iRmNum = 0;
    int iRet = -1;

    iRet = epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);

    pthread_mutex_lock(&m_lock);
    iRmNum = m_hashmap.erase(fd);
    pthread_mutex_unlock(&m_lock);

    iRet |= iRmNum > 0 ? 0 : -1;

end:
    return iRet;
}


int loop::looping()
{
    int iRet = -1;
    int fd = -1;
    int iTmpFd = -1;
    int listenFd = -1;
    int acceptFd = -1;
    int iEvents = 0;
    EVENTCB_P pcb = nullptr;
    struct epoll_event event;
    struct epoll_event astEvent[1];


    unordered_map<int, EVENTCB_P>::const_iterator it;

    int iNum = 0;

    if ((-1 == m_fd) || (-1 == m_iEventFd))
    {
        iRet = -1;
        goto end;
    }

    iRet = insert(m_iEventFd, _triggerCB);
    if (0 != iRet)
    {
        goto end;
    }


    for (;;)
    {
        iNum = epoll_wait(m_fd, astEvent, 1, -1);
        if (iNum > 0)
        {
            iTmpFd = astEvent[0].data.fd;
            iEvents = astEvent[0].events;

            if (0 != (iEvents & (EPOLLIN)))
            {
                pcb = nullptr;
                pthread_mutex_lock(&m_lock);
                it = m_hashmap.find(iTmpFd);
                if (it != m_hashmap.end())
                {
                    pcb = it->second;
                }
                pthread_mutex_unlock(&m_lock);

                if (pcb)
                {
                    pcb(iTmpFd, iEvents, (void *)this);
                }

            }

            if (0 != (iEvents & (EPOLLERR | EPOLLHUP)))
            {

                pthread_mutex_lock(&m_lock);
                m_hashmap.erase(iTmpFd);
                pthread_mutex_unlock(&m_lock);

                close(iTmpFd);
            }

            if (m_iCanExit)
            {
                break;
            }
        }

    }



    pthread_mutex_lock(&m_lock);
    for (it = m_hashmap.begin(); it != m_hashmap.end(); ++it)
    {
        fd = it->first;
        epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
    }

    m_hashmap.clear();
    pthread_mutex_unlock(&m_lock);

    iRet = 0;
end:

    return iRet;
}

int loop::_triggerCB(int fd, int iEvent, void *pData)
{
    eventfd_t e;
    int iRet = -1;
    enum trigger_type enType = TRIGGER_INVALID;
    struct tag_trigger stTrigger;
    loop *pThis = (loop*)pData;

    bzero(&stTrigger, sizeof(stTrigger));

    iRet = eventfd_read(fd, &e);
    if (0 == iRet)
    {
        do
        {
            enType = TRIGGER_INVALID;


            pthread_mutex_lock(&pThis->m_quelock);
            if(!pThis->m_que.empty())
            {
                stTrigger = pThis->m_que.front();
                enType = (enum trigger_type)stTrigger.iType;
                pThis->m_que.pop();
            }
            pthread_mutex_unlock(&pThis->m_quelock);

            if (TRIGGER_INVALID == enType)
            {
                break;
            }

            switch(enType)
            {
                case TRIGGER_EXIT:
                {
                    pThis->m_iCanExit = 1;
                    break;
                }
                case TRIGGER_CUSTOM:
                {
                    if (pThis->m_pCBtrigger)
                    {
                        pThis->m_pCBtrigger(fd, &stTrigger);
                    }
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

int loop::trigger(struct tag_trigger stTrigger)
{
    int iRet = 0;
    eventfd_t e = 1;

    pthread_mutex_lock(&m_quelock);
    m_que.push(stTrigger);
    pthread_mutex_unlock(&m_quelock);

    iRet = eventfd_write(m_iEventFd, e);

end:
    return iRet;
}



