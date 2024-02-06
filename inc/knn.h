#ifndef KNN_KNN_H
#define KNN_KNN_H

#include "datasetio.h"

/**
 * @brief Do a k-Nearest-Neighbors prediction.
 *
 * @param       k       Number of neighbors.
 * @param       n       Number of predictions.
 * @param[in]   dataset Base dataset.
 * @return Dataset n with predictions based on the given dataset and k neighbors.
 */
knn_dataset *knn_kNN(int k, int n, knn_dataset const *dataset);

#endif
