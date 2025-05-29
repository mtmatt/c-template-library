#ifndef CDS_ARRAY_H
#define CDS_ARRAY_H

#include <stddef.h>
#include <stdbool.h>

typedef struct cds_array {
  char *data;
  size_t size, capacity, element_size;
} CdsArray;

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY NEW
 * 
 * Description: Creates a new dynamic array with the specified element size.
 * 
 * Arguments: element_size   The size of each element in the array.
 *
 * Returns: A newly created struct cds_array instance. The data field can be NULL if memory allocation
 *          fails.
 * 
 * Notes: The caller is responsible for freeing the memory allocated for the array using cds_array_delete.
 *********************************************************************************************************
 */
struct cds_array cds_array_new(size_t element_size);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY DELETE
 * 
 * Description: Frees the memory allocated for the dynamic array.
 * 
 * Arguments: array   A pointer to the struct cds_array instance to be deleted.
 *
 * Returns: none
 * 
 * Notes: The caller should ensure that the array pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
void cds_array_delete(struct cds_array *array);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY PUSH BACK
 * 
 * Description: Adds a new element to the end of the dynamic array.
 * 
 * Arguments: array         A pointer to the struct cds_array instance.
 *            new_element   A pointer to the element to be added to the array.
 *
 * Returns: 0 on success, -1 on failure (e.g., if memory allocation fails).
 * 
 * Notes: The array will automatically resize if its capacity is exceeded.
 *********************************************************************************************************
 */
int cds_array_push_back(struct cds_array *array, const void *new_element);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY POP BACK
 * 
 * Description: Removes the last element from the dynamic array.
 * 
 * Arguments: array   A pointer to the struct cds_array instance.
 *
 * Returns: 0 on success, -1 on failure (e.g., if the array is already empty).
 * 
 * Notes: The size of the array is decreased by one.
 *********************************************************************************************************
 */
int cds_array_pop_back(struct cds_array *array);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY AT
 * 
 * Description: Returns a pointer to the element at the specified index in the dynamic array.
 * 
 * Arguments: array   A pointer to the struct cds_array instance.
 *            index   The index of the element to be accessed.
 *
 * Returns: A pointer to the element at the specified index, or NULL if the index is out of bounds.
 * 
 * Notes: none
 *********************************************************************************************************
 */
void* cds_array_at(const struct cds_array *array, size_t index);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY GET
 * 
 * Description: Returns a pointer to the element at the specified index in the dynamic array.
 * 
 * Arguments: array   A pointer to the struct cds_array instance.
 *            index   The index of the element to be accessed.
 *
 * Returns: A pointer to the element at the specified index.
 * 
 * Notes: The caller should ensure that the index is within the bounds of the array.
 *********************************************************************************************************
 */
void *cds_array_get(const struct cds_array *array, size_t index);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY SIZE
 * 
 * Description: Returns the number of elements in the dynamic array.
 * 
 * Arguments: array   A pointer to the struct cds_array instance.
 *
 * Returns: The number of elements in the array.
 * 
 * Notes: The caller should ensure that the array pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
size_t cds_array_size(const struct cds_array *array);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY EMPTY
 * 
 * Description: Checks if the dynamic array is empty.
 * 
 * Arguments: array   A pointer to the struct cds_array instance.
 *
 * Returns: true if the array is empty, false otherwise.
 * 
 * Notes: The caller should ensure that the array pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
bool cds_array_empty(const struct cds_array *array);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY SORT
 * 
 * Description: Sorts the elements of the dynamic array using the Introsort algorithm.
 * 
 * Arguments: array   A pointer to the struct cds_array instance to be sorted.
 *            compare A pointer to a function that compares two elements.
 *                    The comparison function must return an integer less than, equal to, or greater 
 *                    than zero if the first argument is considered to be respectively less than, 
 *                    equal to, or greater than the second.
 *
 * Returns: 0 on success, -1 on failure (e.g., if memory allocation for the temporary buffer fails).
 * 
 * Notes: The sort is performed in-place.
 *********************************************************************************************************
 */
int cds_array_sort(struct cds_array *array, int (*compare)(const void *, const void *));

#endif