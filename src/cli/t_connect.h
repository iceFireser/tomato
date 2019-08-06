#ifndef __T_CONNECT__
#define __T_CONNECT__

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>



#include "tomato.h"

TOMATO_PAGE *tomato_page_malloc(int iPageNum);

unsigned long tomato_page_size();

int tomato_page_free(TOMATO_PAGE *pPage);

int tomato_page_append(TOMATO_PAGE *pPage, void *pData, unsigned int uiLen);
int tomato_page_modify(TOMATO_PAGE *pPage, void *pKey, unsigned int uiKeyLen,
                                void *pData, unsigned int uiDtaLen);
int tomato_page_clear(TOMATO_PAGE *pPage);

void *PAGA_DATA(TOMATO_PAGE *pPage);
struct tag_tomaoto_address *PAGE_ADDRESS(TOMATO_PAGE *pPage);


typedef struct tag_tomato_trans
{
    int iconnectFd;
    int iFlag;
    struct tag_tomato_page **ppPage;
    unsigned int iNum;
    unsigned int iMaxNum;
    unsigned int reserve;
    struct tag_tomaoto_address *pstAddress;

}TOMATO_TRANS;

TOMATO_TRANS *tomato_trans_start(in_addr_t addr);

int tomato_trans_end(TOMATO_TRANS *pTrans);


int tomato_set_address(TOMATO_TRANS *pTrans, struct tag_tomaoto_address *pstAddress);
int tomato_get_address(TOMATO_TRANS *pTrans, struct tag_tomaoto_address *pstAddres);
int tomato_set_operator(TOMATO_TRANS *pTrans, TOMATO_PAGE *pPage, int iNum,
                                  unsigned int iOperator);

int tomato_commit(TOMATO_TRANS *pTrans);

#endif

