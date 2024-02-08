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

void knn_kNN(int k, float const *target, float const *data, int size, knn_neighbor *neighbors)
{
    int i, j;
    float temp_distance;
    knn_neighbor temp_neighbor;

    for (i = 0; i < k; i++)
    {
        neighbors[i].index = i;
        neighbors[i].eval = calculate_distance(&data[i * NHOURS], target);
    }

    for (i = 0; i < k - 1; i++)
    {
        for (j = 0; j < k - i - 1; j++)
        {
            if (neighbors[j].eval > neighbors[j + 1].eval)
            {
                temp_neighbor = neighbors[j];
                neighbors[j] = neighbors[j + 1];
                neighbors[j + 1] = temp_neighbor;
            }
        }
    }

    for (i = k; i < size; i++)
    {
        temp_distance = calculate_distance(&data[i * NHOURS], target);

        if (temp_distance < neighbors[k - 1].eval)
        {
            neighbors[k - 1].index = i;
            neighbors[k - 1].eval = temp_distance;

            for (j = 0; j < k - 1; j++)
            {
                for (int l = 0; l < k - j - 1; l++)
                {
                    if (neighbors[l].eval > neighbors[l + 1].eval)
                    {
                        temp_neighbor = neighbors[l];
                        neighbors[l] = neighbors[l + 1];
                        neighbors[l + 1] = temp_neighbor;
                    }
                }
            }
        }
    }
}
