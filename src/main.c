#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Sorted (easy to access) k-NN arguments.
 */
struct knn_args
{
    int k;
    char const *pathname;
    size_t np, nt;
};

/**
 * @brief Parses arguments.
 *
 * @param       argc Argument count.
 * @param[in]   argv Argument vector.
 * @param[out]  args Arguments.
 * @return Upon successful return, the number of args parsed is returned.
 * If an error is encountered, a negative value is returned.
 */
static int init(int argc, char const **argv, struct knn_args *args)
{
    if (argc != 5)
        return -1;

    *args = (struct knn_args){
        .k = strtol(argv[1], NULL, 10),
        .pathname = argv[2],
        .np = strtol(argv[3], NULL, 10),
        .nt = strtol(argv[4], NULL, 10)};

    return 4;
}

/**
 * @brief Executes program.
 *
 * @param[in] args Arguments.
 * @return Upon successful return, a zero or positive value is returned.
 * If an error is encountered, a negative value is returned.
 */
static int exec(struct knn_args const *args)
{
    return -1;
}

int main(int argc, char **argv)
{
    struct knn_args args;

    if (init(argc, argv, &args) < 0)
    {
        fprintf(stderr, "initialization error\n");
        return EXIT_FAILURE;
    }

    if (exec(&args) < 0)
    {
        fprintf(stderr, "execution error\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
