#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "knn.h"

/**
 * @brief Calculate distance between two days
 *
 * @param   neighbor    Day to comparate.
 * @param   target      Target day.
 *
 * @return Distance.
 */
static float calculate_distance(float const *neighbor, float const *target)
{
    float total_distance = 0.0;

    for (int hour = 0; hour < NHOURS; hour++)
        total_distance += fabs(neighbor[hour] - target[hour]);

    return total_distance;
}

static void initialize_array(int k, float const *target, float const *data, knn_neighbor *nk)
{
    int n;

    for (n = 0; n < k; ++n)
        nk[n] = (knn_neighbor){
            .index = n,
            .eval = calculate_distance(&data[n * NHOURS], target)};
}

void knn_kNN(int k, float const *target, float const *data, int size, knn_neighbor *nk)
{
    int i, j;
    float temp_distance;
    knn_neighbor temp_neighbor;

    initialize_array(k, target, data, nk);

    for (i = 0; i < k - 1; i++)
    {
        for (j = 0; j < k - i - 1; j++)
        {
            if (nk[j].eval > nk[j + 1].eval)
            {
                temp_neighbor = nk[j];
                nk[j] = nk[j + 1];
                nk[j + 1] = temp_neighbor;
            }
        }
    }

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
