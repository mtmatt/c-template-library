#ifndef CDS_UTIL_H
#define CDS_UTIL_H

/*
 *********************************************************************************************************
 *                                            EXIT WITH ERRNO
 * 
 * Description: Print the format string s to stderr file stream, and perror. 
 * 
 * Arguments: s   is a format string follows the rule defined in stdio.h 
 *
 *            ... are those inserted values described in the format string
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
 *                                             MISCELLANEOUS
 *********************************************************************************************************
 */
int max(int a, int b);
int min(int a, int b);

#endif