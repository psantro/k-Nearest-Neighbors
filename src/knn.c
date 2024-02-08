#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "knn.h"

static float calculate_distance(float const *neighbor, float const *target)
{
    float total_distance = 0.0;

    for (int hour = 0; hour < NHOURS; hour++)
        total_distance += fabs(neighbor[hour] - target[hour]);

    return total_distance;
}

static void initialize_array(int k, float const *target, float const *data, knn_neighbor *nk)
{
    for (int n = 0; n < k; ++n)
        nk[n] = (knn_neighbor){
            .index = n,
            .eval = calculate_distance(&data[n * NHOURS], target)};
}

static void swap_neighbor(knn_neighbor *a, knn_neighbor *b)
{
    knn_neighbor tmp;
    tmp = *a, *a = *b, *b = tmp;
}

static void bubble_sort_array(int k, knn_neighbor *nk)
{
    int nswaps;

    do
    {
        nswaps = 0;
        for (int n = 0; n < k - 1; ++n)
            if (nk[n].eval > nk[n + 1].eval)
                swap_neighbor(&nk[n], &nk[n + 1]), ++nswaps;
    } while (nswaps != 0);
}

void knn_kNN(int k, float const *target, float const *data, int size, knn_neighbor *nk)
{
    int i, j;
    float temp_distance;

    initialize_array(k, target, data, nk);
    bubble_sort_array(k, nk);

    for (i = k; i < size; i++)
    {
        temp_distance = calculate_distance(&data[i * NHOURS], target);

        if (temp_distance < nk[k - 1].eval)
        {
            nk[k - 1].index = i;
            nk[k - 1].eval = temp_distance;

            for (j = 0; j < k - 1; j++)
            {
                for (int l = 0; l < k - j - 1; l++)
                {
                    if (nk[l].eval > nk[l + 1].eval)
                    {
                        temp_neighbor = nk[l];
                        nk[l] = nk[l + 1];
                        nk[l + 1] = temp_neighbor;
                    }
                }
            }
        }
    }
}
