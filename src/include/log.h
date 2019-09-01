#ifndef __LOG_H__
#define __LOG_H__


int logInit(const char *pcDir);
void logDestroy();

enum log_level
{
    LOG_FATAL,
    LOG_ERROR,
    LOG_INFO,
    LOG_DEBUG,
    LOG_MAX_LEVEL
};

void setLogMaxLevel(int iLevel);

void log(const char *format, ...);

#define log_fatal(fmt, arg...) log("fatal:%s-%s-%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##arg)
#define log_error(fmt, arg...) log("error:%s-%s-%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##arg)
#define log_info(fmt, arg...)  log("info:%s-%s-%d " fmt,  __FILE__, __FUNCTION__, __LINE__, ##arg)
#define log_debug(fmt, arg...) log("debug:%s-%s-%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##arg)

#endif

