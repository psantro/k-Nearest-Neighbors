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
static float calculate_distance(float neighbor[NHOURS], float target[NHOURS])
{
    float total_distance = 0.0;

    for (int hour = 0; hour < NHOURS; hour++)
    {
        total_distance += fabs(neighbor[hour] - target[hour]);
    }

    return total_distance;
}

void knn_kNN(int k, float const target[NHOURS], float const data[][NHOURS], int size, knn_neighbor neighbors[])
{
    int i, j;
    float temp_distance;

    neighbors[0].index = 0;
    neighbors[0].eval = calculate_distance(data[0], target);

    for (i = 1; i < k; i++)
    {
        if ((temp_distance = calculate_distance(data[i], target)) < neighbors[0].eval)
        {
            j = 1;
            while(temp_distance < neighbors[j].eval) {
                j++;
            }


        }
        neighbors[i].index = i;
        neighbors[i].eval = calculate_distance(data[i], target);
    }
}