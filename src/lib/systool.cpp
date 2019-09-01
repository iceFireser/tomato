
#include "systool.h"

int scnprintf(char *str, size_t size, const char *format, ...)
{
    int iLen = 0;
    va_list ap;
    va_start(ap, format);
    iLen = vscnprintf(str, size, format, ap);
    va_end(ap);

    return iLen;
}
int vscnprintf(char *str, size_t size, const char *format, va_list ap)
{
    int iLen = 0;
    iLen = vsnprintf(str, size, format, ap);

    return (size > iLen) ? iLen : (size - 1);
}


