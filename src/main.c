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
 * @param       pid         Process id.
 * @return On failure returns zero.
 */
static int exec(char const *filename, int k, int np, int nt, int pid)
{
    int npid;
    float *data, *chunk_data;
    int *chunk_counts, *chunk_displs;
    int nday, ndays, ok;
    int chunk_size, master_chunk_size, slaves_chunk_size, chunk_start, chunk_end;

    // Step 1.1 Master loads dataset.
    if (pid == 0)
    {
        // Master initializes ndays and data buffer.
        ok = knn_load_dataset(filename, &ndays, &data);
        if (!ok)
        {
            fprintf(stderr, "%d: Error: Dataset loading error.\n", pid);
            return 0;
        }
    }
    else
    {
        // Slaves initializes data to NULL.
        data = NULL;
    }

    // Step 1.2. Slaves initializes ndays.
    MPI_Bcast(&ndays, 1, MPI_INTEGER, 0, MPI_COMM_WORLD);

    // Step 1.3. All initialize chunk sizes and chunk indexes (start-end).
    calculate_chunk_size(ndays - NPREDICTIONS, np, &master_chunk_size, &slaves_chunk_size);
    calculate_chunk_indexes(pid, np, master_chunk_size, slaves_chunk_size, &chunk_start, &chunk_end);

    // Step 1.4. Master initializes chunk data and chunk counts. Slaves initializes chunk data.
    if (pid == 0)
    {
        chunk_data = malloc(master_chunk_size * sizeof *chunk_data);
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

        chunk_displs = malloc(master_chunk_size * sizeof *chunk_displs);
        if (chunk_data == NULL)
        {
            fprintf(stderr, "%d: Error: Chunk displs error.\n", pid);
            return 0;
        }

        for (int n = 0; n < (np - 1); ++n)
            chunk_counts[n] = slaves_chunk_size;
        chunk_counts[np - 1] = master_chunk_size;

        for (int n = 0; n < np; ++n)
            chunk_displs[n] = NHOURS;

        for (int n = 0; n < np; ++n)
            printf("%d\n", chunk_displs[n]);
    }
    else
    {
        chunk_data = malloc(slaves_chunk_size * sizeof *chunk_data);
        if (chunk_data == NULL)
        {
            fprintf(stderr, "%d: Error: Chunk data error.\n", pid);
            return 0;
        }
    }

    /**
     * @todo THIS DOES NOT WORK.
     */
    // Step 2. Scatter excluding root dataset chunks (cannot use Scatter).
    MPI_Scatterv(data, chunk_counts, chunk_displs, MPI_FLOAT, chunk_data, chunk_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
    printf("%d: %.1f\n", pid, chunk_data[0]);

    // Step 3. Find Neighbors subgroups.
    for (nday = chunk_start; nday < chunk_end; ++nday)
    {
        // Step 3.1. Scatter
        // Step 3.2. kNN
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
