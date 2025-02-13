#ifndef CDS_STACK_H
#define CDS_STACK_H

#include <stddef.h>

typedef struct cds_stack {
  char *data;
  size_t size, capacity, element_size;
} CdsStack;

/*
 *********************************************************************************************************
 *
 *                                            CDS STACK NEW
 * 
 * Description: Creates a new stack with the specified element size.
 * 
 * Arguments: element_size   The size of each element in the stack.
 *
 * Returns: A newly created struct cds_stack instance. The data field can be NULL if memory allocation
 *          fails.
 * 
 * Notes: The caller is responsible for freeing the memory allocated for the stack using cds_stack_delete.
 *********************************************************************************************************
 */
struct cds_stack cds_stack_new(const size_t element_size);

/*
 *********************************************************************************************************
 *
 *                                            CDS STACK DELETE
 * 
 * Description: Frees the memory allocated for the stack.
 * 
 * Arguments: stack   A pointer to the struct cds_stack instance to be deleted.
 *
 * Returns: none
 * 
 * Notes: The caller should ensure that the stack pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
void cds_stack_delete(struct cds_stack *stack);

/*
 *********************************************************************************************************
 *
 *                                            CDS STACK PUSH
 * 
 * Description: Adds a new element to the top of the stack.
 * 
 * Arguments: stack        A pointer to the struct cds_stack instance.
 *            new_element  A pointer to the element to be added to the stack.
 *
 * Returns: 0 on success, -1 on failure (e.g., if memory allocation fails).
 * 
 * Notes: The stack will automatically resize if its capacity is exceeded.
 *********************************************************************************************************
 */
int cds_stack_push(struct cds_stack *stack, const void *new_element);

/*
 *********************************************************************************************************
 *
 *                                            CDS STACK POP
 * 
 * Description: Removes the top element from the stack.
 * 
 * Arguments: stack   A pointer to the struct cds_stack instance.
 *
 * Returns: 0 on success, -1 on failure (e.g., if the stack is already empty).
 * 
 * Notes: The size of the stack is decreased by one.
 *********************************************************************************************************
 */
int cds_stack_pop(struct cds_stack *stack);

/*
 *********************************************************************************************************
 *
 *                                            CDS STACK TOP
 * 
 * Description: Returns a pointer to the top element of the stack.
 * 
 * Arguments: stack   A pointer to the struct cds_stack instance.
 *
 * Returns: A pointer to the top element of the stack, or NULL if the stack is empty.
 * 
 * Notes: none
 *********************************************************************************************************
 */
void* cds_stack_top(struct cds_stack *stack);

#endif