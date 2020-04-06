#ifndef _STRING_EX_H_
#define _STRING_EX_H_

#include <string.h>
#include <stdarg.h>

#if defined(__cplusplus)
extern "C"
{
#endif

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
                  char *pcSum, unsigned long ullen);



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
size_t vscnprintf(char *pcBuf, size_t ulLen, const char *pcFmt, va_list pAp);

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
size_t scnprintf(char *pcBuf, size_t ulLen, const char *pcFmt, ...);


/*****************************************************************************
 函 数 名  : strcncpy
 功能描述  : 字符串拷贝,当字符串发生截断时，会在结尾处补0
           比strncpy更安全
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2019年9月10日
    作    者   : haozelong
    修改内容   : 新生成函数

*****************************************************************************/
static inline char *strcncpy(char *dest, const char *src, size_t n)
{
    char *pRet = strncpy(dest, src, n);
    dest[n - 1] = '\0';

    return pRet;
}

/*****************************************************************************
 函 数 名  : extname 
 功能描述  : 返回文件名后缀
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2019年9月10日
    作    者   : haozelong
    修改内容   : 新生成函数

*****************************************************************************/
static inline const char *extname(const char *name)
{
    const char *pcRet = strrchr(name, '.');

    if (pcRet)
    {
        pcRet++;
    }

    return pcRet;
}

#if defined(__cplusplus)
}
#endif

#endif
