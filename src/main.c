#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "datasetio.h"

/**
 * @brief Sorted (easy to access) k-NN arguments.
 */
struct knn_args
{
    char const *filename;
    int k, np, nt;
};

/**
 * @brief Parses arguments.
 *
 * @param       argc Argument count.
 * @param[in]   argv Argument vector.
 * @param[out]  args Arguments.
 * @return On failure returns zero. on success returns the number of args parsed.
 */
static int init(int argc, char **argv, struct knn_args *args)
{
    if (argc != 4)
    {
        fprintf(stderr, "Error: Insufficent arguments.\n");
        return 0;
    }

    args->filename = argv[2];
    args->k = strtol(argv[1], NULL, 10);
    args->nt = strtol(argv[3], NULL, 10);

    return 3;
}

/**
 * @brief Load dataset method forwader for @p knn_load_dataset .
 *
 * @param       pid         Process id.
 * @param[in]   filename    Filename forwader.
 * @param[out]  ndays       Ndays forwader.
 * @param[out]  data        Data forwader.
 * @return On failure returns zero.
 */
static int load_dataset(int pid, char const *filename, int *ndays, float **data)
{
    if (pid == 0)
        if (!knn_load_dataset(filename, ndays, data))
            return 0;
        else
            data = NULL;
}

/**
 * @brief Calculates chunk size for master and slaves based on total and np;
 *
 * @param       total               Total of data to analize.
 * @param       np                  Number of processes.
 * @param[out]  master_chunk_size   Master chunk size.
 * @param[out]  slaves_chunk_size   Slaves chunk size.
 */
static void calculate_chunk_size(int total, int np, int *master_size, int *slaves_size)
{
    *master_size = total / np + total % np;
    *slaves_size = total / np;
}

/**
 * @brief Executes program.
 *
 * @param[in]   filename    Dataset filename.
 * @param       k           Number of neighbors.
 * @param       np          Number of processes.
 * @param       nt          Number of threads.
 * @param       pid         Process id.
 * @return On failure returns zero.
 */
static int exec(char const *filename, int k, int np, int nt, int pid)
{
    int npid;
    float *data, *chunk_data, target[NHOURS];
    int *chunk_counts, *chunk_displs;
    int nday, ndays, ok;
    int master_chunk_size, slaves_chunk_size, chunk_size;

    printf("Loading dataset...");
    ok = load_dataset(pid, filename, &ndays, &data);
    if (!ok)
    {
        fprintf(stderr, "%d: Error: Dataset loading error.\n", pid);
        return 0;
    }
    printf("ok\n");

    MPI_Bcast(&ndays, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    calculate_chunk_size(ndays - NPREDICTIONS, np, &master_chunk_size, &slaves_chunk_size);

    // Step 1.4. Master initializes chunk data and chunk counts. Slaves initializes chunk data.
    if (pid == 0)
    {
        printf("Chunk size: %d (master), %d (slaves)\n", master_chunk_size, slaves_chunk_size);
        chunk_size = master_chunk_size;
        chunk_data = malloc(NHOURS * chunk_size * sizeof *chunk_data);
        if (chunk_data == NULL)
        {
            fprintf(stderr, "%d: Error: Chunk data error.\n", pid);
            return 0;
        }

        chunk_counts = malloc(np * sizeof *chunk_counts);
        if (chunk_counts == NULL)
        {
            fprintf(stderr, "%d, Error: Chunk counts error.\n", pid);
            return 0;
        }

        chunk_displs = malloc(np * sizeof *chunk_displs);
        if (chunk_data == NULL)
        {
            fprintf(stderr, "%d: Error: Chunk displs error.\n", pid);
            return 0;
        }

        chunk_counts[0] = NHOURS * master_chunk_size;
        for (int n = 1; n < np; ++n)
            chunk_counts[n] = NHOURS * slaves_chunk_size;

        chunk_displs[0] = 0;
        for (int n = 1; n < np; ++n)
            chunk_displs[n] = chunk_displs[n - 1] + chunk_counts[n - 1];
    }
    else
    {
        chunk_size = slaves_chunk_size;
        chunk_data = malloc(NHOURS * chunk_size * sizeof *chunk_data);
        if (chunk_data == NULL)
        {
            fprintf(stderr, "%d: Error: Chunk data error.\n", pid);
            return 0;
        }
    }

    // Step 2. Scatter excluding root dataset chunks (cannot use Scatter).
    if (pid == 0)
        printf("Scattering chunks...");
    MPI_Scatterv(data, chunk_counts, chunk_displs, MPI_FLOAT, chunk_data, NHOURS * chunk_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (pid == 0)
        printf("ok\n");

    // Step 3. Find Neighbors subgroups.
    for (nday = ndays - NPREDICTIONS; nday < ndays; ++nday)
    {
        // Step 3.1. Scatter
        MPI_Scatter(&data[ndays - NPREDICTIONS + nday], NHOURS, MPI_FLOAT, target, NHOURS, MPI_FLOAT, 0, MPI_COMM_WORLD);

        // Step 3.2. kNN
        // knn_kNN(k, target, chunk_data, chunk_size, )

        // Step 3.3. Gather np * k subgroups.
    }

    free(chunk_data);
    if (pid == 0)
    {
        free(chunk_counts);
        free(chunk_displs);
        free(data);
    }
    return 1;
}

int main(int argc, char **argv)
{
    struct knn_args args;
    double time;
    int pid;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &args.np);

    if (pid == 0)
        time = MPI_Wtime();

    if (!init(argc, argv, &args))
    {
        fprintf(stderr, "%d: Error: Initialization aborted.\n", pid);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    omp_set_num_threads(args.nt);
    if (!exec(args.filename, args.k, args.np, args.nt, pid))
    {
        fprintf(stderr, "%d: Error: Execution aborted.\n", pid);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if (pid == 0)
        printf("Total time %.3lfs.\n", MPI_Wtime() - time);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
