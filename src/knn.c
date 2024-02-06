#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "knn.h"
#include "llist.h"

float calculate_distance(float *neighbor, float *base)
{
    float total_distance = 0.0;

    for (int hour = 0; hour < NHOURS; hour++)
    {
        total_distance += fabs(neighbor[hour] - base[hour]);
    }

    return total_distance;
}

knn_kllist find_k_nearest_neighbors(knn_dataset dataset, size_t base, size_t k)
{
    knn_kllist neighbors;
    neighbors->first = NULL;
    neighbors->size = 0;

    float *base_measurements = dataset->table[base];
    float *temp_measurements;
    float distance;

    for (size_t i = 0; i < dataset->ndays; i++)
    {
        if (i != base)
        {
            temp_measurements = dataset->table[i];
            distance = calculate_distance(temp_measurements, base_measurements);
            insert_sorted(&neighbors, distance, i);

            if (neighbors->size > k)
            {
                knn_knode *temp = neighbors->first;
                neighbors->first = neighbors->first->next;
                free(temp);
                neighbors->size--;
            }
        }
    }

    return neighbors;
}

static void generate_prediction(size_t k, knn_kllist neighbors, knn_dataset dataset, float prediction[NHOURS])
{
    float sum = 0;
    size_t nk, nhour;
    knn_knode node = neighbors->first;

    for (nhour = 0; nhour < NHOURS; nhour++)
    {
        sum = 0;
        while (node != NULL)
        {
            sum += dataset->table[node->nk][nhour] / k;
        }
        prediction[nhour] = sum;
    }
}

static void save_prediction(FILE *f, float prediction[NHOURS])
{
    size_t nhours;

    for (nhours = 0; nhours < NHOURS - 1; nhours++)
    {
        fprintf(f, "%f,", prediction[nhours]);
    }
    fprintf(f, "%f\n", prediction[nhours]);
}

void knn(size_t k, knn_dataset dataset)
{
    FILE *file;
    size_t ndays;

    file = fopen("Prediction.txt", "w");
    if (file == NULL)
    {
        perror("Unable to open file");
        abort();
    }
    for (ndays = 0; ndays < NPREDICTIONS; ndays++)
    {
        knn_kllist neighbors = find_k_nearest_neighbors(dataset, ndays, k);
        float prediction[NHOURS];
        generate_prediction(k, neighbors, prediction);
        save_predcition(file, prediction);
        // funcion de Marcos
    }
    fclose(file);
}
