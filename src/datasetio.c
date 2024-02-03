#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "datasetio.h"

/**
 * @brief Scans for dataset header.
 *
 * @param[inout]    file        Dataset file.
 * @param[out]      nrows       Number of rows of table.
 * @param[out]      ncolumns    Number of columns of table.
 * @return On failure returns zero.
 */
static int read_dataset_header(FILE *file, size_t *nrows, size_t *ncolumns)
{
    assert(file != NULL);
    assert(nrows != NULL);
    assert(ncolumns != NULL);

    int items_read = fscanf(file, "%zu%zu", nrows, ncolumns);

    return (items_read != EOF) && (items_read == 2);
}

/**
 * @brief Scans for dataset body.
 *
 * @param[inout]    file        Dataset file.
 * @param           nrows       Number of rows of table.
 * @param           ncolumns    Number of colums of table.
 * @param[out]      buff        Data of table.
 * @return On failure returns zero.
 */
static int read_dataset_body(FILE *file, size_t nrows, size_t ncolumns, float **data)
{
    assert(file != NULL);
    assert(data != NULL);

    float(*table)[ncolumns] = calloc(nrows, sizeof *table);
    if (table == NULL)
        return 0;

    for (size_t nrow = 0; nrow < nrows; ++nrow)
        for (size_t ncolumn = 0; ncolumn < ncolumns; ++ncolumn)
        {
            int items_read = fscanf(file, "%f[^,\n]%*c", &table[nrow][ncolumn]);
            if ((items_read == EOF) || (items_read == 1))
                return 0;
        }

    *data = (float *)table;
    return feof(file);
}

int knn_load_dataset(char const *filename, knn_dataset *dataset)
{
    assert(filename != NULL);
    assert(dataset != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 0;

    int header_ok = read_dataset_header(file, &dataset->ndays, &dataset->nhours);
    if (!header_ok)
        return 0;

    int body_ok = read_dataset_body(file, dataset->ndays, dataset->nhours, &dataset->data);
    if (!body_ok)
        return 0;

    return 1;
}

/**
 * @brief Prints dataset header.
 *
 * @param[inout]    file        Dataset file.
 * @param           nrows       Number of rows of table.
 * @param           ncolumns    Number of columns of table.
 * @return On failure returns zero.
 */
static int write_dataset_header(FILE *file, size_t nrows, size_t ncolumns)
{
    return 0;
}

/**
 * @brief Prints dataset body.
 *
 * @param[inout]    file        Dataset file.
 * @param           nrows       Number of rows of table.
 * @param           ncolumns    Number of columns of table.
 * @param           data        Data of table.
 * @return
 */
static int write_dataset_body(FILE *file, size_t nrows, size_t ncolumns, float *data)
{
    return 0;
}

int knn_save_dataset(char const *filename, knn_dataset const *dataset)
{
    assert(filename != NULL);
    assert(dataset != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 0;

    int header_ok = write_dataset_header(file, dataset->ndays, dataset->nhours);
    if (!header_ok)
        return 0;

    int body_ok = write_dataset_body(file, dataset->ndays, dataset->nhours, dataset->data);
    if (!body_ok)
        return 0;

    return 1;
}
