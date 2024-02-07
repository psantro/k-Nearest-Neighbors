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
    int load_ok;

    if (pid == 0)
    {
        printf("Loading dataset...");
        load_ok = knn_load_dataset(filename, ndays, data);
        if (!load_ok)
        {
            fprintf(stderr, "failed\n%d: Error: Dataset loading error.\n", pid);
            return 0;
        }
        printf("done\n");
    }
    else
    {
        data = NULL;
    }

    return 1;
}

/**
 * @brief Broadcast number of days.
 *
 * @param       pid     Process id.
 * @param[out]  ndays   Number of days.
 * @return On failure returns zero.
 */
static int broadcast_ndays(int pid, int *ndays)
{
    int bcast_ok;

    if (pid == 0)
        printf("Broadcasting ndays...");

    bcast_ok = MPI_Bcast(ndays, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);
    if (bcast_ok != MPI_SUCCESS)
    {
        fprintf(stderr, "failed\n%d: Error: Broadcast ndays error.\n", pid);
        return 0;
    }

    if (pid == 0)
        printf("done\n");

    return 1;
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
 * @brief Initialize necessary chunk metadata.
 *
 * @param       pid                 Process id.
 * @param       np                  Number of processes.
 * @param       chunk_ndays         Number of dataset days to chunk.
 * @param[out]  chunk_size          Current chunk size.
 * @param[out]  chunk_data          Current data.
 * @param[out]  chunk_counts        Current counts.
 * @param[out]  chunk_displs        Current displacements.
 * @return On failure returns zero.
 */
static int initialize_chunk_metadata(int pid, int np, int chunk_ndays, int *chunk_size, float **chunk_data, int **chunk_counts, int **chunk_displs)
{
    int master_chunk_size, slaves_chunk_size, n;

    calculate_chunk_size(chunk_ndays, np, &master_chunk_size, &slaves_chunk_size);
    if (pid == 0)
    {
        printf("Initializing chunk metadata...");
        *chunk_size = master_chunk_size;
        *chunk_data = malloc(NHOURS * *chunk_size * sizeof **chunk_data);
        if (*chunk_data == NULL)
        {
            fprintf(stderr, "failed\n%d: Error: Chunk data error.\n", pid);
            return 0;
        }

        *chunk_counts = malloc(np * sizeof **chunk_counts);
        if (*chunk_counts == NULL)
        {
            fprintf(stderr, "failed\n%d, Error: Chunk counts error.\n", pid);
            return 0;
        }

        *chunk_displs = malloc(np * sizeof **chunk_displs);
        if (*chunk_displs == NULL)
        {
            fprintf(stderr, "failed\n%d: Error: Chunk displs error.\n", pid);
            return 0;
        }

        (*chunk_counts)[0] = NHOURS * master_chunk_size;
        for (n = 1; n < np; ++n)
            (*chunk_counts)[n] = NHOURS * slaves_chunk_size;

        (*chunk_displs)[0] = 0;
        for (n = 1; n < np; ++n)
            (*chunk_displs)[n] = (*chunk_displs)[n - 1] + (*chunk_counts)[n - 1];

        printf("done\n");
        printf("Chunk size: %d (master), %d (slaves)\n", master_chunk_size, slaves_chunk_size);
    }
    else
    {
        *chunk_size = slaves_chunk_size;
        *chunk_data = malloc(NHOURS * *chunk_size * sizeof **chunk_data);
        if (*chunk_data == NULL)
        {
            fprintf(stderr, "\n%d: Error: Chunk data error.\n", pid);
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Scatter chunks forwader.
 *
 * @param       pid             Process id.
 * @param[in]   data            Send data buffer.
 * @param[in]   chunk_counts    Chunk counts.
 * @param[in]   chunk_displs    Chunk displacements.
 * @param[out]  chunk_data      Chunk data.
 * @param       chunk_size      Chunk size.
 * @return On failure returns zero.
 */
static int scatter_chunks(int pid, float const *data, int const *chunk_counts, int const *chunk_displs, float *chunk_data, int chunk_size)
{
    int scatter_ok;

    if (pid == 0)
        printf("Scattering chunks...");

    scatter_ok = MPI_Scatterv(data, chunk_counts, chunk_displs, MPI_FLOAT, chunk_data, NHOURS * chunk_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    if (scatter_ok != MPI_SUCCESS)
    {
        fprintf(stderr, "failed\n%d: Error: Scattering chunks error.\n", pid);
        return 0;
    }

    if (pid == 0)
        printf("done\n");

    return 1;
}

/**
 * @brief Clean everything.
 *
 * @param           pid             Process id.
 * @param[inout]    chunk_data      Chunk data.
 * @param[inout]    chunk_counts    Chunk counts.
 * @param[inout]    chunk_displs    Chunk displacements.
 * @param[inout]    data            Dataset data.
 */
static void cleanup(int pid, float *chunk_data, int *chunk_counts, int *chunk_displs, float *data)
{
    free(chunk_data);
    if (pid == 0)
    {
        free(chunk_counts);
        free(chunk_displs);
        free(data);
    }
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
    float *data, *chunk_data, target[NHOURS];
    int nday, ndays, chunk_size, *chunk_counts, *chunk_displs;

    if (!load_dataset(pid, filename, &ndays, &data))
        return 0;

    if (!broadcast_ndays(pid, &ndays))
        return 0;

    if (!initialize_chunk_metadata(pid, np, ndays - NPREDICTIONS, &chunk_size, &chunk_data, &chunk_counts, &chunk_displs))
        return 0;

    if (!scatter_chunks(pid, data, chunk_counts, chunk_displs, chunk_data, chunk_size))
        return 0;

    // Step 3. Find Neighbors subgroups.
    for (nday = ndays - NPREDICTIONS; nday < ndays; ++nday)
    {
        // Step 3.1. Scatter
        MPI_Scatter(&data[ndays - NPREDICTIONS + nday], NHOURS, MPI_FLOAT, target, NHOURS, MPI_FLOAT, 0, MPI_COMM_WORLD);

        // Step 3.2. kNN
        // knn_kNN(k, target, chunk_data, chunk_size, )

        // Step 3.3. Gather np * k subgroups.
    }

    cleanup(pid, chunk_data, chunk_counts, chunk_displs, data);
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
        printf("Total execution time: %.3lfs\n", MPI_Wtime() - time);

    MPI_Finalize();

    return EXIT_SUCCESS;
}
