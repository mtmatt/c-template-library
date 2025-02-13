#include <assert.h>
#include <stdint.h>

#include <cds/stack.h>
#include <cds/util.h>

void test_stack() {
  struct cds_stack stack = cds_stack_new(sizeof(int32_t));
  int32_t a = 5;
  cds_stack_push(&stack, (void*) &a);
  a = 10;
  cds_stack_push(&stack, (void*) &a);
  
  assert(CONV(int32_t) cds_stack_top(&stack) == 10);
  assert(cds_stack_size(&stack) == 2);
  assert(cds_stack_pop(&stack) == 0);
  assert(cds_stack_size(&stack) == 1);
  assert(cds_stack_empty(&stack) == false);
  assert(CONV(int32_t) cds_stack_top(&stack) == 5);
  assert(cds_stack_pop(&stack) == 0);
  assert(cds_stack_pop(&stack) == -1);

  cds_stack_delete(&stack);
  assert(stack.data == NULL);
  assert(stack.size == 0);
  assert(stack.capacity == 0);
  assert(stack.element_size == 0);
}