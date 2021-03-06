#ifndef __QUEUE_EX_H__
#define __QUEUE_EX_H__

struct queue_node
{
    struct queue_node *pre;
    struct queue_node *next;

    void *value;


};

typedef void (*QUEUE_CLEAR_PF)(void *value);

struct queue_ex
{
    struct queue_node *first;
    struct queue_node *last;

    int num;

    QUEUE_CLEAR_PF clear;

    struct queue_curse *arr;
    int arr_num;
};

struct queue_curse
{
    unsigned long index;
    void *value;
};

struct queue_ex *queue_init();
int queue_push(struct queue_ex *queue, void *value);
void *queue_pop(struct queue_ex *queue);

struct queue_curse *queue_front(struct queue_ex *queue);
struct queue_curse *queue_last(struct queue_ex *queue);


struct queue_curse *queue_begin(struct queue_ex *queue);
struct queue_curse *queue_end(struct queue_ex *queue);
struct queue_curse *queue_next(struct queue_ex *queue, struct queue_curse *);
struct queue_curse *queue_pre(struct queue_ex *queue, struct queue_curse *);

int queue_swap(struct queue_ex *q1, struct queue_ex *q2);

int queue_size(struct queue_ex *queue);
void queue_fini(struct queue_ex *queue);

#endif

