#include <string.h>

#include "cds/heap.h"
#include "cds/array.h"

struct cds_heap cds_heap_new(size_t element_size, int (*cmp)(const void *, const void *)) {
  struct cds_heap new_heap = {
    .data = cds_array_new(element_size),
    .cmp = cmp};
  new_heap.data.size = 1;
  return new_heap;
}

void cds_heap_delete(struct cds_heap *heap) {
  cds_array_delete(&heap->data);
  heap->cmp = NULL;
}

int cds_heap_increase_key(struct cds_heap *heap) {
  size_t i = heap->data.size - 1;
  while(i > 1 && heap->cmp(cds_array_at(&heap->data, i >> 1), cds_array_at(&heap->data, i)) > 0) {
    static char tp[1024] = {0};
    if (heap->data.element_size < 1024) {
      memmove(tp, cds_array_at(&heap->data, i >> 1), heap->data.element_size);
      memmove(cds_array_at(&heap->data, i >> 1), cds_array_at(&heap->data, i),
        heap->data.element_size);
      memmove(cds_array_at(&heap->data, i), tp, heap->data.element_size);
    } else {
      return -1;
    }
    i >>= 1;
  }
  return 0;
}

int cds_heap_push(struct cds_heap *heap, const void *new_element) {
  if (cds_array_push_back(&heap->data, new_element) != 0) {
    return -1;
  }
  if (cds_heap_increase_key(heap) != 0) {
    return -1;
  }
  return 0;
}

size_t cds_heap_get_smallest(struct cds_heap *heap, size_t index) {
  size_t l = index << 1;
  size_t r = index << 1 | 1;
  size_t smallest = index;
  if (l <= cds_heap_size(heap) &&
      heap->cmp(cds_array_at(&heap->data, l), cds_array_at(&heap->data, smallest)) < 0) {
    smallest = l;
  }
  if (r <= cds_heap_size(heap) &&
      heap->cmp(cds_array_at(&heap->data, r), cds_array_at(&heap->data, smallest)) < 0) {
    smallest = r;
  }
  return smallest;
}

void cds_heap_min_heapify(struct cds_heap *heap, size_t index) {
  size_t smallest = cds_heap_get_smallest(heap, index);
  while(smallest != index) {
    static char tp[1024] = {0};
    memmove(tp, cds_array_at(&heap->data, index), heap->data.element_size);
    memmove(cds_array_at(&heap->data, index), cds_array_at(&heap->data, smallest),
      heap->data.element_size);
    memmove(cds_array_at(&heap->data, smallest), tp, heap->data.element_size);
    index = smallest;
    smallest = cds_heap_get_smallest(heap, smallest);
  }
}

int cds_heap_pop(struct cds_heap *heap) {
  memmove(cds_array_at(&heap->data, 1),
    cds_array_at(&heap->data, heap->data.size - 1),
    heap->data.element_size);
  if (cds_array_pop_back(&heap->data) != 0) {
    return -1;
  }
  cds_heap_min_heapify(heap, 1);
  return 0;
}

void* cds_heap_top(const struct cds_heap *heap) {
  return cds_array_at(&heap->data, 1);
}

size_t cds_heap_size(const struct cds_heap *heap) {
  return heap->data.size - 1;
}

bool cds_heap_empty(const struct cds_heap *heap) {
  return cds_heap_size(heap) == 0;
}