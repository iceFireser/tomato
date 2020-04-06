#ifndef __MAP_EX_H__
#define __MAP_EX_H__


typedef int (*key_compare)(void *key1, void *key2);
typedef int (*key_value_clear)(void *key1, void *value);

struct avl_tree;

struct map_ex;

struct map_curse
{
    void *private;
    void *key;
    void *value;
    int index;
};


struct map_ex *map_init(key_compare com_cb, key_value_clear kvclear_cb);
int map_insert(struct map_ex *map, void *key, void *value);
int map_erase(struct map_ex *map, void *key);
void *map_find(struct map_ex *map, void *key);

struct map_curse *map_begin(struct map_ex *map);
struct map_curse *map_end(struct map_ex *map);
struct map_curse *map_next(struct map_ex *map, struct map_curse *);
struct map_curse *map_pre(struct map_ex *map, struct map_curse *);

void map_clear(struct map_ex *map);
void map_fini(struct map_ex *map);

#endif
