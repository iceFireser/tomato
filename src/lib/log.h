#ifndef __LOG_H__
#define __LOG_H__

#include <pthread.h>
#ifdef __cplusplus
        extern "C" {
#endif

int logInit(const char *pcDir);
void logDestroy();

enum log_level
{
    LOG_L_FATAL,
    LOG_L_ERROR,
    LOG_L_INFO,
    LOG_L_DEBUG,
    LOG_L_MAX_LEVEL
};
extern  unsigned int g_log_level;

void setLogMaxLevel(int iLevel);

void _log(enum log_level level, const char *format, ...);

#define log_fatal(fmt, arg...) _log(LOG_L_FATAL, "%s-%s-%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##arg)
#define log_error(fmt, arg...) _log(LOG_L_ERROR, "%s-%s-%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##arg)
#define log_info(fmt, arg...)  _log(LOG_L_INFO,  "%s-%s-%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##arg)
#define log_debug(fmt, arg...) _log(LOG_L_DEBUG, "%s-%s-%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##arg)

void debug_echo(const char *file_name, const void *data, unsigned long data_len);

#ifdef __cplusplus
	}
#endif

#endif

