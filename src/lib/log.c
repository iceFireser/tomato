#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <dirent.h>
#include <scheme.h>
#include <semaphore.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <scheme.h>

#include <pthread.h>

#include <configure.h>

#include "threadm.h"

#include "queue_ex.h"
#include "systool.h"
#include "string_ex.h"
#include "loop.h"
#include "log.h"

#define LOG_EVT_ADD 0x00000001


unsigned int g_log_level =  LOG_L_INFO;

#define LOG_MSG_BUF_LEN (1024 * 4 + 256)
#define LOG_MSG_BUF_PUSH_LEN (1024 * 4)

struct tag_log
{
    enum log_level level;
    time_t tm;
    pthread_t tid;
    char szLog[LOG_MSG_BUF_PUSH_LEN];

};

static struct queue_ex  *g_logQueue = NULL;
static pthread_mutex_t g_logLock;
static const char *g_pcDir = "/var/log/agent";
static sem_t log_sem;
static struct loop *log_l = NULL;
static FILE *log_fp = NULL;
static unsigned long log_index = 0;


static void *logThread(void *pData);
static unsigned long log_max_index(void);
static int log_timer_cb(int fd, struct tag_loop_event *pInfo);
static int log_event_cb(int fd, struct tag_loop_event *pInfo);


static void log_recycle(void)
{
    pthread_mutex_destroy(&g_logLock);

    if (log_fp)
    {
        fclose(log_fp);
        log_fp = NULL;
    }

    if (g_logQueue)
    {
        queue_fini(g_logQueue);
        g_logQueue = NULL;
    }

    if (log_l)
    {
        loop_fini(log_l);
        log_l = NULL;
    }

    return;
}

int logInit(const char *pcDir)
{
    int iRet  = -1;
    char szLogPath[PATH_MAX];

    if (pcDir)
    g_pcDir = pcDir;

    sem_init(&log_sem, 0, 0);

    pthread_mutex_init(&g_logLock, NULL);


    if (0 != access(g_pcDir, F_OK))
    {
       mkdir(g_pcDir, 0755);
    }

    log_index = log_max_index();

    /* 打开新日志文件 */
    snprintf(szLogPath, sizeof(szLogPath), "%s/%lu.log", g_pcDir, log_index);
    log_fp = fopen(szLogPath, "a");
    if (!log_fp)
    {
       goto err;
    }

    g_logQueue = queue_init();
    if (!g_logQueue)
    {
        goto err;
    }

    log_l = loop_init();
    if (!log_l)
    {
        goto err;
    }

    loop_set_timer_cb(log_l, 1, log_timer_cb);
    loop_set_event_cb(log_l, log_event_cb);

    iRet = thread_reg("log", logThread, NULL);
    if (iRet)
    {
        goto err;
    }

    return iRet;
err:
    log_recycle();

    return iRet;

}

void logDestroy()
{
    log_recycle();
    return;
}


void setLogMaxLevel(int iLevel)
{

}

void debug_echo(const char *file_name, const void *data, unsigned long data_len)
{

    FILE *fp = fopen(file_name, "a+");
    if (fp)
    {
        fwrite(data, data_len, 1, fp);
        fflush(fp);
        fsync(fileno(fp));
        fclose(fp);
    }

    return;
}

void _log(enum log_level level, const char *format, ...)
{
    va_list ap;
    int iLen = 0;
    int ret;

    struct tag_log *pstLog = NULL;
    struct tag_loop_event e;

    if (NULL == log_l)
        return;
    if((unsigned int)level > g_log_level)
        return;

    pstLog = malloc(sizeof(struct tag_log));
    if (!pstLog)
        return;

    pstLog->level = level;
    time(&pstLog->tm);
    pstLog->tid = pthread_self();

    va_start(ap, format);
    iLen += vscnprintf(pstLog->szLog + iLen, sizeof(pstLog->szLog) - iLen, format, ap);
    va_end(ap);

    iLen += scnprintf(pstLog->szLog + iLen, sizeof(pstLog->szLog) - iLen, "\n");


    pthread_mutex_lock(&g_logLock);
    ret = queue_push(g_logQueue, pstLog);
    if (ret)
    {
        free(pstLog);
        pstLog = NULL;
    }
    pthread_mutex_unlock(&g_logLock);

    e.type = LOG_EVT_ADD;
    e.pData = NULL;
    loop_write_event(log_l, e);

    return;
}

static const char *levelstr(enum log_level level)
{
    const char *pcRet = NULL;
    switch (level)
    {
    case LOG_L_FATAL:
        pcRet = "FATAL";
        break;
    case LOG_L_ERROR:
        pcRet = "ERROR";
        break;
    case LOG_L_INFO:
        pcRet = "INFO";
        break;
    case LOG_L_DEBUG:
        pcRet = "DEBUG";
        break;
    default:
        pcRet = "UNKNOW";
        break;
    }

    return pcRet;
}


static unsigned long log_max_index(void)
{
    DIR *d;
    struct dirent *dent;
    const char *ext;
    unsigned long max_index = 0;
    unsigned long index;
    struct stat s;
    char buf[PATH_MAX];
    char *endp;

    d = opendir(g_pcDir);

    if (!d)
    {
        return 0;
    }

    while ((dent = readdir(d)))
    {
        if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
            continue;

        snprintf(buf, sizeof(buf), "%s/%s", g_pcDir, dent->d_name);
        if (stat(buf, &s) == -1)
            continue;

        if (S_ISREG(s.st_mode))
        {
            ext = extname(dent->d_name);
            if (!ext || strncmp("log", ext, 4))
                continue;

            index = strtoul(dent->d_name, &endp, 10);

            max_index = max_index > index ? max_index : index;
        }
    }

    closedir(d);

    return max_index;
}

static FILE *next_log(unsigned long index)
{
    char szLogPath[PATH_MAX];
    FILE *log_fp = NULL;

    snprintf(szLogPath, sizeof(szLogPath), "%s/%lu.log", g_pcDir, index);
    log_fp = fopen(szLogPath, "w");
    if (!log_fp)
    {
        goto end;
    }
    snprintf(szLogPath, sizeof(szLogPath), "%s/%lu.log", g_pcDir, index - LOG_FILE_MAX);

    if (0 == access(szLogPath, F_OK))
        unlink(szLogPath);


end:
    return log_fp;
}

static int log_flush = 0;

static int log_timer_cb(int fd, struct tag_loop_event *pInfo)
{
    if (log_fp && log_flush)
    {
        fflush(log_fp);
        log_flush = 0;
    }

    return 0;
}

static int log_event_cb(int fd, struct tag_loop_event *pInfo)
{
    static int i;
    FILE *fptmp = NULL;
    ssize_t lLen = 0;
    static char buf[LOG_MSG_BUF_LEN];
    char tmbuf[32];
    time_t tm;
    struct tm tmt;
    struct tag_log stLog;
    struct tag_log *pstLog = NULL;
    char szLogPath[PATH_MAX];
    struct queue_ex *tmQueue = queue_init();
    if (!tmQueue)
        return 0;



    pthread_mutex_lock(&g_logLock);
    if(queue_size(g_logQueue))
    {
        queue_swap(g_logQueue, tmQueue);
    }
    pthread_mutex_unlock(&g_logLock);


    while(queue_size(tmQueue))
    {

        /* To determine whether a file exists */
        if (0 == (i % 100))
        {
            if (log_fp)
            {
                snprintf(szLogPath, sizeof(szLogPath), "%s/%lu.log", g_pcDir, log_index);

                if (0 != access(szLogPath, F_OK))
                {
                    fptmp = next_log(++log_index);
                    if (fptmp)
                    {
                        fclose(log_fp);
                        log_fp = fptmp;
                    }
                    else
                    {
                        ftruncate(fileno(log_fp), 0);
                    }

                }
            }

        }


        /* 日志文件翻滚 */
        if (0 == (i % 1000))
        {
            if (log_fp)
            {
                lLen = ftell(log_fp);
                if (lLen >= SIZE_10M)
                {
                    fptmp = next_log(++log_index);
                    if (fptmp)
                    {
                        fclose(log_fp);
                        log_fp = fptmp;
                    }
                    else
                    {
                        ftruncate(fileno(log_fp), 0);
                    }
                }
            }
        }

        /* 写入日志 */
        pstLog = queue_pop(tmQueue);
        if (!pstLog)
        {
            goto end;
        }

        memcpy(&stLog, pstLog, sizeof(stLog));
        free(pstLog);
        pstLog = NULL;

        tmbuf[0] = '\0';
        tm = stLog.tm;
        localtime_r(&tm,&tmt);
        strftime(tmbuf,sizeof(tmbuf),"%Y-%m-%d_%H:%M:%S",&tmt);

        snprintf(buf, sizeof(buf), "[%s  %s]: tid:%lu %s", tmbuf,
                 levelstr(stLog.level), stLog.tid, stLog.szLog);

        if (log_fp)
        {
            fputs(buf, log_fp);
        }

        i++;
        log_flush++;

    }

end:
    queue_fini(tmQueue);

    return 0;
}



void *logThread(void *pData)
{

    loop_running(log_l);

    if (log_fp)
    {
        fclose(log_fp);
        log_fp = NULL;
    }

    return NULL;
}


