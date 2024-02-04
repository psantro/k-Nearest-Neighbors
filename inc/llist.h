#ifndef KNN_LLIST_H
#define KNN_LLIST_H

#include <stddef.h>

/**
 * @brief Linked list
 */
typedef struct knn_node
{
    size_t nk;
    knn_node next;
} *knn_node;

#endif
