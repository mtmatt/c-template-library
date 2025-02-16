#ifndef CDS_UTIL_H
#define CDS_UTIL_H

/*
 *********************************************************************************************************
 *
 *                                            EXIT WITH ERRNO
 * 
 * Description: Print the format string s to stderr file stream, and perror. 
 * 
 * Arguments: s    is a format string follows the rule defined in stdio.h 
 *            ...  are those inserted values described in the format string
 *
 * Returns: none
 * 
 * Notes: none
 *********************************************************************************************************
 */
#define ERR_EXIT(s, ...) do { fprintf(stderr, s, __VA_ARGS__); perror(""); exit(errno); } \
  while(0);

/*
 *********************************************************************************************************
 *
 *                                  CONVERT void* TO t AND DEREFERENCE
 * 
 * Description: Convert the void pointer to type t and deference
 * 
 * Arguments: t   is the type the caller expected the data should be
 *
 * Returns: none
 * 
 * Notes: none
 *********************************************************************************************************
 */
#define CONV(t) *(t*)

/*
 *********************************************************************************************************
 *                                             MISCELLANEOUS
 *********************************************************************************************************
 */
int max(int a, int b);
int min(int a, int b);

#endif