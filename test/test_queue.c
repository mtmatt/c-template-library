#include <assert.h>
#include <stdint.h>

#include <cds/queue.h>
#include <cds/util.h>
#include "test_queue.h"

void test_queue() {
  struct cds_queue queue = cds_queue_new(sizeof(int32_t));
  for (int32_t i = 0; i < 10; ++i) {
    cds_queue_push(&queue, &i);
  }
  for (int32_t i = 0; i < 10; ++i) {
    assert(CONV(int32_t) cds_queue_front(&queue) == i);
    cds_queue_pop(&queue);
    assert(cds_queue_size(&queue) == 10 - i - 1);
  }
  cds_queue_delete(&queue);

#ifdef CDS_QUEUE_IMPL_WITH_ARRAY
  assert(queue.data == NULL);
  assert(queue.size == 0);
  assert(queue.capacity == 0);
  assert(queue.element_size == 0);
  assert(queue.front == 0);
#endif

}