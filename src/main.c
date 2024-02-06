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
 * @brief Calculates chunks size for master and slaves based on total and np;
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
 * @return On failure returns zero.
 */
static int exec(char const *filename, int k, int np, int nt)
{
    float *data;
    int pid, ndays, load_ok;
    dataset data_to_analize, data_to_predict;
    int master_chunk_size, slaves_chunk_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    // Step 1. Master loads dataset.
    if (pid == 0)
    {
        load_ok = knn_load_dataset(filename, &ndays, &data);
        if (!load_ok)
        {
            fprintf(stderr, "Error: Dataset loading error.\n");
            return 0;
        }
    }

    // Step 2? Broadcast chunksize.
    calculate_chunk_size(ndays - NPREDICTIONS, np, &master_chunk_size, &slaves_chunk_size);
    MPI_Bcast(&slaves_chunk_size, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

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
