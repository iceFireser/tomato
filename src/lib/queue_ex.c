#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "queue_ex.h"

void queue_default_clear(void *value)
{
    return;
}

struct queue_ex *queue_init()
{
    struct queue_ex *q = (struct queue_ex *)malloc(sizeof(struct queue_ex));
    if (!q)
        return NULL;

    bzero(q, sizeof(*q));

    q->clear = queue_default_clear;

    return q;
}

int queue_push(struct queue_ex *queue, void *value)
{
    struct queue_node *node;

    if (!queue)
        return -1;

    node = (struct queue_node *)malloc(sizeof(struct queue_node));
    if (!node)
        return -1;

    bzero(node, sizeof(*node));

    node->value = value;

    if (!queue->num)
    {
        queue->first = node;
        queue->last = node;
        queue->num = 1;
    }
    else
    {
        node->next = NULL;
        node->pre = queue->last;

        queue->last->next = node;
        queue->last = node;

        queue->num++;
    }


    return 0;
}
void *queue_pop(struct queue_ex *queue)
{
    struct queue_node *node;
    void *value = NULL;

    if (!queue)
        return NULL;

    if (queue->num > 1)
    {
        node = queue->last;
        queue->last = node->pre;
        value = node->value;
        bzero(node, sizeof(*node));
        free(node);
        node = NULL;

        queue->num--;
    }
    else if (1 == queue->num)
    {
        node = queue->last;
        value = node->value;
        bzero(node, sizeof(*node));
        free(node);

        queue->first = NULL;
        queue->last = NULL;
        queue->num = 0;
    }

    return value;
}

struct queue_curse *queue_front(struct queue_ex *queue)
{
    void *value = NULL;

    if ((!queue) || (!queue->first))
        return NULL;

    value = queue->first->value;

    return value;

}
struct queue_curse *queue_last(struct queue_ex *queue)
{
    void *value = NULL;

    if ((!queue) || (!queue->last))
        return NULL;

    value = queue->last->value;

    return value;

}

struct queue_curse *queue_begin(struct queue_ex *queue)
{
    struct queue_node *node = NULL;
    int i;
    unsigned long len = sizeof(struct queue_curse) * (queue->num + 1);
    struct queue_curse *arr = (struct queue_curse *)malloc(len);
    if (!arr)
        return NULL;

    bzero(arr, len);

    for (i = 0, node = queue->first; node && (i < len); node = node->next, i++)
    {
        arr[i].value = node->value;
        arr[i].index = i;
    }

    if (queue->arr)
    {
        free(queue->arr);
        queue->arr = NULL;
    }

    queue->arr = arr;
    queue->arr_num = queue->num;

    return &queue->arr[0];
}
struct queue_curse *queue_end(struct queue_ex *queue)
{
    if ((!queue) || (!queue->arr_num))
        return NULL;

    return &queue->arr[queue->arr_num];
}
struct queue_curse *queue_next(struct queue_ex *queue, struct queue_curse *curse)
{
     if ((!queue) || (!queue->arr_num))
        return NULL;

    if ( (curse->index) <= queue->arr_num)
        return &queue->arr[curse->index + 1];

    return NULL;
}
struct queue_curse *queue_pre(struct queue_ex *queue, struct queue_curse *curse)
{
     if ((!queue) || (!queue->arr_num))
        return NULL;

    if ( (curse->index - 1) >= 0 )
    {
        return &queue->arr[curse->index - 1];
    }

    return NULL;
}


int queue_swap(struct queue_ex *q1, struct queue_ex *q2)
{
    struct queue_ex tmp;

     if (!q1 || !q2)
        return -1;

    memcpy(&tmp, q1, sizeof(struct queue_ex));
    memcpy(q1, q2, sizeof(struct queue_ex));
    memcpy(q2, &tmp, sizeof(struct queue_ex));

    return 0;
}

int queue_size(struct queue_ex *queue)
{
    if (!queue)
        return 0;

    return queue->num;
}
void queue_fini(struct queue_ex *queue)
{
    struct queue_node * node;
    struct queue_node * node_next;

    if (!queue)
        return;

    for (node = queue->first; node; node = node_next)
    {
        node_next = node->next;
        queue->clear(node->value);
        bzero(node, sizeof(*node));
        free(node);

    }

    bzero(queue, sizeof(*queue));
    free(queue);

    return;
}


#if 0

int main(int argc, char *argv[])
{
    int arr[] = {1,2,3,4,5,6,7};
    int ret, i, j;

    struct queue_ex * q = queue_init();
    if (!q)
    {
        printf("queue init null\n");
    }

    for (i = 0; i < 7; i++)
    {
        ret = queue_push(q, (void *)(long)arr[i]);
        if (ret)
        {
            printf("num:%d ret:%d\n", i, ret);
            break;
        }

    }

    struct queue_curse *c;

    for (c = queue_begin(q); c != queue_end(q); c = queue_next(q, c))
    {

        printf("%d ", (int)(long)c->value);
    }


    queue_fini(q);

    return 0;
}


#endif

