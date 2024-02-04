#ifndef KNN_DATASETIO_H
#define KNN_DATASETIO_H

#include <stddef.h>

#define NHOURS 24

typedef struct knn_dataset
{
    size_t ndays;
    float table[][NHOURS];
} knn_dataset;

/**
 * @brief Loads a dataset given a filename.
 *
 * @param[in]   filename    Input dataset filename.
 * @param[out]  dataset     Readed dataset.
 * @return On failure returns zero.
 */
int knn_load_dataset(char const *filename, knn_dataset *dataset);

/**
 * @brief Saves a dataset given a filename.
 *
 * @param[in]   filename    Output dataset filename.
 * @param[in]   dataset     Dataset to append.
 * @return On failure return zero.
 */
int knn_save_dataset(char const *filename, knn_dataset const *dataset);

#endif
