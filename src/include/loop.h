#ifndef __LOOP_H_
#define __LOOP_H_

#include <pthread.h>


#include <unordered_map>
#include <queue>


enum trigger_type
{
    TRIGGER_INVALID,
    TRIGGER_EXIT,
    TRIGGER_CUSTOM,
    TRIGGER_MAX,
};

struct tag_trigger
{
    int   iType;
    int   iSubType;
    void *pData;
};

typedef int (*EVENTCB_P)(int fd, int iEvent, void *pData);
typedef int (*TRIGGERCB_P)(int fd, struct tag_trigger *pstTrigger);


class loop
{
public:
    loop();
    ~loop();

    int insert(int fd, EVENTCB_P pCB);


    int triggleCB(TRIGGERCB_P pCB);
    int trigger(struct tag_trigger stTrigger);

    int looping();
private:
    int erase(int fd);

    static int _triggerCB(int fd, int iEvent, void *pData);

private:
    int m_fd;
    int m_iEventFd;
    int m_iTimerFd;

    pthread_mutex_t m_lock;
    std::unordered_map<int, EVENTCB_P> m_hashmap;

    pthread_mutex_t m_quelock;
    std::queue<struct tag_trigger> m_que;

    TRIGGERCB_P m_pCBtrigger;

    int m_iCanExit;


};



#endif
