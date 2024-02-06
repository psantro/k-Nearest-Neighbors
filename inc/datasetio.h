#ifndef KNN_DATASETIO_H
#define KNN_DATASETIO_H

#include <stddef.h>

#define NHOURS 24
#define NPREDICTIONS 1000

typedef float (*dataset)[NHOURS];

/**
 * @brief Allocates a dataset given a number of days
 *
 * @param       ndays   Number of days.
 * @param[out]  data    Data buffer.
 * @return On failure returns zero.
 */
int knn_allocate_dataset(int ndays, float **data);

/**
 * @brief Loads a dataset given a filename.
 *
 * @param[in]   filename    Input dataset filename.
 * @param[out]  ndays       Number of days.
 * @param[out]  data        Dataset data.
 * @return On failure returns zero.
 */
int knn_load_dataset(char const *filename, int *ndays, float **data);

/**
 * @brief Saves a dataset given a filename.
 *
 * @param[in]   filename    Output dataset filename.
 * @param[in]   dataset     Dataset to append.
 * @return On failure return zero.
 */
// int knn_save_dataset(char const *filename, knn_dataset const *dataset);

#endif
