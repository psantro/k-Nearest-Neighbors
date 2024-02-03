#ifndef KNN_DATASETIO_H
#define KNN_DATASETIO_H

#include <stddef.h>

/**
 * @brief Temporal series dataset.
 */
typedef struct knn_dataset
{
    size_t ndays, nhours;
    float *data;
} knn_dataset;

/**
 * @brief Reads a knn_dataset from a dataset file given a filename.
 *
 * @param[in]   filename    Path to dataset file.
 * @param[out]  dataset     Readed dataset.
 * @return On failure returns zero.
 */
int knn_load_dataset(char const *filename, knn_dataset *dataset);

/**
 * @brief Appends a knn_dataset to a dataset file given a filename.
 *
 * @param[in]   filename    Path to dataset file.
 * @param[in]   dataset     Dataset to append.
 * @return On failure return zero.
 */
int knn_save_dataset(char const *filename, knn_dataset const *dataset);

#endif
