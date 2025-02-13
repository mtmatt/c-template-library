#include <assert.h>
#include <stdint.h>

#include <cds/stack.h>

int main(void) {
  struct cds_stack stack = cds_stack_new(sizeof(int32_t));
  int32_t a = 5;
  cds_stack_push(&stack, (void*) &a);
  assert(*(int32_t*) cds_stack_top(&stack) == a);
}