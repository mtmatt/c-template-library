#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#ifdef CDS_QUEUE_IMPL_WITH_ARRAY

struct cds_queue cds_queue_new(size_t element_size) {
  struct cds_queue new_queue = {
    .data = (char*) malloc(element_size),
    .size = 0,
    .capacity = 1,
    .element_size = element_size,
    .front = 0};
  return new_queue;
}

void cds_queue_delete(struct cds_queue *queue) {
  free(queue->data);
  queue->data = NULL;
  queue->size = queue->capacity = 0;
  queue->element_size = 0;
  queue->front = 0;
}

int cds_queue_push(struct cds_queue *queue, const void *new_element) {
  if (queue->size == queue->capacity) {
    queue->capacity <<= 1;  // cap * 2
    char *tp_buf = (char*) malloc(queue->capacity * queue->element_size);
    if (tp_buf == NULL) {
      return -1;
    }
    if (queue->front + queue->size >= queue->capacity) {
      const size_t tail_length = queue->capacity - queue->front;
      memmove(tp_buf, queue->data + queue->front * queue->element_size,
        tail_length * queue->element_size);
      memmove(tp_buf + tail_length * queue->element_size, queue->data,
        (queue->size - tail_length) * queue->element_size);
    } else {
      memmove(tp_buf, queue->data + queue->front * queue->element_size,
        queue->size * queue->element_size);
    }
  }
  memcpy(queue->data +
    ((queue->front + queue->size) % queue->capacity) * queue->element_size,
    new_element,
    queue->element_size);
  queue->size++;
  return 0;
}

int cds_queue_pop(struct cds_queue *queue) {
  if (queue->size == 0) {
    return -1;
  }
  queue->front = (queue->front + 1) % queue->capacity;
  queue->size--;
  return 0;
}

void* cds_queue_front(const struct cds_queue *queue) {
  return queue->data + queue->front * queue->element_size;
}

#else

// TODO: link list implmented queue
struct cds_queue cds_queue_new(size_t element_size)
void cds_queue_delete(struct cds_queue *queue);
int cds_queue_push(struct cds_queue *queue, const void *new_element);
int cds_queue_pop(struct cds_queue *queue);
void* cds_queue_front(const struct cds_queue *queue);

#endif

size_t cds_queue_size(const struct cds_queue *queue) {
  return queue->size;
}

bool cds_queue_empty(const struct cds_queue *queue) {
  return queue->size == 0;
}