#include <pthread.h>
#include <sys/syscall.h>
#include <stdlib.h>

#include <scheme.h>
#include <configure.h>

#include <atomic_port.h>

#include "string_ex.h"
#include "threadm.h"
#include "list.h"



struct thream_managar
{
    pthread_mutex_t lock;
    struct list_head header;
};

struct thream_managar g_thm_ctx;

static void *mid_routine(void *arg)
{
    struct tag_info_thread *pInfo = (struct tag_info_thread *)arg;

    pInfo->ttid = syscall(SYS_gettid);

    pInfo->status = THREAD_RUNNING;

    if (pInfo->start_routine)
    {
        pInfo->start_routine(pInfo->arg);
    }
    pInfo->status = THREAD_EXIT;

    return NULL;
}
int thread_unreg_by_pthreadid(pthread_t pthid)
{
    struct tag_info_thread* pInfo = NULL;
    int ret = -1;
    pthread_mutex_lock(&g_thm_ctx.lock);
    list_for_each_entry(pInfo,&g_thm_ctx.header,node)
    {
        if(pInfo->id == pthid)
        {
            list_del_init(&pInfo->node);
            free(pInfo);
            ret = 0;
            goto END;
        }
    }
END:
    pthread_mutex_unlock(&g_thm_ctx.lock);
    return ret;
}

int thread_unreg_by_name(const char *name)
{
    struct tag_info_thread* pInfo;
    int ret = -1;
    pthread_mutex_lock(&g_thm_ctx.lock);
    list_for_each_entry(pInfo,&g_thm_ctx.header,node)
    {
        if(!strncmp(name,pInfo->name,strlen(pInfo->name)))
        {
            list_del_init(&pInfo->node);
            free(pInfo);
            ret = 0;
           // goto END;
        }
    }
    pthread_mutex_unlock(&g_thm_ctx.lock);
    return ret;
}

int thread_reg(const char *name, void *(*start_routine) (void *), void *arg)
{
    int iRet = -1;


    struct tag_info_thread* pInfo = (struct tag_info_thread*)malloc(sizeof(struct tag_info_thread));
    if(!pInfo)
    {
        goto end;
    }
    pthread_mutex_lock(&g_thm_ctx.lock);
    list_add_tail(&pInfo->node,&g_thm_ctx.header);
    pthread_mutex_unlock(&g_thm_ctx.lock);

    if (name)
        strcncpy(pInfo->name, name, sizeof(pInfo->name));

    pInfo->start_routine = start_routine;
    pInfo->arg = arg;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    iRet = pthread_create(&pInfo->id, &attr, mid_routine, (void*)pInfo);
    pthread_attr_destroy(&attr);

    //pthread_setname_np(pInfo->id, name);
end:
    return iRet;
}

pthread_t g_main_tid = 0;

void main_thread()
{

    INIT_LIST_HEAD(&g_thm_ctx.header);
    pthread_mutex_init(&g_thm_ctx.lock, NULL);

    g_main_tid = pthread_self();
}

/* 获取线程信息 */
int thread_list(struct tag_info_thread *arr, int num)
{
    long i;

    struct tag_info_thread *pInfo;

    if (!arr || num <= 0)
        return 0;

    arr[0].id = g_main_tid;
    strcpy(arr[0].name, "main");
    arr[0].ttid = getpid();
    arr[0].start_routine = NULL;
    arr[0].status = THREAD_PAUSE;
    pthread_mutex_lock(&g_thm_ctx.lock);
    i = 1;
    list_for_each_entry(pInfo,&g_thm_ctx.header,node)
    {
        if(i< (long)num)
        {
            memcpy(&arr[i],pInfo,sizeof(*pInfo));
            i++;
        }
    }

    pthread_mutex_unlock(&g_thm_ctx.lock);


    return (int)(i);

}


