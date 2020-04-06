#ifndef __ERROR_EX_H__
#define __ERROR_EX_H__



#ifdef  __cplusplus
extern "C" {
#endif

/* 错误码 */
#define EX_ERROR                     -1
#define EX_SUCCESS           0x00000000
#define ERR_PARAM            0x00000001 /* 参数错误 */
#define ERR_BADALLOC         0x00000002 /* 内存申请失败 */
#define ERR_NULLPTR          0x00000003 /* 空指针 */
#define ERR_OUTBOUNDS        0x00000004 /* 值越界 */
#define ERR_NOTEXIST         0x00000005 /* 该项不存在 */
#define ERR_HASEXIST         0x00000006 /* 已经存在 */
#define ERR_SYNC             0x00000007 /* 同步数据失败 */





const char *error_msg(int ret);

#ifdef  __cplusplus
}
#endif


#endif
