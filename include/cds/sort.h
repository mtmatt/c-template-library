#ifndef CDS_SORT_H
#define CDS_SORT_H

#include "cds/array.h"

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY SORT
 *
 * Description: Sorts the elements of the dynamic array using the Introsort algorithm.
 *
 * Arguments: array   A pointer to the struct cds_array instance to be sorted.
 *            cmp     A pointer to a comparison function.
 *
 * Returns: none
 *
 * Notes: The array is sorted in place.
 *********************************************************************************************************
 */
void cds_array_sort(struct cds_array *array, cds_cmp_func cmp);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY SEARCH LINEAR
 *
 * Description: Searches for an element in the dynamic array using linear search.
 *
 * Arguments: array   A pointer to the struct cds_array instance.
 *            key     A pointer to the element to search for.
 *            cmp     A pointer to a comparison function.
 *
 * Returns: A pointer to the element if found, or NULL if not found.
 *
 * Notes: none
 *********************************************************************************************************
 */
void *cds_array_search_linear(const struct cds_array *array, const void *key, cds_cmp_func cmp);

/*
 *********************************************************************************************************
 *
 *                                            CDS ARRAY SEARCH BINARY
 *
 * Description: Searches for an element in the dynamic array using binary search.
 *
 * Arguments: array   A pointer to the struct cds_array instance.
 *            key     A pointer to the element to search for.
 *            cmp     A pointer to a comparison function.
 *
 * Returns: A pointer to the element if found, or NULL if not found.
 *
 * Notes: The array must be sorted according to the comparison function for the binary search to work correctly.
 *********************************************************************************************************
 */
void *cds_array_search_binary(const struct cds_array *array, const void *key, cds_cmp_func cmp);

#endif
