#ifndef __TOMATO_H__
#define __TOMATO_H__

#define TOMATO_PAGE_SIZE (1024 * 4)
#define PAGE_NUM_DEFAULT 4

#define log printf


typedef struct tag_tomato_page
{
    struct tag_tomaoto_address *pstAddress;
    char *pcData;
    unsigned long ulTotalLen;
    unsigned long ulUseLen;
    unsigned long ulfreeLen;
}TOMATO_PAGE;

struct tag_tomaoto_address
{
    unsigned int uiFree;
    unsigned int uiDeviceCode;
    unsigned int uiId;
    unsigned int uiKey;
};

enum tomato_trans_operator
{
    TRANS_WRITE,
    TRANS_READ,
};



#endif

