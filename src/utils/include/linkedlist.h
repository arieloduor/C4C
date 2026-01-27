/********************************************************************************************************\
 * FILE: linked_list.h                                                                                  *
 * AUTHOR: Michael Kamau                                                                                *
 *                                                                                                      *
 * PURPOSE:                                                                                             *
 * This header implements a generic singly linked list in C.                                            *
 * It provides dynamic storage for arbitrary data items by copying the data into the list nodes.        *
 * The list maintains pointers to both the head and tail nodes, enabling efficient append (push_back)   *
 * operations and traversal from the front.                                                             *
 *                                                                                                      *
 * USAGE:                                                                                               *
 * - Create a new list using `linked_list()`.                                                           *
 * - Add elements at the back using `push_back(list, data, size)`.                                      *
 * - Remove elements from the front or back using `pop_front()` or `pop_back()`.                        *
 * - Access elements by index with `get_at_index(list, index)` (O(n) operation).                        *
 * - Check the number of elements with `len_of_list(list)`.                                             *
 * - Clear all elements and free memory using `clear_list(&list)`.                                      *
 * EXAMPLE:                                                                                             *
 * LinkedList *list = linked_list();                                                                    *
 * int value = 42;                                                                                      *
 * push_back(list, &value, sizeof(int));                                                                *
 * size_t n = len_of_list(list);                                                                        *
 * pop_front(list);                                                                                     *
 * clear_list(&list);                                                                                   *
\********************************************************************************************************/



/********************************************************************************************************\                                                                                                      *
 * NOTE :                                                                                               *
 * - The list OWNS THE DATA: all `malloc`ed memory in nodes is freed when elements are popped           *
 *   or when the list is cleared.                                                                       *
 * - `pop_front()` and `pop_back()` free both the node and its data; they DO NOT RETURN DATA.           *
 * - Indexing is zero-based; attempting to get an index >= len_of_list(list) returns NULL.              *
 * - This implementation is singly linked; pop_back is O(n).                                            *
 *                                                                                                      *
\********************************************************************************************************/

#ifndef C4C_LINKED_LIST_H
#define C4C_LINKED_LIST_H

#include "utils.hpp"

struct Node
{

    void *data;
    struct  Node *next;
    
};

typedef struct Node Node;

struct LinkedList
{
    Node *head;
    Node *tail;
    size_t len;
};

typedef struct LinkedList LinkedList;

/**
 * 
 * This funciton creates a new empty linked list.
 *
 * Returns:
 *   A pointer to a dynamically allocated LinkedList.
 *   Caller is responsible for freeing it using clear_list().
 */

LinkedList *linked_list()
{
    LinkedList *linkedlist = (LinkedList*)malloc(sizeof(LinkedList));

    if(!linkedlist)
    {
        return NULL;
    }
    linkedlist->head = NULL;
    linkedlist->tail = NULL;
    linkedlist->len = 0;

    return linkedlist;

}

/**
 * This function appends a new element to the end of the list.
 *
 * Parameters:
 *   - list -> pointer to the LinkedList
 *   - data -> pointer to the data to copy into the node
 *   - size -> size of the data in bytes
 *
 * Returns:
 *   true if the insertion succeeded, false on allocation failure or null list.
 *
 * Notes:
 *   The list makes a copy of the data, so the caller retains ownership of the original.
 */

static inline bool push_back(LinkedList *list, void *data, size_t size)
{
    if(!list)
    {
        return false;
    }

    Node *new_node = (Node*)malloc(sizeof(Node));

    if(!new_node)
    {
        return false;
    }
    new_node->data = malloc(size);

    if(!new_node->data)
    {
        free(new_node);
        return false;
    }

    memcpy(new_node->data, data, size);

    if(list->len <= 0)
    {
        list->head = new_node;
        list->tail = new_node;
        list->len++;
        return true;
    }

    list->tail->next = new_node;
    list->tail = new_node;
    new_node->next = NULL;
    list->len++;

    return true;

}


/**
 * This function removes the last element of the list.
 *
 * Parameters:
 *   - list -> pointer to the LinkedList
 *
 * Notes:
 *   - Frees both the node and the data it contains.
 *   - Does nothing if the list is empty or null.
 *   - After pop_back, list->tail and list->head are updated accordingly.
 */

static inline void pop_back(LinkedList *list)
{
    if(!list)
    {
        return;
    }

    if(list->len == 1)
    {
        list->len--;
        free(list->head->data);
        free(list->head);
        list->head = NULL;
        list->tail = NULL;

        return;
    }

    Node * temp = list->head;
    while(temp->next != list->tail)
    {
        temp = temp->next;
    }

    Node *to_pop = temp->next;
    list->tail = temp;
    list->tail->next =  NULL;
    list->len--;

    free(to_pop->data);
    free(to_pop);
}


/**
 * This function removes the first element of the list.
 *
 * Parameters:
 *   - list -> pointer to the LinkedList
 *
 * Notes:
 *   - Frees both the node and the data it contains.
 *   - Does nothing if the list is empty or null.
 *   - After pop_front, list->head and list->tail are updated accordingly.
 */

static inline void pop_front(LinkedList *list)
{
    if(!list)
    {
        return;
    }

    if(list->len == 1)
    {
        free(list->head->data);
        free(list->head);
        list->head = NULL;
        list->tail = NULL;
        list->len--;
        return;
    }

    Node *temp = list->head;
    list->head = list->head->next;
    temp->next = NULL;
    list->len--;
    free(temp->data);
    free(temp);
}


/**
 * This function retrieves the data pointer stored at a specific index.
 *
 * Parameters:
 *   - list -> pointer to the LinkedList
 *   - idx  -> zero-based index of the element
 *
 * Returns:
 *   Pointer to the data at the given index, or NULL if index is out of bounds or list is null.
 *
 * Notes:
 *   - This operation is O(n) in a singly linked list.
 *   - Returned pointer is owned by the list; do not free it.
 */

static inline void *get_at_index(LinkedList *list, size_t idx)
{
    if(!list)
    {
        return NULL;
    }

    if(list->len <= idx)
    {
        return NULL;
    }

    Node *temp = list->head;

    for(size_t i = 0; i<idx; i++)
    {
        temp = temp->next;
    }

    return temp->data;
}


/**
 * This function returns the number of elements currently in the list.
 *
 * Parameters:
 *   - list -> pointer to the LinkedList
 *
 * Returns:
 *   Size of the list (number of nodes). Returns 0 if list is null.
 */

static inline size_t len_of_list(LinkedList *list)
{
    if(!list)
    {
        return 0;
    }

    return list->len;
}


/**
 * This function frees all memory used by the list, including all nodes and their data.
 *
 * Parameters:
 *   - list -> pointer to a LinkedList* (pointer to the list pointer)
 *
 * Notes:
 *   - After this call, *list is set to NULL.
 *   - Safe to call on an already empty or null list.
 */

static inline void clear_list(LinkedList **list)
{
    if(!list)
    {
        return;
    }

    Node *node = (*list)->head;
    for(size_t i=0; i< (*list)->len; i++)
    {
        Node *temp = node->next;
        free(node->data);
        free(node);
        node = temp;
    }
    free(*list);
    *list = NULL;
}

#endif //C4C_LINKED_LIST_H