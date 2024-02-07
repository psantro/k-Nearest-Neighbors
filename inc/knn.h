#ifndef KNN_KNN_H
#define KNN_KNN_H

#include <stddef.h>
#include "datasetio.h"

typedef struct knn_neighbor_index_eval_pair
{
    float eval;
    int index;
} knn_neighbor;

/**
 * @brief Find k-Nearest Neighbors.
 *
 * @param       k           Nearest Neighbors.
 * @param[in]   target      Find close neighbors to.
 * @param[in]   data        Matrix of neighbors of size @p size by @c NHOURS .
 * @param       size        Data row count.
 * @param[out]  neighbors   Array of k-Nearest Neighbors to target indexes.
 */
void knn_kNN(int k, float const target[NHOURS], float const data[][NHOURS], int size, knn_neighbor neighbors[]);

#endif
