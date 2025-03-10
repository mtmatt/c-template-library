#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <cds/heap.h>
#include <cds/util.h>
#include "test_heap.h"

int int_cmp(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

void test_heap() {
  struct cds_heap heap = cds_heap_new(sizeof(int), int_cmp);

  // Test heap is initially empty
  assert(cds_heap_empty(&heap));

  // Test pushing elements
  int values[] = {5, 3, 8, 1, 2};
  for (int i = 0; i < 5; i++) {
    assert(cds_heap_push(&heap, &values[i]) == 0);
  }

  // Test heap size
  assert(cds_heap_size(&heap) == 5);

  // Test heap top
  int *top = (int *)cds_heap_top(&heap);
  assert(*top == 1);

  // Test popping elements
  assert(cds_heap_pop(&heap) == 0);
  top = (int *)cds_heap_top(&heap);
  assert(*top == 2);

  assert(cds_heap_pop(&heap) == 0);
  top = (int *)cds_heap_top(&heap);
  assert(*top == 3);

  // Test heap size after pops
  assert(cds_heap_size(&heap) == 3);

  // Clean up
  cds_heap_delete(&heap);
}