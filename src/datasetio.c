#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "datasetio.h"

/**
 * @brief Parses dataset header.
 *
 * @param[inout]    file        Dataset file.
 * @param[out]      nrows       Number of rows of the dataset.
 * @param[out]      ncolumns    Number of columns of the dataset.
 * @return On success returns zero.
 */
static int read_dataset_header(FILE *file, size_t *nrows, size_t *ncolumns)
{
    assert(file != NULL);
    assert(nrows != NULL);
    assert(ncolumns != NULL);

    int input_items = fscanf(file, "%zu%zu", nrows, ncolumns);

    return (input_items != EOF) && (input_items == 2);
}

/**
 * @brief Parses dataset body.
 *
 * @param[inout]    file        Dataset file.
 * @param           nrows       Number of rows of the dataset.
 * @param           ncolumns    Number of colums of the dataset.
 * @param[out]      buff        Dataset buffer.
 * @return On success returns zero.
 */
static int read_dataset_body(FILE *file, size_t nrows, size_t ncolumns, float **data_buffer)
{
    assert(file != NULL);
    assert(data_buffer != NULL);

    float(*table)[ncolumns] = calloc(nrows, sizeof *table);
    if (table == NULL)
        return 1;

    for (size_t nrow = 0; nrow < nrows; ++nrow)
        for (size_t ncolumn = 0; ncolumn < ncolumns; ++ncolumn)
        {
            int input_items = fscanf(file, "%f", &table[nrow][ncolumn]);
            if ((input_items == EOF) || (input_items == 1))
                return 1;
        }

    *data_buffer = (float *)table;
    return 0;
}

int knn_load_dataset(char const *pathname, knn_dataset *dataset)
{
    assert(pathname != NULL);
    assert(dataset != NULL);

    FILE *file = fopen(pathname, "r");
    if (file == NULL)
        return 1;

    int header_ok = read_dataset_header(file, &dataset->ndays, &dataset->nhours);
    if (!header_ok)
        return 1;

    int body_ok = read_dataset_body(file, dataset->ndays, dataset->nhours, &dataset->data_buff);
    if (!body_ok)
        return 1;

    return 0;
}

int knn_save_dataset(char const *pathname, knn_dataset const *dataset)
{
    return 0;
}
