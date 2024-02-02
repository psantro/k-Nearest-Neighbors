#ifndef KNN_DATASETIO_H
#define KNN_DATASETIO_H

#include <stddef.h>

/**
 * @brief Temporal series dataset.
 */
typedef struct knn_dataset
{
    size_t ndays, nhours;
    float *data_buff;
} knn_dataset;

/**
 * @brief Reads a knn_dataset from a dataset file given a pathname.
 *
 * @param[in]   pathname    Path to dataset file.
 * @param[out]  dataset     Readed dataset.
 * @return On success returns zero.
 */
int knn_load_dataset(char const *pathname, knn_dataset *dataset);

/**
 * @brief Appends a knn_dataset to a dataset file given a pathname.
 *
 * @param[in]   pathname    Path to dataset file.
 * @param[in]   dataset     Dataset to append.
 * @return On success return zero.
 */
int knn_save_dataset(char const *pathname, knn_dataset const *dataset);

#endif
