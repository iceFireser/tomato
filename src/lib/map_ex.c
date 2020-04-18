#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <scheme.h>

#include "map_ex.h"

struct avl_tree_node
{
    struct avl_tree_node *parent;
    struct avl_tree_node *left;
    struct avl_tree_node *right;

    int max_deep;

    void *key;
    void *value;
};


struct avl_tree
{
    struct avl_tree_node root;
    int num;

    key_compare compare_pf;
    key_value_clear kvclear_pf;
};

struct map_ex
{
    struct avl_tree *tree;
    struct avl_tree_node **node_arr;
    int node_arr_num;
    struct map_curse **curse_arr;
    int curse_arr_num;
};

#define TREE_TAIL (void *)(~0ul)

static struct avl_tree_node *tree_find_node(struct avl_tree *tree, void *key);


static int default_key_compare(void *key1, void *key2)
{
    long k1 = (long)key1;
    long k2 = (long)key2;

    return k1 - k2;
}

static int default_key_value_clear(void *key1, void *value)
{
    (void)key1;

    (void)value;

    return 0;
}


static struct avl_tree *tree_init()
{
    struct avl_tree *tree = malloc(sizeof(struct avl_tree));
    if (!tree)
        return NULL;

    bzero(tree, sizeof(*tree));

    tree->compare_pf = default_key_compare;
    tree->kvclear_pf = default_key_value_clear;

    return tree;
}

static void tree_clear(struct avl_tree *tree)
{
    struct avl_tree_node *node = NULL;
    unsigned long i, j;

    unsigned long node_max = (unsigned long)(tree->num + 1);
    struct avl_tree_node **node_queue = malloc(sizeof(struct avl_tree_node *) * node_max);

    if (!node_queue)
        goto err;

    memset(node_queue, 0xFF, sizeof(struct avl_tree_node *) * node_max);
    node_queue[0] = tree->root.left;


    for (i = 0, j = 1, node = node_queue[0];
         (i < node_max) && (j < node_max) && (node != TREE_TAIL) && (node) ;
         node = node_queue[++i])
    {
        if (node)
        {

            if (node->left)
                node_queue[j++] = node->left;

            if (node->right)
                node_queue[j++] = node->right;

            bzero(node, sizeof(*node));
            free(node);
        }

    }

    tree->root.left = NULL;
    tree->num = 0;

    if (node_queue)
        free(node_queue);

    return;

err:
    return;
}


static void tree_fini(struct avl_tree *tree)
{
    tree_clear(tree);
    bzero(tree, sizeof(*tree));
    free(tree);
    return;
}



static void node_init(struct avl_tree_node *node, void *key, void *value)
{
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;


    node->max_deep = 1;

    node->key = key;
    node->value = value;

    return;
}

static int num_2_pow(int x)
{
    int i;
    int num = 0;
    if (!x)
    {
        num = 1;
    }
    else if (1 == x)
    {
        num = 2;
    }
    else
    {
        for (num = 2, i = 0; i < x; i++)
        {
            num *= num;
        }
    }

    return num;
};

int tree_full_node_num(int hight)
{
    int num = 0;
    int i;
    for (i = 0; i < hight; i++)
    {
        num += num_2_pow(i);
    }

    return num;
}

int max_deep(struct avl_tree_node *node)
{
    if (!node)
        return 0;

    return node->max_deep;
}

int tree_spin(struct avl_tree *tree, struct avl_tree_node *spin_node)
{
    /* insert */
    struct avl_tree_node *node = spin_node;
    struct avl_tree_node *tmp_node;
    struct avl_tree_node *tmp2_node;

    for (;node && node->parent; node=node->parent)
    {
        if (max_deep(node->left) - max_deep(node->right) > 1)
        {

            if (max_deep(node->left->left) > max_deep(node->left->right))
            {
                /* LL */
                tmp_node = node->left;

                node->left = tmp_node->right;
                if (node->left)
                    node->left->parent = node;

                tmp_node->parent = node->parent;
                if (node == tmp_node->parent->left)
                {
                    tmp_node->parent->left = tmp_node;
                }
                else
                {
                    tmp_node->parent->right = tmp_node;
                }


                tmp_node->right = node;
                if (tmp_node->right)
                    tmp_node->right->parent = tmp_node;

                node = tmp_node;

            }
            else
            {

                /* LR */
                tmp_node = node->left;
                tmp2_node = node->left->right;


                tmp_node->right = tmp2_node->left;
                if (tmp_node->right)
                    tmp_node->right->parent = tmp_node;

                node->left = tmp2_node->right;
                if (node->left)
                    node->left->parent = node;

                tmp2_node->parent = node->parent;
                if (node == tmp2_node->parent->left)
                {
                    tmp2_node->parent->left = tmp2_node;
                }
                else
                {
                    tmp2_node->parent->right = tmp2_node;
                }

                tmp2_node->left = tmp_node;
                if (tmp2_node->left)
                    tmp2_node->left->parent = tmp2_node;

                tmp2_node->right = node;
                if (tmp2_node->right)
                    tmp2_node->right->parent = tmp2_node;

                node = tmp2_node;
            }

        }
        else if (max_deep(node->right) - max_deep(node->left) > 1)
        {
            if (max_deep(node->right->right) > max_deep(node->right->left))
            {
                /* RR */
                tmp_node = node->right;

                node->right = tmp_node->left;
                if (node->right)
                    node->right->parent = node->right;

                tmp_node->parent = node->parent;
                if (node == tmp_node->parent->left)
                {
                    tmp_node->parent->left = tmp_node;
                }
                else
                {
                    tmp_node->parent->right = tmp_node;
                }

                tmp_node->left = node;
                if (tmp_node->left)
                    tmp_node->left->parent = tmp_node;

                node = tmp_node;
            }
            else
            {
                /* RL */
                tmp_node = node->right;
                tmp2_node = node->right->left;


                tmp_node->left = tmp2_node->right;
                if (tmp_node->left)
                    tmp_node->left->parent = tmp_node;

                node->right = tmp2_node->left;
                if (node->right)
                    node->right->parent = node;

                tmp2_node->parent = node->parent;
                if (node == tmp2_node->parent->left)
                {
                    tmp2_node->parent->left = tmp2_node;
                }
                else
                {
                    tmp2_node->parent->right = tmp2_node;
                }

                tmp2_node->left = node;
                if (tmp2_node->left)
                    tmp2_node->left->parent = tmp2_node;

                tmp2_node->right = tmp_node;
                if (tmp2_node->right)
                    tmp2_node->right->parent = tmp2_node;

                node = tmp2_node;
            }

        }
        else
        {
            /* equal */
        }

        if (node->left)
        {
            tmp_node = node->left;
            tmp_node->max_deep = MAX(max_deep(tmp_node->left), max_deep(tmp_node->right)) + 1;
        }

        if (node->right)
        {
            tmp_node = node->right;
            tmp_node->max_deep = MAX(max_deep(tmp_node->left), max_deep(tmp_node->right)) + 1;
        }
        node->max_deep = MAX(max_deep(node->left), max_deep(node->right)) + 1;
   }

    return 0;
}

int tree_insert(struct avl_tree *tree, void *key, void *value)
{
    int ret = -1;
    int ret_compare;
    struct avl_tree_node *it_node;
    struct avl_tree_node **it_next_node;

    struct avl_tree_node *node = malloc(sizeof(struct avl_tree_node));
    if (!node)
    {
        goto err;
    }

    node_init(node, key, value);

    if (!tree->root.left)
    {
        tree->root.left = node;
        tree->root.right = NULL;
        tree->root.parent = NULL;
        tree->num = 1;

        node->parent = &tree->root;
    }
    else
    {
        for (it_next_node = NULL, it_node = tree->root.left; it_node; )
        {
            ret_compare = tree->compare_pf(key, it_node->key);

            if (ret_compare < 0) /* left */
            {
                it_next_node = &it_node->left;
            }
            else if (ret_compare > 0)
            {
                it_next_node = &it_node->right;
            }
            else
            {
                ret = -2;
                goto err;
            }

            if (!(*it_next_node) )
            {
                node->parent = it_node;
                *it_next_node = node;
                tree->num++;

                break;
            }
            else
            {
                it_node = *it_next_node;
            }

        }

        /* tree's spin */
        tree_spin(tree, node->parent);

    }



    return 0;
err:
    if (node)
    {
        free(node);
        node = NULL;
    }
    return ret;
}

struct avl_tree_node *tree_find_right_child_dep_left(struct avl_tree_node *node)
{
    struct avl_tree_node *it_node;

    for (it_node = node->right; it_node; it_node = it_node->left)
    {
        if (!it_node->left)
        {
            break;
        }

    }

    return it_node;
}

static void tree_replace_node(struct avl_tree_node *substitute_node, struct avl_tree_node *node)
{

    substitute_node->left = node->left;
    substitute_node->right = node->right;
    substitute_node->parent = node->parent;


    if (substitute_node->left)
        substitute_node->left->parent = substitute_node;

    if (substitute_node->right)
        substitute_node->right->parent = substitute_node;

    if (node == substitute_node->parent->left)
    {
        substitute_node->parent->left = substitute_node;
    }
    else
    {
        substitute_node->parent->right = substitute_node;
    }

    return;
}



int tree_erase(struct avl_tree *tree, void *key)
{
    struct avl_tree_node *node;
    struct avl_tree_node *node_parent;
    struct avl_tree_node *tmp_node;
    struct avl_tree_node *substitute_node;
    node = tree_find_node(tree, key);

    if (node)
    {
        if (!node->left && !node->right)
        {
            /* rease node self only */
            node_parent = node->parent;

            if (node == node_parent->left)
            {
                node_parent->left = NULL;
            }
            else
            {
                node_parent->right = NULL;
            }

        }
        else if (node->left && !node->right)
        {
            node_parent = node->parent;
            if (node == node_parent->left)
            {
                node_parent->left = node->left;
            }
            else
            {
                node_parent->right = node->left;
            }

            node->left->parent = node_parent;

        }
        else if (!node->left && node->right)
        {
            node_parent = node->parent;
            if (node == node_parent->left)
            {
                node_parent->left = node->right;
            }
            else
            {
                node_parent->right = node->right;
            }

            node->right->parent = node_parent;
        }
        else
        {
            /* tree_find_right_child_dep_left */

            substitute_node = tree_find_right_child_dep_left(node);

            node_parent = substitute_node->parent;

            if (substitute_node == node_parent->left)
            {


                if (substitute_node->right)
                {
                    node_parent->left = substitute_node;
                    node_parent->left->parent = node_parent;
                    tmp_node = substitute_node->right;
                    tree_replace_node(substitute_node, node);

                    node_parent = tmp_node->parent;
                }
                else
                {
                    node_parent->left = NULL;

                    tree_replace_node(substitute_node, node);

                    /* node_parent */
                }
            }
            else
            {
                if (substitute_node->right)
                {
                    node_parent->right = substitute_node->right;
                    node_parent->right->parent = node_parent;
                    tmp_node = substitute_node->right;

                    tree_replace_node(substitute_node, node);
                    node_parent = tmp_node->parent;

                }
                else
                {
                    node_parent->right = NULL;
                    tree_replace_node(substitute_node, node);

                    node_parent = substitute_node;
                    /* node_parent */
                }

            }

        }

        tree->num--;

        tree_spin(tree, node_parent);

        tree->kvclear_pf(node->key, node->value);
        bzero(&node, sizeof(node));
        free(node);
        node = NULL;

    }


    return 0;
}

void tree_print(struct avl_tree *tree)
{
    struct avl_tree_node *node = NULL;
    int i, j;

    struct avl_tree_node *node_queue[1024];
    memset(node_queue, 0xFF, sizeof(node_queue));
    node_queue[0] = tree->root.left;


    for (i = 0, j = 1, node = node_queue[0]; node != TREE_TAIL; i++, node = node_queue[i])
    {
        if (node)
        {
            printf("%ld  ", (long)node->key);

            node_queue[j++] = node->left;
            node_queue[j++] = node->right;
        }
        else
        {
            printf("null  ");
        }


    }

    return;
}


void tree_travel(struct avl_tree *tree, struct avl_tree_node **node_queue,
                     unsigned long node_max)
{
    struct avl_tree_node *node = NULL;
    unsigned long i, j;


    memset(node_queue, 0xFF, sizeof(struct avl_tree_node *) * node_max);
    node_queue[0] = tree->root.left;


    for (i = 0, j = 1, node = node_queue[0];
         (i < node_max) && (j < node_max) && (node != TREE_TAIL) && (node) ;
         node = node_queue[++i])
    {
        if (node)
        {

            if (node->left)
                node_queue[j++] = node->left;

            if (node->right)
                node_queue[j++] = node->right;
        }

    }

    return;
}


static struct avl_tree_node *tree_find_node(struct avl_tree *tree, void *key)
{
    struct avl_tree_node *node = NULL;
    int ret_compare;
    struct avl_tree_node *it_node;
    struct avl_tree_node **it_next_node;

    for (it_next_node = NULL, it_node = tree->root.left; it_node; )
    {
        ret_compare = tree->compare_pf(key, it_node->key);

        if (ret_compare < 0) /* left */
        {
            it_next_node = &it_node->left;
        }
        else if (ret_compare > 0)
        {
            it_next_node = &it_node->right;
        }
        else
        {
            node = it_node;
            break;
        }

        if (!(*it_next_node) )
        {
            break;
        }
        else
        {
            it_node = *it_next_node;
        }

    }

    return node;


}

void *tree_find(struct avl_tree *tree, void *key)
{
    void *value = NULL;
    struct avl_tree_node *node = NULL;

    node = tree_find_node(tree, key);
    if (node)
    {
        value = node->value;
    }

    return value;

}



void map_recycle(struct map_ex *map)
{
    if (map->tree)
    {
        tree_fini(map->tree);
        map->tree = NULL;
    }

    bzero(map, sizeof(*map));
    return;
};

struct map_ex *map_init(key_compare com_cb, key_value_clear kvclear_cb)
{
    struct map_ex *map = NULL;

    struct avl_tree *tree = tree_init();
    if (!tree)
    {
        goto err;
    }

    map = malloc(sizeof(struct map_ex));
    if (!map)
    {
        goto err;
    }

    map->tree = tree;


    return map;

err:
    map_recycle(map);
    if (map)
    {
        free(map);
        map = NULL;
    }

    return map;
}
int map_insert(struct map_ex *map, void *key, void *value)
{
    return tree_insert(map->tree, key, value);

}
int map_erase(struct map_ex *map, void *key)
{
    return tree_erase(map->tree, key);
}
void *map_find(struct map_ex *map, void *key)
{
    return tree_find(map->tree, key);

}

struct map_curse *map_begin(struct map_ex *map)
{
    int i;
    struct avl_tree_node **node_arr = malloc(sizeof(struct avl_tree_node *) * (map->tree->num + 1) );
    struct map_curse **curse_arr = malloc(sizeof(struct map_curse *) * (map->tree->num + 1) );

    if ((!node_arr) || (!curse_arr) )
        goto err;

    bzero(node_arr, sizeof(struct avl_tree_node *) * (map->tree->num + 1));
    bzero(curse_arr, sizeof(struct map_curse *) * (map->tree->num + 1));

    tree_travel(map->tree, node_arr, map->tree->num + 1);


    if (map->node_arr)
    {
        free(map->node_arr);
        map->node_arr = node_arr;
        map->node_arr_num = map->tree->num;
    }

    if (map->curse_arr)
    {
        free(map->curse_arr);
        map->curse_arr = curse_arr;
        map->curse_arr_num = map->tree->num;
    }

    for (i = 0; i < map->tree->num; i++)
    {
        curse_arr[i]->key = node_arr[i]->key;
        curse_arr[i]->value = node_arr[i]->value;
        curse_arr[i]->index = i;
    }

    return curse_arr[0];

err:
    if (node_arr)
        free(node_arr);

    if (curse_arr)
        free(curse_arr);

    return NULL;
}
struct map_curse *map_end(struct map_ex *map)
{

    if (map->curse_arr)
    {
        return map->curse_arr[map->curse_arr_num];
    }

    return NULL;
}
struct map_curse *map_next(struct map_ex *map, struct map_curse *curse)
{
    if (map && curse)
    {
        if ( (map->curse_arr) && (curse->index + 1) <= map->curse_arr_num )
        {
            return map->curse_arr[curse->index + 1];
        }
    }


    return NULL;

}
struct map_curse *map_pre(struct map_ex *map, struct map_curse *curse)
{
    if (map && curse)
    {
        if ( (map->curse_arr) && (curse->index - 1) >= 0 )
        {
            return map->curse_arr[curse->index - 1];
        }
    }

    return NULL;

}

void map_clear(struct map_ex *map)
{

    tree_clear(map->tree);

    if (map->node_arr)
    {
        free(map->node_arr);
    }

    if (map->curse_arr)
    {
        free(map->curse_arr);
    }

    return;
}

void map_print(struct map_ex *map)
{
    tree_print(map->tree);

    return;
}

void map_fini(struct map_ex *map)
{
    if (!map)
        return;

    tree_fini(map->tree);

    if (map->node_arr)
    {
        free(map->node_arr);
    }

    if (map->curse_arr)
    {
        free(map->curse_arr);
    }

    bzero(map, sizeof(*map));
    free(map);


    return;
}

#if 0
int main()
{
    struct map_ex *map = map_init(0, 0);
    int i;
    long key_arr[] = {1,2,4,3,5};
    long key;

    for (i = 0; i < (int)(sizeof(key_arr)/sizeof(key_arr[0])); i++)
    {
        key = key_arr[i];
        map_insert(map, (void *)key, NULL);
    }

    key=1;
    map_erase(map, (void *)key);

    map_print(map);

    return 0;
};

#endif
