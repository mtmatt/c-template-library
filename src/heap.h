#ifndef CDS_HEAP_H
#define CDS_HEAP_H

#include <stddef.h>

#include "array.h"

struct cds_heap {
  struct cds_array data;
  int (*cmp)(const void *, const void *);
};

struct cds_heap cds_heap_new(size_t element_size, int (*cmp)(const void *, const void *));
void cds_heap_delete(struct cds_heap *heap);
int cds_heap_push(struct cds_heap *heap, const void *new_element);
int cds_heap_pop(struct cds_heap *heap);
void* cds_heap_top(const struct cds_heap *heap);
size_t cds_heap_size(const struct cds_heap *heap);
bool cds_heap_empty(const struct cds_heap *heap);

#endif