#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "datasetio.h"

/**
 * @brief Scans dataset header.
 *
 * @param[inout]    file        Input dataset file.
 * @param[out]      nrows       Number of table rows.
 * @param[out]      ncolumns    Number of table columns.
 * @return On failure returns zero.
 */
static int scan_dataset_header(FILE *file, size_t *nrows, size_t *ncolumns)
{
    assert(file != NULL);
    assert(nrows != NULL);
    assert(ncolumns != NULL);

    int items_read = fscanf(file, "%zu%zu", nrows, ncolumns);

    return (items_read != EOF) && (items_read == 2);
}

/**
 * @brief Scans dataset body.
 *
 * @param[inout]    file        Input dataset file.
 * @param           nrows       Number of table rows.
 * @param           ncolumns    Number of table colums.
 * @param[out]      buff        Table data.
 * @return On failure returns zero.
 */
static int scan_dataset_body(FILE *file, size_t nrows, size_t ncolumns, float **data)
{
    assert(file != NULL);
    assert(data != NULL);

    float(*table)[ncolumns] = calloc(nrows, sizeof *table);
    if (table == NULL)
        return 0;

    for (size_t nrow = 0; nrow < nrows; ++nrow)
    {
        for (size_t ncolumn = 0; ncolumn < (ncolumns - 1); ++ncolumn)
        {
            fscanf(file, "%f[^,]%*c", &table[nrow][ncolumn]);
        }
        fscanf(file, "%f[^\n]%*c", &table[nrow][ncolumns - 1]);
    }

    *data = (float *)table;
    return 0;
}

/**
 * @brief Prints dataset header.
 *
 * @param[inout]    file        Output dataset file.
 * @param           nrows       Number of table rows.
 * @param           ncolumns    Number of table columns.
 * @return On failure returns zero.
 */
static int print_dataset_header(FILE *file, size_t nrows, size_t ncolumns)
{
    assert(file != NULL);

    fprintf(file, "%zu %zu", nrows, ncolumns);

    return 0;
}

/**
 * @brief Prints dataset body.
 *
 * @param[inout]    file        Output dataset file.
 * @param           nrows       Number of table rows.
 * @param           ncolumns    Number of table columns.
 * @param           data        Table data.
 * @return On failure returns zero.
 */
static int print_dataset_body(FILE *file, size_t nrows, size_t ncolumns, float *data)
{
    assert(file != NULL);
    assert(data != NULL);

    float(*table)[ncolumns] = (float(*)[ncolumns])data;

    for (size_t nrow = 0; nrow < nrows; ++nrow)
    {
        for (size_t ncolumn = 0; ncolumn < (ncolumns - 1); ++ncolumns)
        {
            fprintf(file, "%.1f,", table[nrow, ncolumn]);
        }
        fprintf(file, "%.1f\n", table[nrow, ncolumns - 1]);
    }

    free(data);
    return 0;
}

int knn_load_dataset(char const *filename, knn_dataset *dataset)
{
    assert(filename != NULL);
    assert(dataset != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 0;

    int header_ok = scan_dataset_header(file, &dataset->ndays, &dataset->nhours);
    if (!header_ok)
        return 0;

    int body_ok = scan_dataset_body(file, dataset->ndays, dataset->nhours, &dataset->data);
    if (!body_ok)
        return 0;

    return 1;
}

int knn_save_dataset(char const *filename, knn_dataset const *dataset)
{
    assert(filename != NULL);
    assert(dataset != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 0;

    int header_ok = print_dataset_header(file, dataset->ndays, dataset->nhours);
    if (!header_ok)
        return 0;

    int body_ok = print_dataset_body(file, dataset->ndays, dataset->nhours, dataset->data);
    if (!body_ok)
        return 0;

    return 1;
}
