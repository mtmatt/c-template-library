#ifndef CDS_QUEUE_H
#define CDS_QUEUE_H

#include <stddef.h>
#include <stdbool.h>

#define CDS_QUEUE_IMPL_WITH_ARRAY

#ifdef CDS_QUEUE_IMPL_WITH_ARRAY

typedef struct cds_queue {
  char *data;
  size_t size, capacity, element_size, front;
} CdsQueue;

#else

static struct cds_queue_node {
  void *data;
  struct cds_queue_node *next;
};

typedef struct cds_queue {
  struct cds_queue_node *head, *tail;
  size_t size, element_size;
} CdsQueue;

#endif

/*
 *********************************************************************************************************
 *
 *                                            CDS QUEUE NEW
 * 
 * Description: Creates a new queue with the specified element size.
 * 
 * Arguments: element_size   The size of each element in the queue.
 *
 * Returns: A newly created struct cds_queue instance. The data field can be NULL if memory allocation
 *          fails.
 * 
 * Notes: The caller is responsible for freeing the memory allocated for the queue using cds_queue_delete.
 *********************************************************************************************************
 */
struct cds_queue cds_queue_new(size_t element_size);

/*
 *********************************************************************************************************
 *
 *                                            CDS QUEUE DELETE
 * 
 * Description: Frees the memory allocated for the queue.
 * 
 * Arguments: queue   A pointer to the struct cds_queue instance to be deleted.
 *
 * Returns: none
 * 
 * Notes: The caller should ensure that the queue pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
void cds_queue_delete(struct cds_queue *queue);

/*
 *********************************************************************************************************
 *
 *                                            CDS QUEUE PUSH
 * 
 * Description: Adds a new element to the end of the queue.
 * 
 * Arguments: queue        A pointer to the struct cds_queue instance.
 *            new_element  A pointer to the element to be added to the queue.
 *
 * Returns: 0 on success, -1 on failure (e.g., if memory allocation fails).
 * 
 * Notes: The queue will automatically resize if its capacity is exceeded.
 *********************************************************************************************************
 */
int cds_queue_push(struct cds_queue *queue, const void *new_element);
 
/*
 *********************************************************************************************************
 *
 *                                             CDS QUEUE POP
 * 
 * Description: Removes the front element from the queue.
 * 
 * Arguments: queue   A pointer to the struct cds_queue instance.
 *
 * Returns: 0 on success, -1 on failure (e.g., if the queue is already empty).
 * 
 * Notes: The size of the queue is decreased by one.
 *********************************************************************************************************
 */
int cds_queue_pop(struct cds_queue *queue);
 
/*
 *********************************************************************************************************
 *
 *                                            CDS QUEUE FRONT
 * 
 * Description: Returns a pointer to the front element in the queue.
 * 
 * Arguments: queue   A pointer to the struct cds_queue instance.
 *
 * Returns: A pointer to the front element, or NULL if the queue is empty.
 * 
 * Notes: none
 *********************************************************************************************************
 */
void* cds_queue_front(const struct cds_queue *queue);
 
/*
 *********************************************************************************************************
 *
 *                                            CDS QUEUE SIZE
 * 
 * Description: Returns the number of elements in the queue.
 * 
 * Arguments: queue   A pointer to the struct cds_queue instance.
 *
 * Returns: The number of elements in the queue.
 * 
 * Notes: The caller should ensure that the queue pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
size_t cds_queue_size(const struct cds_queue *queue);
 
/*
 *********************************************************************************************************
 *
 *                                            CDS QUEUE EMPTY
 * 
 * Description: Checks if the queue is empty.
 * 
 * Arguments: queue   A pointer to the struct cds_queue instance.
 *
 * Returns: true if the queue is empty, false otherwise.
 * 
 * Notes: The caller should ensure that the queue pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
bool cds_queue_empty(const struct cds_queue *queue);

#endif