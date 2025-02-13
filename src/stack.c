#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "stack.h"

struct cds_stack cds_stack_new(const size_t element_size) {
  struct cds_stack new_stack;
  new_stack.data = (char*) malloc(element_size);
  new_stack.size = 0;
  new_stack.capacity = 1;
  new_stack.element_size = element_size;
  return new_stack;
}

void cds_stack_delete(struct cds_stack *stack) {
  free(stack->data);
  stack->data = NULL;
  stack->size = stack->capacity = 0;
  stack->element_size = 0;
}

int cds_stack_push(struct cds_stack *stack, const void *new_element) {
  if (stack->size == stack->capacity) {
    stack->capacity <<= 1;  // cap * 2
    stack->data = (char*) realloc(stack->data, stack->capacity * stack->element_size);
    if (stack->data == NULL) {
      return errno;
    }
  }
  memcpy(stack->data + stack->size * stack->element_size, new_element, stack->element_size);
  stack->size++;
  return 0;
}

int cds_stack_pop(struct cds_stack *stack) {
  if (stack->size == 0) {
    return -1;
  }
  stack->size--;
  return 0;
}

void* cds_stack_top(const struct cds_stack *stack) {
  if (stack->size == 0) {
    return NULL;
  }
  return (void*) (stack->data + (stack->size - 1) * stack->element_size);
}

size_t cds_stack_size(const struct cds_stack *stack) {
  return stack->size;
}

bool cds_stack_empty(const struct cds_stack *stack) {
  return stack->size == 0;
}