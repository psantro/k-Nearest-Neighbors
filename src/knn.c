#include <assert.h>
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
            .eval = calculate_distance(&data[n * NHOURS], target),
            .index = n};
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
            if (nk[n].eval < nk[n + 1].eval)
                swap_neighbor(&nk[n], &nk[n + 1]), ++nswaps;
    } while (nswaps != 0);
}

static void sink_first(int k, knn_neighbor *nk)
{
    int n = 0;
    while (nk[n].eval < nk[n + 1].eval && n < k)
        swap_neighbor(&nk[n], &nk[n + 1]), ++n;
}

static void find_k(int k, float const *target, float const *data, int size, knn_neighbor *nk)
{
    float dst;
    for (int n = k; n < size; ++n)
    {
        dst = calculate_distance(&data[n * NHOURS], target);

        if (dst < nk[0].eval)
        {
            nk[0] = (knn_neighbor){
                .eval = dst,
                .index = n};
            sink_first(k, nk);
        }
    }
}

void knn_kNN(int k, float const *target, float const *data, int size, knn_neighbor *nk)
{
    assert(k > 0);
    assert(target != NULL);
    assert(data != NULL);
    assert(size > 0);
    assert(nk != NULL);

    initialize_array(k, target, data, nk);
    bubble_sort_array(k, nk);
    find_k(k, target, data, size, nk);
}

void knn_prediction(int k, knn_neighbor const *neighbors, float const *data, float *predictions)
{
    // Iterators
    int prediction, neighbor, hour;
    float sum;

    for (prediction = 0; prediction < NPREDICTIONS; prediction++)
    {
        for (hour = 0; hour < NHOURS; hour++)
        {
            sum = 0;
            for (neighbor = 0; neighbor < k; neighbor++)
            {
                sum += data[(neighbors[(prediction * k) + neighbor].index * NHOURS) + hour];
            }
            predictions[(prediction * NHOURS) + hour];
        }
    }
}

void knn_save_prediction(float *predictions)
{
    FILE *f = fopen("../out/predictions.txt", "w");
    int hour;


}
