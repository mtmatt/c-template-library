#ifndef CDS_HEAP_H
#define CDS_HEAP_H

#include <stddef.h>

#include "array.h"

struct cds_heap {
  struct cds_array data;
  int (*cmp)(const void *, const void *);
};

/*
 *********************************************************************************************************
 *
 *                                            CDS HEAP NEW
 * 
 * Description: Creates a new heap with the specified element size and comparison function.
 * 
 * Arguments: element_size   The size of each element in the heap.
 *            cmp            A pointer to a comparison function that determines the order of elements.
 *
 * Returns: A newly created struct cds_heap instance. The data field can be NULL if memory allocation
 *          fails.
 * 
 * Notes: The caller is responsible for freeing the memory allocated for the heap using cds_heap_delete.
 *********************************************************************************************************
 */
struct cds_heap cds_heap_new(size_t element_size, int (*cmp)(const void *, const void *));

/*
 *********************************************************************************************************
 *
 *                                            CDS HEAP DELETE
 * 
 * Description: Deletes the heap and frees the memory allocated for it.
 * 
 * Arguments: heap   A pointer to the heap to be deleted.
 *
 * Returns: None.
 * 
 * Notes: The heap pointer should not be used after calling this function.
 *********************************************************************************************************
 */
void cds_heap_delete(struct cds_heap *heap); 

/*
 *********************************************************************************************************
 *
 *                                            CDS HEAP PUSH
 * 
 * Description: Inserts a new element into the heap.
 * 
 * Arguments: heap          A pointer to the heap.
 *            new_element   A pointer to the new element to be inserted.
 *
 * Returns: 0 on success, -1 on failure (e.g., memory allocation failure).
 * 
 * Notes: The new element is copied into the heap.
 *********************************************************************************************************
 */
int cds_heap_push(struct cds_heap *heap, const void *new_element);

/*
 *********************************************************************************************************
 *
 *                                            CDS HEAP POP
 * 
 * Description: Removes the top element from the heap.
 * 
 * Arguments: heap   A pointer to the heap.
 *
 * Returns: 0 on success, -1 on failure (e.g., if the heap is empty).
 * 
 * Notes: The top element is determined by the comparison function provided during heap creation.
 *********************************************************************************************************
 */
int cds_heap_pop(struct cds_heap *heap);

/*
 *********************************************************************************************************
 *
 *                                            CDS HEAP TOP
 * 
 * Description: Retrieves the top element of the heap without removing it.
 * 
 * Arguments: heap   A pointer to the heap.
 *
 * Returns: A pointer to the top element, or NULL if the heap is empty.
 * 
 * Notes: The top element is determined by the comparison function provided during heap creation.
 *********************************************************************************************************
 */
void* cds_heap_top(const struct cds_heap *heap);

/*
 *********************************************************************************************************
 *
 *                                            CDS HEAP SIZE
 * 
 * Description: Returns the number of elements in the heap.
 * 
 * Arguments: heap   A pointer to the heap.
 *
 * Returns: The number of elements in the heap.
 * 
 * Notes: None.
 *********************************************************************************************************
 */
size_t cds_heap_size(const struct cds_heap *heap);

/*
 *********************************************************************************************************
 *
 *                                            CDS HEAP EMPTY
 * 
 * Description: Checks if the heap is empty.
 * 
 * Arguments: heap   A pointer to the heap.
 *
 * Returns: true if the heap is empty, false otherwise.
 * 
 * Notes: None.
 *********************************************************************************************************
 */
bool cds_heap_empty(const struct cds_heap *heap);

#endif