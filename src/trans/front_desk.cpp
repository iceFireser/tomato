//
// Created by hzl on 19-6-2.
//

#include <unistd.h>

#include <pthread.h>

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <new>


#include "front_desk.h"

class reception
{
public:
    reception(int fd)
    {
        m_iFd = fd;
        m_eventFd = -1;
    }
    ~reception(){}
    int start(void);
    void run(void);
    int processEvent(int fd, int event);
    int processMsg(int fd);

    int m_iFd;
    int m_eventFd;

private:
    pthread_t m_tid;

private:
    static void *callRun(void *pData);

};

int reception::start()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&m_tid, &attr, reception::callRun, (void *)this);
    pthread_attr_destroy(&attr);
}

void *reception::callRun(void *pData)
{
    reception *pThis = (reception *)pData;

    pThis->run();

    return nullptr;
}

void reception::run()
{
    int iRet = -1;
    int fd = -1;
    int listenFd = -1;
    int acceptFd = -1;
    struct epoll_event event;
    struct epoll_event astEvent[1];
    int iNum = 0;
    int i;

    fd = epoll_create(4);
    if (- 1 == fd)
    {
        goto end;
    }


    if (-1 == m_iFd)
    {
        goto end;
    }

    event.events = EPOLLIN | EPOLLHUP | EPOLLERR;
    event.data.fd = m_iFd;
    iRet = epoll_ctl(fd, EPOLL_CTL_ADD, m_iFd, &event);
    if (0 != iRet)
    {
        goto end;
    }

    m_eventFd =fd;
    for (;;)
    {
        iNum = epoll_wait(fd, astEvent, 1, -1);
        if (iNum > 0)
        {
            /* process event */
            iRet = processEvent(astEvent[0].data.fd, astEvent[0].events);
            if (0 != iRet)
            {
                break;
            }

        }
        else if (iNum < 0)
        {
            break;
        }

    }


    end:
    if (-1 != fd)
    {
        close(fd);
        fd = -1;
    }

    if (-1 != m_iFd)
    {
        close(m_iFd);
        m_iFd = -1;
    }

    delete this;

    return;
}

int reception::processEvent(int fd, int event)
{

    if (0 != (event & EPOLLIN))
    {
        processMsg(fd);

    }



    if (0 != (event & (EPOLLHUP | EPOLLERR)))
    {
        epoll_ctl(m_eventFd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        return -1;
        /* close connect */
    }

    return 0;
}

int reception::processMsg(int fd)
{


    return 0;
}

/*============================================================*/



int front_desk::start()
{
    run(nullptr);

    return 0;
}


void *front_desk::run(void *pData)
{
    int iRet = -1;
    int fd = -1;
    int listenFd = -1;
    int acceptFd = -1;
    struct epoll_event event;
    struct epoll_event astEvent[1];
    int iNum = 0;


    fd = epoll_create(4);
    if (- 1 == fd)
    {
        goto end;
    }

    listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == listenFd)
    {
        goto end;
    }

    iRet = listen(listenFd, 256);
    if (0 != iRet)
    {
        goto end;
    }

    event.events = EPOLLIN | EPOLLHUP | EPOLLERR;
    event.data.fd = listenFd;
    iRet = epoll_ctl(fd, EPOLL_CTL_ADD, listenFd, &event);
    if (0 != iRet)
    {
        goto end;
    }

    for (;;)
    {
        iNum = epoll_wait(fd, astEvent, 1, -1);
        if (iNum > 0)
        {
            acceptFd = accept(listenFd, nullptr, nullptr);
            reception *pRe = new(std::nothrow) reception(acceptFd) ;
            pRe->start();
        }
        else if (iNum < 0)
        {
            break;
        }
    }


end:
    if (-1 != fd)
    {
       close(fd);
       fd = -1;
    }

    if (-1 != listenFd)
    {
       close(listenFd);
       listenFd = -1;
    }


    return nullptr;
}