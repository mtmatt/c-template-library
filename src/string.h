#ifndef CDS_STRING_H
#define CDS_STRING_H

#include <stddef.h>
#include <stdbool.h>

#include "array.h"

#ifndef CDS_STRING_OPT_CAPACITY
#define CDS_STRING_OPT_CAPACITY 8
#endif

typedef struct cds_string {
  char opt_data[CDS_STRING_OPT_CAPACITY], *data;
  size_t size, capacity;
} CdsString;

/*
 *********************************************************************************************************
 *
 *                                            CDS STRING NEW
 * 
 * Description: Creates a new string.
 * 
 * Arguments: none
 *
 * Returns: A newly created struct cds_string instance. The data field is NULL at first.
 * 
 * Notes: The caller is responsible for freeing the memory allocated for the string using
 *        cds_string_delete.
 *********************************************************************************************************
 */
struct cds_string cds_string_new();

/*
 *********************************************************************************************************
 *
 *                                            CDS STRING FROM
 * 
 * Description: Creates a new string from a c string
 * 
 * Arguments: data     A c string.
 *            length   The length of the string data.
 *
 * Returns: A newly created struct cds_string instance. The data field may be NULL at first.
 * 
 * Notes: The caller is responsible for freeing the memory allocated for the string using
 *        cds_string_delete.
 *********************************************************************************************************
 */
struct cds_string cds_string_from(char *data, size_t length);

/*
 *********************************************************************************************************
 *
 *                                            CDS STRING MOVE
 * 
 * Description: Move the second string to the first one.
 * 
 * Arguments: first    The destination where the second string moved to.
 *            second   The string going to be moved.
 *
 * Returns: none
 * 
 * Notes: The caller should ensure that both string are valid. The first string will be deleted after the
 *        function call.
 *********************************************************************************************************
 */
void cds_string_move(struct cds_string *first,  struct cds_string *second);

/*
 *********************************************************************************************************
 *
 *                                           CDS STRING DELETE
 * 
 * Description: Frees the memory allocated for the string.
 * 
 * Arguments: string   A pointer to the struct cds_string instance to be deleted.
 *
 * Returns: none
 * 
 * Notes: The caller should ensure that the string pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
void cds_string_delete(struct cds_string *string);

/*
 *********************************************************************************************************
 *
 *                                            CDS STRING GET
 * 
 * Description: Get the pointer storing the data.
 * 
 * Arguments: string   A pointer to the struct cds_string instance.
 *
 * Returns: A pointer pointed to the data, or NULL if the string is not valid.
 * 
 * Notes: none
 *********************************************************************************************************
 */
char* cds_string_get(struct cds_string *string);

/*
 *********************************************************************************************************
 *
 *                                            CDS STRING SIZE
 * 
 * Description: Returns the number of elements in the string.
 * 
 * Arguments: string   A pointer to the struct cds_string instance.
 *
 * Returns: The number of elements in the string.
 * 
 * Notes: The caller should ensure that the string pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
size_t cds_string_size(const struct cds_string *string);

/*
 *********************************************************************************************************
 * 
 *                                            CDS STRING EMPTY
 * 
 * Description: Checks if the string is empty.
 * 
 * Arguments: string   A pointer to the struct cds_string instance.
 *
 * Returns: true if the string is empty, false otherwise.
 * 
 * Notes: The caller should ensure that the string pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
bool cds_string_empty(const struct cds_string *string);

/*
 *********************************************************************************************************
 * 
 *                                            CDS STRING SPLIT
 * 
 * Description: Split a string into an array of string according to the delimiters
 * 
 * Arguments: string       A pointer to the struct cds_string instance.
 *            delimiters   The chars decide where the string have to be split.
 *
 * Returns: An array storing one or more cds_string(s).
 * 
 * Notes: The caller should ensure that the string pointer is not NULL before calling this function.
 *        Unlike C library, the original string will not be modified.
 *********************************************************************************************************
 */
struct cds_array cds_string_split(const struct cds_string *string, char delimiters[]);

/*
 *********************************************************************************************************
 * 
 *                                           CDS STRING CONCAT
 * 
 * Description: Concat the second string to the first one.
 * 
 * Arguments: first    The destination.
 *            second   A string preparing to be concat to the first.
 *
 * Returns: 0 on success, -1 on failure (e.g., if memory allocation fails).
 * 
 * Notes: The caller should ensure that the string pointer is not NULL before calling this function.
 *        The second string will not be modified.
 *********************************************************************************************************
 */
int cds_string_concat(struct cds_string *first, const struct cds_string *second);

// TODO: push, pop, resize

#endif