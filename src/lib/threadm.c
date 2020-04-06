#include <pthread.h>
#include <sys/syscall.h>
#include <stdlib.h>

#include <scheme.h>
#include <configure.h>

#include <atomic_port.h>

#include "string_ex.h"
#include "threadm.h"
#include "list.h"



int thread_reg(const char *name, void *(*start_routine) (void *), void *arg)
{
    int iRet = -1;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_t id;

    iRet = pthread_create(&id, &attr, start_routine, NULL);
    pthread_attr_destroy(&attr);


    return iRet;
}



