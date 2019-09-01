#ifndef __SYSTOOL_H__
#define __SYSTOOL_H__

#include <stdio.h>
#include <stdarg.h>

int scnprintf(char *str, size_t size, const char *format, ...);
int vscnprintf(char *str, size_t size, const char *format, va_list ap);



#endif
