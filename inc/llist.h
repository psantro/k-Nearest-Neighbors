#ifndef KNN_LLIST_H
#define KNN_LLIST_H

#include <stddef.h>

/**
 * @brief Linked list node of neighbor.
 */
typedef struct knn_knode
{
    size_t nk;
    knn_knode next;
} *knn_knode;

/**
 * @brief Linked list of neighbors.
 */
typedef knn_knode knn_kllist;

#endif
