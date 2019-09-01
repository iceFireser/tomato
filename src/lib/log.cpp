
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/stat.h>

#include <queue>

#include "systool.h"
#include "loop.h"
#include "log.h"

using namespace std;

struct tag_log
{
    time_t tm;
    char szLog[256];

};

static queue<struct tag_log> g_logQueue;
static pthread_mutex_t g_logLock;
static pthread_t g_logTid = -1;
static const char *g_pcDir = "/var/log/tmt";

static void *logThread(void *pData);

int logInit(const char *pcDir)
{
    int iRet  = -1;

    if (pcDir)
    g_pcDir = pcDir;

    pthread_mutex_init(&g_logLock, nullptr);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    iRet = pthread_create(&g_logTid, &attr, logThread, nullptr);
    pthread_attr_destroy(&attr);


    return iRet;

}
void logDestroy()
{
    pthread_mutex_destroy(&g_logLock);

}


void setLogMaxLevel(int iLevel)
{

}

void log(const char *format, ...)
{
    va_list ap;
    int iLen = 0;
    time_t tm = 0;
    char szBuf[32];

    szBuf[0] = '\0';
    struct tag_log stLog;

    time(&tm);
    iLen += scnprintf(stLog.szLog + iLen, sizeof(stLog.szLog) - iLen,
                      "%s-", ctime_r(&tm, szBuf));

    va_start(ap, format);
    iLen += vscnprintf(stLog.szLog + iLen, sizeof(stLog.szLog) - iLen, format, ap);
    va_end(ap);

    iLen += scnprintf(stLog.szLog + iLen, sizeof(stLog.szLog) - iLen, "\n");


    pthread_mutex_lock(&g_logLock);
    g_logQueue.push(stLog);
    pthread_mutex_unlock(&g_logLock);
end:
    return;
}

void *logThread(void *pData)
{
    int i;
    queue<struct tag_log> tmQueue;
    struct tag_log stLog;
    FILE *fp = NULL;
    char szLogPath[64];
    ssize_t lLen = 0;

    if (0 != access(g_pcDir, F_OK))
    {
        mkdir(g_pcDir, 0755);
    }

    snprintf(szLogPath, sizeof(szLogPath), "%s/%s", g_pcDir, "log1");
    fp = fopen(szLogPath, "w+");
    if (!fp)
    {
        goto end;
    }


    while(1)
    {

        pthread_mutex_lock(&g_logLock);
        while(!g_logQueue.empty())
        {
            stLog = g_logQueue.front();
            g_logQueue.pop();
            tmQueue.push(stLog);

        }
        pthread_mutex_unlock(&g_logLock);


        while(!tmQueue.empty())
        {
            stLog = tmQueue.front();
            tmQueue.pop();
            fputs(stLog.szLog, fp);
        }

        sleep(1);
    }


    fclose(fp);
    fp = nullptr;

end:
    return nullptr;
}


