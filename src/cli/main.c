
#include <stdio.h>
#include <stdlib.h>

#include <libtomato.h>

int parse_command(const char *src, unsigned long len)
{
    char buf[1024];
    char *p;
    char *heap_p[20];
    int heap_index = 0;
    bzero(heap_p, sizeof(heap_p));

    strcncpy(buf, src, sizeof(buf));


    p = strtok(buf, " \n\t");
    if (p)
    {
        heap_p[heap_index++] = p;
    }

    while (p = strtok(NULL, " \n\t"))
    {
        heap_p[heap_index++] = p;
    }

    if (heap_index)
    {
        if (!strncmp("regfile", heap_p[0], strlen("regfile") + 1))
       {
            if (!strncmp("add", heap_p[1], strlen("add") + 1))
            {
                printf("add success \n");
            }
            else if (!strncmp("del", heap_p[1], strlen("del") + 1))
            {
                printf("del success \n");
            }
            else if (!strncmp("show", heap_p[1], strlen("show") + 1))
            {

                printf("<1>  listen.mp4 \n");
                printf("total: 1. \n");
            }
            else
            {
                goto help;
            }

       }
       else if (!strncmp("quit", heap_p[0], strlen("quit") + 1)
                || !strncmp("q", heap_p[0], strlen("q") + 1))
       {
            exit(0);
       }
       // else if xxx
       else
       {
           goto help;
       }

    }



    return 0;
help:
    printf("regfile <add/del/show>\n\n");
    return 0;
}


int read_line(int fd, int iEvent, void *pData)
{
    char buf[1024];
    ssize_t len;
    bzero(buf, sizeof(buf));

    len = read(fd, buf, sizeof(buf));

    if (len)
    {
        parse_command(buf, sizeof(buf));
        fprintf(stderr, "cli>");
    }

    return 0;
}


int read_init()
{
    int ret = -1;
    struct tcpc * c = NULL;
    struct loop * lp = loop_init();
    if (!lp)
    {
        printf("loop init err\n");
        goto err;
    }

#if 0
    c = tcpc_init(0, 8805);
    if (!c)
    {
        printf("tcpc_init err\n");
        goto err;
    }
#endif

    ret = loop_insert(lp, 0, read_line);
    if (ret)
    {
        printf("loop_insert err\n");
        goto err;
    }



    fprintf(stderr, "cli>");

    loop_running(lp);

    printf("loop_running end!\n");

    return 0;

err:
    if (lp)
    {
        loop_fini(lp);
    }

    if (c)
    {
        //tcpc_fini(c);
    }
    return -1;
}


int main()
{
    read_init();

    sleep(3);

	return 0;
}
