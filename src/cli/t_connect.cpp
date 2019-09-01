
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "t_connect.h"


TOMATO_PAGE *tomato_page_malloc(int iPageNum)
{
    int iRet = -1;
    TOMATO_PAGE *pPage = NULL;
    char *pcData = NULL;
    struct tag_tomaoto_address *pstAdd = NULL;

    pPage = (TOMATO_PAGE *)malloc(sizeof(TOMATO_PAGE));
    if (!pPage)
    {
        goto err;
    }

    pcData = (char *)malloc(TOMATO_PAGE_SIZE);
    if (!pcData)
    {
        goto err;
    }

    pstAdd = (struct tag_tomaoto_address *)malloc(sizeof(struct tag_tomaoto_address));
    if (NULL == pstAdd)
    {
        goto err;
    }

    bzero(pstAdd, sizeof(struct tag_tomaoto_address));


    bzero(pPage, sizeof(TOMATO_PAGE));
    pPage->pstAddress = pstAdd;
    pPage->ulfreeLen = TOMATO_PAGE_SIZE;
    pPage->ulTotalLen = TOMATO_PAGE_SIZE;
    pPage->ulUseLen = 0;
    pPage->pcData = pcData;


    goto end;

err:
    if (pPage)
    {
        free(pPage);
        pPage = NULL;
    }


    if (pstAdd)
    {
        free(pstAdd);
        pstAdd = NULL;
    }

end:
    return pPage;
}

unsigned long tomato_page_size()
{
    return TOMATO_PAGE_SIZE;
}

int tomato_page_free(TOMATO_PAGE *pPage)
{
    bzero(PAGA_DATA(pPage), pPage->ulTotalLen);
    if (pPage->pstAddress)
    {
        bzero(pPage->pstAddress, sizeof(struct tag_tomaoto_address));
        free(pPage->pstAddress);
    }

    if (pPage->pcData)
    {
        bzero(pPage->pcData, pPage->ulTotalLen);
        free(pPage->pcData);
    }

    bzero(pPage, sizeof(TOMATO_PAGE));
    free(pPage);
}

int tomato_page_append(TOMATO_PAGE *pPage, void *pcData, unsigned int uiLen)
{
    int iRet = -1;

    if ((!pPage) || (!pcData))
    {
        goto end;
    }

    if (pPage->ulfreeLen > uiLen)
    {
        memcpy(pPage->pcData + pPage->ulUseLen, pcData, uiLen);
        pPage->ulUseLen += uiLen;
        pPage->ulfreeLen -= uiLen;
    }


    iRet = 0;
end:
   return iRet;

}
int tomato_page_modify(TOMATO_PAGE *pPage, void *pKey, unsigned int uiKeyLen,
                                void *pcData, unsigned int uiDtaLen)
{

}
int tomato_page_clear(TOMATO_PAGE *pPage)
{

    if (pPage)
    {
        bzero(PAGA_DATA(pPage), pPage->ulTotalLen);
        bzero(PAGE_ADDRESS(pPage), sizeof(struct tag_tomaoto_address));

        if (PAGA_DATA(pPage))
        {
            free(PAGA_DATA(pPage));
        }

        if (PAGE_ADDRESS(pPage))
        {
            free(PAGE_ADDRESS(pPage));
        }

        free(pPage);
    }

    return 0;
}

void *PAGA_DATA(TOMATO_PAGE *pPage)
{
    return pPage->pcData;
}

struct tag_tomaoto_address *PAGE_ADDRESS(TOMATO_PAGE *pPage)
{
    return pPage->pstAddress;
}

TOMATO_TRANS *tomato_trans_start(in_addr_t addr)
{
    int iRet = -1;
    TOMATO_TRANS *pstTrans = NULL;
    struct tag_tomaoto_address *pstAdd = NULL;
    int fd = -1;
    int i;
    struct sockaddr_in stAddr;

    pstTrans = (struct tag_tomato_trans *)malloc(sizeof(struct tag_tomato_trans));
    if (NULL == pstTrans)
    {
        perror("malloc");
        goto end;
    }

    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (-1 == fd)
    {
        perror("socket");
        goto err;
    }

    bzero(&stAddr, sizeof(struct sockaddr_in));
    stAddr.sin_family = AF_INET;
    stAddr.sin_addr.s_addr = addr;
    stAddr.sin_port = htons(FRONT_PORT);

    iRet = connect(fd, (struct sockaddr *)&stAddr, sizeof(struct sockaddr_in));
    if (0 != iRet)
    {
        perror("connect");
        goto err;
    }


    bzero(pstTrans, sizeof(struct tag_tomato_trans));
    pstTrans->iconnectFd = fd;
    pstTrans->pstAddress = NULL;


    pstAdd = (struct tag_tomaoto_address *)malloc(sizeof(struct tag_tomaoto_address));
    if (NULL == pstAdd)
    {
        goto err;
    }

    bzero(pstAdd, sizeof(struct tag_tomaoto_address));

    pstTrans->pstAddress = pstAdd;

    goto end;


err:
    if (-1 != fd)
    {
        close(fd);
        fd = 1;
    }

    if (pstAdd)
    {
        free(pstAdd);
        pstAdd = NULL;
    }


    if (pstTrans)
    {
        for (i = 0; i < pstTrans->iNum; i++)
        {
            tomato_page_free(pstTrans->ppPage[i]);
        }

        free(pstTrans);

        pstTrans = NULL;
    }



end:
    return pstTrans;
}

int tomato_trans_end(TOMATO_TRANS *pstTrans)
{

    int i;

    /* 1.close */
    if (-1 != pstTrans->iconnectFd)
    {
        close(pstTrans->iconnectFd);
        pstTrans->iconnectFd = -1;
    }


    for (i = 0; i < pstTrans->iNum; i++)
    {
        tomato_page_free(pstTrans->ppPage[i]);
    }

    if (pstTrans->pstAddress)
    {
        free(pstTrans->pstAddress);
        pstTrans->pstAddress = NULL;
    }

    bzero(pstTrans, sizeof(TOMATO_TRANS));
    free(pstTrans);

}



int tomato_set_address(TOMATO_TRANS *pstTrans, struct tag_tomaoto_address *pstAddress)
{
    int iRet = -1;
    struct tag_tomaoto_address *pstAdd = NULL;

    if ((NULL == pstTrans) || (NULL == pstAddress))
    {
        iRet = -1;
        goto end;
    }

    pstAdd = (struct tag_tomaoto_address *)malloc(sizeof(struct tag_tomaoto_address));
    if (NULL == pstAdd)
    {
        goto end;
    }

    memcpy(pstAdd, pstAddress, sizeof(struct tag_tomaoto_address));

    pstTrans->pstAddress = pstAdd;

    iRet = 0;
end:
    return iRet;
}
int tomato_get_address(TOMATO_TRANS *pstTrans, struct tag_tomaoto_address *pstAddress)
{
    int iRet = -1;
    struct tag_tomaoto_address *pstAdd = NULL;

    if ((NULL == pstTrans) || (NULL == pstAddress))
    {
        iRet = -1;
        goto end;
    }

    if (NULL == pstTrans->pstAddress)
    {
        iRet = -1;
        goto end;
    }

    pstAdd = pstTrans->pstAddress;

    memcpy(pstAddress, pstAdd, sizeof(struct tag_tomaoto_address));


    iRet = 0;
end:
    return iRet;

}
int tomato_set_operator(TOMATO_TRANS *pstTrans, TOMATO_PAGE *pPage, int iNum,
                                  unsigned int iOperator)
{
    int iRet = -1;
    struct tag_tomato_page **ppPage = NULL;
    int iFlag = 0;
    int i;

    if ((NULL == pstTrans) || (NULL == pPage))
    {
        goto end;
    }

    switch(iOperator)
    {
        case TRANS_WRITE:
        {
            iFlag = TRANS_WRITE;
            break;
        }
        case TRANS_READ:
        {
            iFlag = TRANS_READ;
            break;
        }
        default:
        {
            iRet = -1;
            goto end;
            break;
        }
    }

    ppPage = (struct tag_tomato_page **)malloc(
                   sizeof(struct tag_tomato_page *) * PAGE_NUM_DEFAULT);

    if (NULL == ppPage)
    {
        iRet = -1;
        goto end;
    }

    pstTrans->ppPage = ppPage;
    pstTrans->iFlag = iFlag;
    pstTrans->iMaxNum = PAGE_NUM_DEFAULT;

    for (i = 0; (i < iNum) && (i < pstTrans->iMaxNum); i++)
    {
        pstTrans->ppPage[pstTrans->iNum++] = pPage;
    }


    iRet = 0;
end:
    return iRet;

}

int tomato_commit(TOMATO_TRANS *pstTrans)
{
    int iRet = -1;
    ssize_t lLen = 0;


    if (!pstTrans)
    {
        goto end;
    }

    /* 1.send */

    switch(pstTrans->iFlag)
    {
        case TRANS_WRITE:
        {
            lLen = send(pstTrans->iconnectFd, &(pstTrans->iFlag), sizeof(int), 0);
            if ((ssize_t)sizeof(int) != lLen)
            {
                log_error("send error lLen:%ld\n", lLen);
            }

            lLen = send(pstTrans->iconnectFd, pstTrans->ppPage[0]->pcData, TOMATO_PAGE_SIZE, 0);
            if (TOMATO_PAGE_SIZE != lLen)
            {
                log_error("send error lLen:%ld\n", lLen);
            }

            lLen = recv(pstTrans->iconnectFd, pstTrans->pstAddress,
                        sizeof(struct tag_tomaoto_address), MSG_WAITALL);
            if ((ssize_t)sizeof(struct tag_tomaoto_address) != lLen)
            {
                log_error("recv error lLen:%ld\n", lLen);
            }

            break;
        }
        case TRANS_READ:
        {
            lLen = send(pstTrans->iconnectFd, &(pstTrans->iFlag), sizeof(int), 0);
            if (sizeof(int) != lLen)
            {
                log_error("send error lLen:%ld\n", lLen);
            }

            lLen = send(pstTrans->iconnectFd, pstTrans->pstAddress,
                        sizeof(struct tag_tomaoto_address), 0);
            if ((ssize_t)sizeof(struct tag_tomaoto_address) != lLen)
            {
                log_error("send error lLen:%ld\n", lLen);
            }

            lLen = recv(pstTrans->iconnectFd, pstTrans->ppPage[0]->pcData, TOMATO_PAGE_SIZE,
                        MSG_WAITALL);
            if (TOMATO_PAGE_SIZE != lLen)
            {
                log_error("recv error lLen:%ld\n", lLen);
            }

            break;
        }
        default:
        {
            iRet = -1;
            goto end;
            break;
        }
    }

    iRet = 0;
end:
    return iRet;
}



