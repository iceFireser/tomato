#ifndef __SCHEME_H__
#define __SCHEME_H__


#define SIZE_1K (1024)
#define SIZE_4K (1024 * 4)
#define SIZE_64K (1024 * 64)
#define SIZE_1M (1024 * 1024 * 1)
#define SIZE_4M (1024 * 1024 * 4)
#define SIZE_10M (1024 * 1024 * 10)


#define ARR_SIZE(a) (sizeof(a)/sizeof(a[0]))

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define BIT_MATCH(a,b) ((b)==((a) & (b)))
#define BIT_TEST(a,b)  ((a) & (b))

#ifdef  __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C_BEGIN
#define EXTERN_C_END
#endif


#endif
