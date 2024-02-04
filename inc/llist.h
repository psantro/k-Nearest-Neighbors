#ifndef KNN_LLIST_H
#define KNN_LLIST_H

#include <stddef.h>

/**
 * @brief Linked list node of neighbor.
 */
typedef struct knn_knode
{
    size_t nk;
    float distance;
    struct knn_knode *next;
} knn_knode;

/**
 * @brief Linked list of neighbors.
 */
typedef struct knn_kllist
{
    knn_knode *first;
    size_t size;
} knn_kllist;

#endif
