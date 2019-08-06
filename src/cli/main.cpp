#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>


#include "t_connect.h"

#define FLAG_TYPE         0x00000001
#define FLAG_SUBTYPE      0x00000002
#define FLAG_OUT          0x00000004
#define FLAG_ADDRESS      0x00000008
#define FLAG_DATA         0x00000010


struct tag_param
{
    unsigned int uiFlag;
    const char *pcType;    /* -t 类型（读写） */
    const char *pcSubType; /* --subtype */
    const char *pcOut;     /* -o 输出文件目标(out) */
    const char *pcAddress; /* -a 输入地址文件(address) */
    const char *pcData;    /* -d 数据来源文件 */
};


int usage()
{
    printf("-t type\n");
    printf("--subtype\n");
    printf("-o out\n");
    printf("-a address file\n");
    printf("-d input data\n");
    printf("-h help\n");
    printf("example : ./cli -a address.file -d data.file -t write -o out.file\n");
}

int main(int argc, char *argv[])
{
    struct tag_param para;
    int i;
    TOMATO_PAGE *pPage = NULL;
    TOMATO_TRANS *pTrans = NULL;
    int iRet = -1;
    int id;
    char szBuf[256];
    struct tag_tomaoto_address stAddress;
    int fd = -1;
    ssize_t lLen = 0;

    bzero(&para, sizeof(para));
    bzero(&szBuf, sizeof(szBuf));
    bzero(&stAddress, sizeof(stAddress));

    for (i = 1; i < argc; i++)
    {
        if (0 == strcmp(argv[i], "-t"))
        {
            i++;
            para.uiFlag |= FLAG_TYPE;
            para.pcType = argv[i];
        }
        else if (0 == strcmp(argv[i], "--subtype"))
        {
            i++;
            para.uiFlag |= FLAG_TYPE;
            para.pcSubType = argv[i];
        }
         else if (0 == strcmp(argv[i], "-o"))
        {
            i++;
            para.uiFlag |= FLAG_TYPE;
            para.pcOut = argv[i];
        }
         else if (0 == strcmp(argv[i], "-a"))
        {
            i++;
            para.uiFlag |= FLAG_TYPE;
            para.pcAddress = argv[i];
        }
         else if (0 == strcmp(argv[i], "-d"))
        {
            i++;
            para.uiFlag |= FLAG_TYPE;
            para.pcData = argv[i];
        }
        else if (0 == strcmp(argv[i], "-h"))
        {
            usage();
            goto end;
        }
        else
        {
            usage();
            goto end;
        }


    }

    pTrans = tomato_trans_start(inet_addr("127.0.0.1"));
    if (NULL == pTrans)
    {
        goto end;
    }

    pPage = tomato_page_malloc(1);
    if (!pPage)
    {
        goto end;
    }



    if (para.pcAddress)
    {
        fd = open(para.pcAddress, O_RDONLY);
        if (-1 == fd)
        {
            goto end;
        }

        lLen = read(fd, &stAddress, sizeof(stAddress));
        if ((unsigned long)lLen != sizeof(stAddress))
        {
            printf("address not exist!\n");
            goto end;
        }

        close(fd);
        fd = -1;


    }


    if (para.pcData)
    {
        fd = open(para.pcData, O_RDONLY);
        if (-1 == fd)
        {
            goto end;
        }

        lLen = read(fd, PAGA_DATA(pPage), tomato_page_size());
        if ((unsigned long)lLen != tomato_page_size())
        {
            printf("read data error!\n");
            goto end;
        }

        close(fd);
        fd = -1;
    }


    if (0 == strcmp(para.pcType, "write"))
    {
        tomato_set_operator(pTrans, pPage, 1, TRANS_WRITE);
    }
    else if (0 == strcmp(para.pcType, "read"))
    {
        tomato_set_operator(pTrans, pPage, 1, TRANS_READ);
    }
    else
    {
        goto end;
    }



    iRet = tomato_commit(pTrans);
    if (0 != iRet)
    {
        printf("commit error!\n");
        goto end;
    }


    if (para.pcOut)
    {
        fd = open(para.pcOut, O_WRONLY | O_TRUNC | O_CREAT, 0644);
        if (-1 == fd)
        {
            goto end;
        }

        lLen = write(fd, PAGA_DATA(pPage), tomato_page_size());
        if ((unsigned long)lLen != tomato_page_size())
        {
            printf("write data error!\n");
            goto end;
        }

        close(fd);
        fd = -1;

        tomato_get_address(pTrans, &stAddress);
        snprintf(szBuf, sizeof(szBuf), "%s.key", para.pcOut);
        fd = open(szBuf, O_WRONLY | O_TRUNC | O_CREAT, 0644);
        if (-1 == fd)
        {
            goto end;
        }

        lLen = write(fd, &stAddress, sizeof(stAddress));
        if ((unsigned long)lLen != sizeof(stAddress))
        {
            printf("read data error!\n");
            goto end;
        }

        close(fd);
        fd = -1;
    }

    printf("ok...\n");
end:

    if (0 != iRet)
    {
        usage();
    }

    if (pPage)
    {
        tomato_page_free(pPage);
        pPage = NULL;
    }

    if (pTrans)
    {
        tomato_trans_end(pTrans);
        pTrans = NULL;
    }

    if(fd)
    {
        close(fd);
        fd = -1;
    }

    return iRet;
}
