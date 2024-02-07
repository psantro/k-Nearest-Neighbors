#include <mpi.h>
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

    *args = (struct knn_args){
        .filename = argv[2],
        .k = strtol(argv[1], NULL, 10),
        .nt = strtol(argv[3], NULL, 10)};

    return 3;
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
 * @brief Calculates chunk range based on chunk size and pid.
 * Ordered by pid as ranked slaves first (1, 2, 3, ...) and master last (0).
 *
 * @param       pid         Process id.
 * @param       np          Number of processes.
 * @param       master_size Master chunk size.
 * @param       slaves_size Slaves chunk size.
 * @param[out]  start       Chunk start index.
 * @param[out]  end         Chunk end index.
 */
static void calculate_chunk_indexes(int pid, int np, int master_size, int slaves_size, int *start, int *end)
{
    if (pid != 0)
    {
        *start = (pid - 1) * slaves_size;
        *end = *start + slaves_size;
    }
    else
    {
        *start = (np - 1) * slaves_size;
        *end = *start + master_size;
    }
}

/**
 * @brief Executes program.
 *
 * @param[in]   filename    Dataset filename.
 * @param       k           Number of neighbors.
 * @param       np          Number of processes.
 * @param       nt          Number of threads.
 * @return On failure returns zero.
 */
static int exec(char const *filename, int k, int np, int nt)
{
    int pid;
    float *data;
    int nday, ndays;
    int master_chunk_size, slaves_chunk_size, chunk_start, chunk_end;
    // dataset data_to_analize, data_to_predict;

    // Step 1.1. Initialize pid.
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    // Step 1.2 Master loads dataset. Master initializes ndays and data buffer.
    if (pid == 0)
    {
        int load_ok = knn_load_dataset(filename, &ndays, &data);
        if (!load_ok)
        {
            fprintf(stderr, "Error: Dataset loading error.\n");
            return 0;
        }
    }

    // Step 1.3. Slaves initializes ndays.
    MPI_Bcast(&ndays, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    // Step 1.4. All initialize chunk sizes and chunk indexes (start-end).
    calculate_chunk_size(ndays - NPREDICTIONS, np, &master_chunk_size, &slaves_chunk_size);
    calculate_chunk_indexes(pid, np, master_chunk_size, slaves_chunk_size, &chunk_start, &chunk_end);

    // Step 1.5. Slaves initializes data buffer.
    if (pid != 0)
    {
        int alloc_ok = knn_allocate_dataset(slaves_chunk_size, &data);
        if (!alloc_ok)
        {
            fprintf(stderr, "Error: Allocation error.\n");
            return 0;
        }
    }

    // Step 2. Scatter dataset chunks.
    MPI_Scatter(data, (np - 1) * NHOURS * slaves_chunk_size, MPI_FLOAT, data, NHOURS * slaves_chunk_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Step 3. Find Neighbors subgroups.
    for (nday = chunk_start; nday < chunk_end; ++nday)
    {
        // Step 3.1. kNN
        // Step 3.2. Gather np * k subgroups.
    }
    /**
     * @todo Make loop scattering and gathering neighbors.
     */

    free(data);
    return 1;
}

int main(int argc, char **argv)
{
    struct knn_args args;
    double start_time, stop_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &args.np);

    if (!init(argc, argv, &args))
    {
        fprintf(stderr, "Error: Initialization aborted.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    start_time = MPI_Wtime();
    if (!exec(args.filename, args.k, args.np, args.nt))
    {
        fprintf(stderr, "Error: Execution aborted.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }
    stop_time = MPI_Wtime() - start_time;

    MPI_Finalize();

    printf("Total time %.3lfs.\n", stop_time);
    return EXIT_SUCCESS;
}
