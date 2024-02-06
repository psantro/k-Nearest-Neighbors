#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "datasetio.h"

/**
 * @brief Scans dataset header.
 *
 * @param[inout]    file        Dataset file.
 * @param[out]      ndays       Number of days.
 * @return On failure returns zero.
 */
static int scan_header(FILE *file, int *ndays)
{
    int nhours, items_read;

    items_read = fscanf(file, "%d%d", ndays, &nhours);

    return (*ndays > NPREDICTIONS) && (nhours == NHOURS);
}

/**
 * @brief Scans dataset body.
 *
 * @param[inout]    file        Dataset file.
 * @param           ndays       Number of days.
 * @param[out]      data        Dataset data.
 * @return On failure returns zero.
 */
static int scan_body(FILE *file, int ndays, float **data)
{
    int nday, nhour, alloc_ok, items_read;
    char buff[256];
    dataset table;

    alloc_ok = knn_allocate_dataset(ndays, data);
    if (!alloc_ok)
        return 0;

    table = (dataset)*data;
    for (nday = 0; nday < ndays; ++nday)
    {
        for (nhour = 0; nhour < (NHOURS - 1); ++nhour)
        {
            items_read = fscanf(file, "%256[^,]%*c", buff);
            table[nday][nhour] = strtof(buff, NULL);
        }
        items_read = fscanf(file, "%256[^\n]%*c", buff);
        table[nday][NHOURS - 1] = strtof(buff, NULL);
    }

    return 1;
}

int knn_allocate_dataset(int ndays, float **data)
{
    assert(ndays > NPREDICTIONS);
    assert(data != NULL);

    *data = malloc(ndays * NHOURS * sizeof **data);
    if (*data == NULL)
    {
        fprintf(stderr, "Error: Could not allocate dataset.\n");
        return 0;
    }

    return 1;
}

int knn_load_dataset(char const *filename, int *ndays, float **data)
{
    FILE *file;
    int header_ok, body_ok;

    assert(filename != NULL);
    assert(ndays != NULL);
    assert(data != NULL);

    file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not open file \"%s\".\n", filename);
        return 0;
    }

    header_ok = scan_header(file, ndays);
    if (!header_ok)
    {
        fprintf(stderr, "Error: Corrupted header in file \"%s\".\n", filename);
        return 0;
    }

    body_ok = scan_body(file, *ndays, data);
    if (!body_ok)
    {
        fprintf(stderr, "Error: Corrupted body in file \"%s\".\n", filename);
        return 0;
    }

    fclose(file);
    return 1;
}
