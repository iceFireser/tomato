#ifndef __THREAD_M__
#define __THREAD_M__

#include <pthread.h>
#include <unistd.h>

#include "list.h"
#ifdef __cplusplus
extern "C" {
#endif

enum thread_status
{
    THREAD_WAIT_RUNNING,
    THREAD_PAUSE,
    THREAD_RUNNING,
    THREAD_EXIT,
    THREAD_STATUS_MAX
};


struct tag_info_thread
{
    struct list_head node;

    pid_t ttid;
    pthread_t id;
    void *(*start_routine) (void *);
    void *arg;
    char name[64];
    enum thread_status status;
};

/* 线程注册 */
int thread_reg(const char *name, void *(*start_routine) (void *), void *arg);



#ifdef __cplusplus
}
#endif



#endif
