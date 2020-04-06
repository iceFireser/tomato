#include <string.h>

#include "error_ex.h"


const char *error_msg(int ret)
{
    const char * pc = NULL;

    switch(ret)
    {
    case EX_SUCCESS:
        pc = "success";
        break;
    case ERR_PARAM:
        pc = "param error";
            break;
    case ERR_BADALLOC:
        pc = "bad alloc";
        break;
    case ERR_NULLPTR:
        pc = "null ptr";
        break;
    case ERR_OUTBOUNDS:
        pc = "out of bounds";
        break;
    case ERR_NOTEXIST:
        pc = "not exist";
        break;
    case ERR_HASEXIST:
        pc = "has exist";
        break;
    case ERR_SYNC:
        pc = "sync is not complete";
        break;
    default:
        pc = "unknow";
        break;

    }


    return pc;
}


