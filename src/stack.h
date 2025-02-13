#ifndef CDS_STACK_H
#define CDS_STACK_H

typedef struct cds_stack {
  char *data;
  int size, capacity, element_size;
} CdsStack;

struct cds_stack cds_stack_new(const int element_size);
void cds_stack_delete(struct cds_stack *stack);
int cds_stack_push(struct cds_stack *stack, const void *new_element);
int cds_stack_pop(struct cds_stack *stack);
void* cds_stack_top(struct cds_stack *stack);

#endif