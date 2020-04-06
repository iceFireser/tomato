#ifndef __LOOP_H_
#define __LOOP_H_


enum trigger_type
{
    TRIGGER_INVALID,
    TRIGGER_EXIT,
    TRIGGER_TIMER,
    TRIGGER_CUSTOM,
    TRIGGER_MAX,
};

struct tag_loop_event
{
    int   type;
    void *pData;
};

struct tag_trigger
{
    enum trigger_type iType;
    struct tag_loop_event e;
};

typedef int (*EVENTCB_P)(int fd, int iEvent, void *pData);

typedef int (*TRIGGERCB_P)(int fd, struct tag_trigger *pstTrigger);

typedef int (*LOOP_CB_P)(int fd, struct tag_loop_event *pInfo);






struct loop
{
    int m_fd;
    int m_iEventFd[2];
    int m_iTimerFd;

    pthread_mutex_t m_lock;
    struct map_ex *m_hashmap;

    pthread_mutex_t m_quelock;
    struct queue_ex *m_que;

    LOOP_CB_P m_event_cb;
    LOOP_CB_P m_timer_cb;

    int m_iCanExit;

    void *m_data;

};

struct loop *loop_init();
void loop_fini(struct loop * lp);

int loop_insert(struct loop * lp, int fd, EVENTCB_P pCB);
/* 拿出fd */
void loop_erase(struct loop * lp,int fd);

/* 设置定时器回调，单位/s */
int loop_set_timer_cb(struct loop * lp, int interval, LOOP_CB_P cb);
int loop_set_event_cb(struct loop * lp, LOOP_CB_P cb);

int loop_write_event(struct loop * lp, struct tag_loop_event e);
int loop_exit_loop(struct loop * lp);

int loop_running(struct loop * lp);

void loop_set_ex_data(struct loop * lp, void *data);
void *loop_get_ex_data(struct loop * lp);

int loop_trigger(struct loop * lp, struct tag_trigger stTrigger);


#endif
