#ifndef CDS_LIST_H
#define CDS_LIST_H

#include <stddef.h>
#include <stdbool.h>

typedef struct cds_list_node {
  void *data;
  struct cds_list_node *next, *prev;
} CdsListNode;

/*
 *********************************************************************************************************
 *
 *                                           CDS LIST NODE NEW
 * 
 * Description: Creates a new list node with the specified data.
 * 
 * Arguments: data   A pointer to the data to be stored in the node.
 *
 * Returns: A newly created struct cds_list_node instance. The data field can be NULL if memory allocation
 *          fails.
 * 
 * Notes: The caller is responsible for freeing the memory allocated for the node using
 *        cds_list_node_delete.
 *********************************************************************************************************
 */
struct cds_list_node* cds_list_node_new(void *data, const size_t element_size);

/*
 *********************************************************************************************************
 *
 *                                         CDS LIST NODE DELETE
 * 
 * Description: Deletes the list node and frees all allocated memory.
 * 
 * Arguments: node   A pointer to the struct cds_list_node instance to be deleted.
 *
 * Returns: none
 * 
 * Notes: The caller is responsible for ensuring that the node pointer is valid.
 *********************************************************************************************************
 */
void cds_list_node_delete(struct cds_list_node *node);

/*
 *********************************************************************************************************
 *
 *                                        CDS LIST NODE SET DATA
 * 
 * Description: Sets the data for a given list node.
 * 
 * Arguments: node   A pointer to the list node whose data is to be set.
 *            data   A pointer to the data to be assigned to the list node.
 *
 * Returns: none
 * 
 * Notes: The caller is responsible for ensuring that the node pointer is valid and that the data pointed
 *        to a memory chunk with its size the same as that of the node.data
 *********************************************************************************************************
 */
void cds_list_node_set_data(struct cds_list_node *node, void *data, const size_t element_size);

/*
 *********************************************************************************************************
 *
 *                                        CDS LIST NODE SET NEXT
 * 
 * Description: Sets the next for a given list node.
 * 
 * Arguments: node   A pointer to the list node whose data is to be set.
 *            next   A pointer to the list node to be assigned to node.
 *
 * Returns: none
 * 
 * Notes: The caller is responsible for ensuring that the node and next pointers are valid.
 *********************************************************************************************************
 */
void cds_list_node_set_next(struct cds_list_node *node, struct cds_list_node *next);

/*
 *********************************************************************************************************
 *
 *                                        CDS LIST NODE SET PREV
 * 
 * Description: Sets the prev for a given list node.
 * 
 * Arguments: node   A pointer to the list node whose data is to be set.
 *            prev   A pointer to the list node to be assigned to node.
 *
 * Returns: none
 * 
 * Notes: The caller is responsible for ensuring that the node and prev pointers are valid.
 *********************************************************************************************************
 */
void cds_list_node_set_prev(struct cds_list_node *node, struct cds_list_node *prev);

typedef struct cds_list {
  struct cds_list_node *head, *tail;
  size_t size, element_size;
} CdsList;

/*
 *********************************************************************************************************
 *
 *                                             CDS LIST NEW
 * 
 * Description: Creates a new dynamic list with the specified element size.
 * 
 * Arguments: element_size   The size of each element in the list.
 *
 * Returns: A newly created struct cds_list instance.
 * 
 * Notes: The caller is responsible for freeing the memory allocated for the list using cds_list_delete.
 *********************************************************************************************************
 */
struct cds_list cds_list_new(const size_t element_size);

/*
 *********************************************************************************************************
 *
 *                                            CDS LIST DELETE
 * 
 * Description: Deletes the dynamic list and frees all allocated memory.
 * 
 * Arguments: list   A pointer to the struct cds_list instance to be deleted.
 *
 * Returns: none
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
void cds_list_delete(struct cds_list *list);

/*
 *********************************************************************************************************
 *
 *                                          CDS LIST PUSH FRONT
 * 
 * Description: Inserts a new element at the front of the list.
 * 
 * Arguments: list          A pointer to the struct cds_list instance.
 *            new_element   A pointer to the new element to be added to the list.
 *
 * Returns: 0 on success, -1 on failure (e.g., memory allocation failure).
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
int cds_list_push_front(struct cds_list *list, void *new_element);

/*
 *********************************************************************************************************
 *
 *                                          CDS LIST PUSH BACK
 * 
 * Description: Inserts a new element at the end of the list.
 * 
 * Arguments: list          A pointer to the struct cds_list instance.
 *            new_element   A pointer to the new element to be added to the list.
 *
 * Returns: 0 on success, -1 on failure (e.g., memory allocation failure).
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
int cds_list_push_back(struct cds_list *list, void *new_element);

/*
 *********************************************************************************************************
 *
 *                                          CDS LIST POP FRONT
 * 
 * Description: Removes the element at the front of the list.
 * 
 * Arguments: list   A pointer to the struct cds_list instance.
 *
 * Returns: 0 on success, -1 on failure (e.g., list is empty).
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
int cds_list_pop_front(struct cds_list *list);

/*
 *********************************************************************************************************
 *
 *                                           CDS LIST POP BACK
 * 
 * Description: Removes the element at the end of the list.
 * 
 * Arguments: list   A pointer to the struct cds_list instance.
 *
 * Returns: 0 on success, -1 on failure (e.g., list is empty).
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
int cds_list_pop_back(struct cds_list *list);

/*
 *********************************************************************************************************
 *
 *                                            CDS LIST INSERT
 * 
 * Description: Inserts a new element after the specified position in the list.
 * 
 * Arguments: list          A pointer to the struct cds_list instance.
 *            position      A pointer to the list node after which the new element will be inserted.
 *            new_element   A pointer to the new element to be added to the list.
 *
 * Returns: 0 on success, -1 on failure (e.g., memory allocation failure).
 * 
 * Notes: The caller is responsible for ensuring that the list and position pointers are valid.
 *********************************************************************************************************
 */
int cds_list_insert(struct cds_list *list, struct cds_list_node *position, void *new_element);

/*
 *********************************************************************************************************
 *
 *                                            CDS LIST REMOVE
 * 
 * Description: Remove an element at the specified position in the list.
 * 
 * Arguments: list       A pointer to the struct cds_list instance.
 *            position   A pointer to the list node after which the new element will be removed.
 *
 * Returns: 0 on success, -1 on failure (e.g., position is NULL).
 * 
 * Notes: The caller is responsible for ensuring that the list is valid.
 *********************************************************************************************************
 */
int cds_list_remove(struct cds_list *list, struct cds_list_node *position);

/*
 *********************************************************************************************************
 *
 *                                            CDS LIST CONCAT
 * 
 * Description: Concatenates the second list to the end of the first list.
 * 
 * Arguments: first    A pointer to the first struct cds_list instance.
 *            second   A pointer to the second struct cds_list instance.
 *
 * Returns: 0 on success, -1 on failure (e.g., invalid list pointers).
 * 
 * Notes: The caller is responsible for ensuring that the list pointers are valid. The second list will be
 *        emptied and its nodes will be appended to the first list.
 *********************************************************************************************************
 */
int cds_list_concat(struct cds_list *first, struct cds_list *second);

/*
 *********************************************************************************************************
 *
 *                                           CDS LIST GET HEAD
 * 
 * Description: Retrieves the data at the head of the list.
 * 
 * Arguments: list   A pointer to the struct cds_list instance.
 *
 * Returns: A pointer to the data at the head of the list, or NULL if the list is empty.
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
void* cds_list_get_head(struct cds_list *list);

/*
 *********************************************************************************************************
 *
 *                                           CDS LIST GET TAIL
 * 
 * Description: Retrieves the data at the tail of the list.
 * 
 * Arguments: list   A pointer to the struct cds_list instance.
 *
 * Returns: A pointer to the data at the tail of the list, or NULL if the list is empty.
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
void* cds_list_get_tail(struct cds_list *list);

/*
 *********************************************************************************************************
 *
 *                                            CDS LIST SEARCH
 * 
 * Description: Searches for an element in the list.
 * 
 * Arguments: list   A pointer to the struct cds_list instance.
 *            data   A pointer to the data to be searched in the list.
 *            cmp    A function pointer to the comparison function. It should return 0 if the elements are
 *                   equal, and non-zero if the elements are not equal.
 *
 * Returns: A pointer to the list node containing the data, or NULL if the data is not found (e.g., the
 *          data is not in the list, the list is NULL, or the cmp is NULL).
 * 
 * Notes: The caller is responsible for ensuring that the list pointer and cmp function pointer are valid.
 *********************************************************************************************************
 */
struct cds_list_node* cds_list_search(struct cds_list *list, void *data, int (*cmp)(const void *, const void *));

/*
 *********************************************************************************************************
 *
 *                                             CDS LIST SIZE
 * 
 * Description: Retrieves the number of elements in the list.
 * 
 * Arguments: list   A pointer to the struct cds_list instance.
 *
 * Returns: The number of elements in the list.
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
size_t cds_list_size(struct cds_list *list);

/*
 *********************************************************************************************************
 *
 *                                            CDS LIST EMPTY
 * 
 * Description: Checks if the list is empty.
 * 
 * Arguments: list   A pointer to the struct cds_list instance.
 *
 * Returns: true if the list is empty, false otherwise.
 * 
 * Notes: The caller is responsible for ensuring that the list pointer is valid.
 *********************************************************************************************************
 */
bool cds_list_empty(struct cds_list *list);

#endif