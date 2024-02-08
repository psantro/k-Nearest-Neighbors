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

void knn_bubble_sort_array(int k, knn_neighbor *nk, int asc)
{
    int nswaps;

    do
    {
        nswaps = 0;
        for (int n = 0; n < k - 1; ++n)
            if (!(nk[n].eval <= nk[n + 1].eval) && asc)
                swap_neighbor(&nk[n], &nk[n + 1]), ++nswaps;
    } while (nswaps != 0);
}

static void sink_first(int k, knn_neighbor *nk)
{
    int n = 0;
    while (nk[n].eval < nk[n + 1].eval && n < k - 1)
        swap_neighbor(&nk[n], &nk[n + 1]), ++n;
}

static void find_k(int k, float const *target, float const *data, int size, knn_neighbor *kn)
{
    float dst;
    for (int n = k; n < size; ++n)
    {
        dst = calculate_distance(&data[n * NHOURS], target);

        if (dst < kn[0].eval)
        {
            kn[0] = (knn_neighbor){
                .eval = dst,
                .index = n};
            sink_first(k, kn);
        }
    }
}

static float calculate_mape(int ndays, float *data, float *prediction, int data_day, int prediction_day)
{
    float mape = 0, dif, error;

    for (int hour = 0; hour < NHOURS; hour++)
    {
        dif = fabs(data[(data_day * NHOURS) + hour] - prediction[(data_day * NHOURS) + hour]);
        error = (100 / NHOURS) * fabs(dif / data[(data_day * NHOURS) + hour]);
        mape += error;
    }

    return mape;
}

void knn_kNN(int k, float const *target, float const *data, int size, knn_neighbor *nk)
{
    assert(k > 0);
    assert(target != NULL);
    assert(data != NULL);
    assert(size > 0);
    assert(nk != NULL);

    initialize_array(k, target, data, nk);
    knn_bubble_sort_array(k, nk, 0);
    find_k(k, target, data, size, nk);
}

static void compute_prediction_and_mape(int k, int ndays, float const *data, int n, knn_neighbor const *neighbors, float *prediction, float *mape)
{
    *mape = 0.0;
    for (int nhour = 0; nhour < NHOURS; ++nhour)
    {
        prediction[nhour] = 0.0;
        for (int neighbor = 0; neighbor < k; ++neighbor)
            prediction[nhour] += data[nhour + NHOURS * neighbors[neighbor].index] / k;

        *mape += (100.0 / NHOURS) * fabs(data[nhour + (ndays - NPREDICTIONS + n) * NHOURS] - prediction[nhour]) / data[nhour + (ndays - NPREDICTIONS + n) * NHOURS];
    }
}

void knn_predictions(int k, int ndays, knn_neighbor const *neighbors, float const *data, float *predictions, float *mape)
{
#pragma omp parallel for
    for (int prediction = 0; prediction < NPREDICTIONS; ++prediction)
        compute_prediction_and_mape(k, ndays, data, prediction, &neighbors[prediction * k], &predictions[prediction * NHOURS], &mape[prediction]);
}
