#include "llist.h"

void insert_sorted(knn_kllist *list, float distance, size_t nk)
{
    knn_knode *new_node = (knn_knode *)malloc(sizeof(knn_knode));
    if (!new_node)
    {
        exit(EXIT_FAILURE);
    }

    new_node->distance = distance;
    new_node->nk = nk;
    new_node->next = NULL;

    knn_knode *current = list->first;
    knn_knode *prev = NULL;

    while (current != NULL && distance < current->distance)
    {
        prev = current;
        current = current->next;
    }

    if (prev == NULL)
    {
        new_node->next = list->first;
        list->first = new_node;
    }
    else
    {
        prev->next = new_node;
        new_node->next = current;
    }

    list->size++;
}