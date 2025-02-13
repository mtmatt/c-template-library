#include <assert.h>
#include <stdint.h>

#include <cds/array.h>
#include <cds/util.h>
#include "test_array.h"

void test_array() {
  struct cds_array array = cds_array_new(sizeof(int32_t));
  for (int32_t i = 0; i < 10; ++i) {
    cds_array_push_back(&array, &i);
  }
  for (int32_t i = 0; i < 10; ++i) {
    assert(CONV(int32_t) cds_array_at(&array, i) == i);
  }
  for (int32_t i = 0; i < 10; ++i) {
    CONV(int32_t) cds_array_at(&array, i) = 100;
  }
  assert(cds_array_size(&array) == 10);
  for (int32_t i = 0; i < 10; ++i) {
    assert(CONV(int32_t) cds_array_at(&array, i) == 100);
  }
  cds_array_pop_back(&array);
  assert(cds_array_at(&array, 9) == NULL);
  cds_array_delete(&array);
  assert(array.data == NULL);
  assert(array.size == 0);
  assert(array.capacity == 0);
  assert(array.element_size == 0);
}