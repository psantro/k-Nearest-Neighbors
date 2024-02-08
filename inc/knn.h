#ifndef KNN_KNN_H
#define KNN_KNN_H

#include <stddef.h>
#include "datasetio.h"

/**
 * @brief Chunk index and distance (eval) pair.
 */
typedef struct knn_neighbor_index_eval_pair
{
    float eval;
    int index;
} knn_neighbor;

/**
 * @brief Find k-Nearest Neighbors.
 *
 * @param       k       Nearest Neighbors.
 * @param[in]   target  Find close neighbors to.
 * @param[in]   data    Matrix of neighbors of size @p size by @c NHOURS .
 * @param       size    Data row count.
 * @param[out]  kn      Array of k-Nearest Neighbors to target indexes.
 */
void knn_kNN(int k, float const *target, float const *data, int size, knn_neighbor *kn);

/**
 * @brief Bubble sort knn array.
 *
 * @param           k   Nearest Neighbors.
 * @param[inout]    nk  Array of k-Nearest Neighbors to target indexes.
 * @param           asc Ascending.
 */
void knn_bubble_sort_array(int k, knn_neighbor *nk, int asc);

void knn_predictions(int k, int ndays, knn_neighbor const *neighbors, float const *data, float *predictions, float *mape);

#endif
