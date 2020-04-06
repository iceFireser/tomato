#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "string_ex.h"

#define STRING_EX_BUF_1K 1024

/*****************************************************************************
 函 数 名  : str_replace
 功能描述  : 替换字符串的子字符串为另一个字符串
 输入参数  : const char *pcSrc          ：输入    源字符串
           const char *pcReplace    ：输入 将要替换的源字符串
           const char *pcDest       ：输入 将要替换的目的字符串
           char *pcSum              ：输出 替换后的字符串
           unsigned long ullen      : pcSum 的缓冲区大小
 输出参数  : 无
 返 回 值  : char
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年11月19日
    作    者   : haozelong
    修改内容   : 新生成函数

*****************************************************************************/
char *str_replace(const char *pcSrc, const char *pcReplace, const char *pcDest,
                  char *pcSum, unsigned long ullen)
{
    char * pcStr = NULL;
    char szBuf[STRING_EX_BUF_1K];
    unsigned long ulMinLen = 0;

    if ((NULL == pcSrc)
       || (NULL == pcReplace)
       || (NULL == pcDest)
       || (STRING_EX_BUF_1K <= strlen(pcSrc)))
    {
        return NULL;
    }

    strncpy(szBuf, pcSrc, STRING_EX_BUF_1K);
    szBuf[STRING_EX_BUF_1K - 1] = '\0';
    pcStr = strstr(szBuf, pcReplace);
    if (NULL == pcStr)
    {
        return NULL;
    }
    *pcStr = '\0';

    pcSrc += strlen(pcReplace);

    ulMinLen = strlen(szBuf) + strlen(pcDest) + strlen(pcSrc) + 1; /* 1 for '\0' */
    if (ullen < ulMinLen)
    {
        return NULL;
    }

    snprintf(pcSum, ullen, "%s%s%s", szBuf, pcDest, pcSrc);

    return pcSum;
}

/*****************************************************************************
 函 数 名  : vscnprintf
 功能描述  : 字符串拼装函数，返回值为已经拼装的长度
           在循环调用时比vsnprintf更安全
 输入参数  : IN char *pcBuf
             IN size_t ulLen
             IN const char *pcFmt
             IN va_list pAp
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年11月19日
    作    者   : haozelong
    修改内容   : 新生成函数

*****************************************************************************/
size_t vscnprintf(char *pcBuf, size_t ulLen, const char *pcFmt, va_list pAp)
{
    size_t ulNum = vsnprintf(pcBuf, ulLen, pcFmt, pAp);
    size_t ulFinal = (ulNum> ulLen) ? ulLen : ulNum;

    return ulFinal;
}

/*****************************************************************************
 函 数 名  : vscnprintf
 功能描述  : 字符串拼装函数，返回值为已经拼装的长度
           在循环调用时比snprintf更安全
 输入参数  : IN char *pcBuf
             IN size_t ulLen
             IN const char *pcFmt
             IN va_list pAp
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2018年11月19日
    作    者   : haozelong
    修改内容   : 新生成函数

*****************************************************************************/
size_t scnprintf(char *pcBuf, size_t ulLen, const char *pcFmt, ...)
{
    va_list pAp;
    va_start(pAp ,pcFmt);
    size_t ulFinal = vscnprintf(pcBuf, ulLen, pcFmt, pAp);
    va_end(pAp);

    return ulFinal;
}




