# Stack Data Structure

The Stack data structure in CDS implements a Last-In, First-Out (LIFO) collection. Elements are added (pushed) and removed (popped) from the same end, referred to as the "top" of the stack. This implementation uses a dynamic array to store the elements, which resizes automatically as needed.

## `CdsStack` Struct

The `CdsStack` struct defines the structure of the stack:

```c
typedef struct cds_stack {
  char *data;          // Pointer to the allocated memory for stack elements
  size_t size;         // Current number of elements in the stack
  size_t capacity;     // Total number of elements the stack can hold before resizing
  size_t element_size; // Size of each element in bytes
} CdsStack;
```

- `data`: A pointer to the block of memory holding the stack elements.
- `size`: The current count of elements on the stack. This also indicates the position of the top element.
- `capacity`: The allocated storage capacity of the underlying array. This grows dynamically.
- `element_size`: The fixed size (in bytes) of each element stored in the stack.

## Functions

### `cds_stack_new`

```c
struct cds_stack cds_stack_new(const size_t element_size);
```

**Description:** Creates a new, empty stack.

**Arguments:**
- `element_size`: The size (in bytes) of each element that will be stored in the stack.

**Returns:** A `CdsStack` struct. The `data` field might be `NULL` if the initial memory allocation fails.

**Notes:** The caller is responsible for freeing the memory allocated for the stack using `cds_stack_delete`.

### `cds_stack_delete`

```c
void cds_stack_delete(struct cds_stack *stack);
```

**Description:** Frees the memory allocated for the stack's underlying data storage.

**Arguments:**
- `stack`: A pointer to the `CdsStack` struct to be deleted.

**Returns:** None.

**Notes:** The `stack` pointer and its `data` field should not be used after calling this function. Ensure the `stack` pointer is not `NULL`.

### `cds_stack_push`

```c
int cds_stack_push(struct cds_stack *stack, const void *new_element);
```

**Description:** Adds a new element to the top of the stack.

**Arguments:**
- `stack`: A pointer to the `CdsStack` struct.
- `new_element`: A pointer to the element to be added. The content of this element is copied onto the stack.

**Returns:** `0` on success, `-1` on failure (e.g., if memory reallocation fails when the stack needs to grow).

**Notes:** The stack automatically resizes (typically by doubling its capacity) if it is full.

### `cds_stack_pop`

```c
int cds_stack_pop(struct cds_stack *stack);
```

**Description:** Removes the element from the top of the stack.

**Arguments:**
- `stack`: A pointer to the `CdsStack` struct.

**Returns:** `0` on success, `-1` if the stack is already empty.

**Notes:** This operation simply decrements the `size` of the stack; it does not erase the data in memory but makes that slot available for the next push.

### `cds_stack_top`

```c
void* cds_stack_top(const struct cds_stack *stack);
```

**Description:** Returns a pointer to the element at the top of the stack without removing it.

**Arguments:**
- `stack`: A pointer to the `CdsStack` struct.

**Returns:** A pointer to the top element. Returns `NULL` if the stack is empty.

### `cds_stack_size`

```c
size_t cds_stack_size(const struct cds_stack *stack);
```

**Description:** Returns the number of elements currently in the stack.

**Arguments:**
- `stack`: A pointer to the `CdsStack` struct.

**Returns:** The number of elements in the stack.

**Notes:** Ensure the `stack` pointer is not `NULL`.

### `cds_stack_empty`

```c
bool cds_stack_empty(const struct cds_stack *stack);
```

**Description:** Checks if the stack is empty.

**Arguments:**
- `stack`: A pointer to the `CdsStack` struct.

**Returns:** `true` if the stack is empty, `false` otherwise.

**Notes:** Ensure the `stack` pointer is not `NULL`.

## Usage Example

```c
#include "cds/stack.h"
#include <stdio.h>

int main() {
    // Create a stack to store integers
    CdsStack int_stack = cds_stack_new(sizeof(int));

    if (int_stack.data == NULL) {
        fprintf(stderr, "Failed to create stack\n");
        return 1;
    }

    printf("Stack created. Is it empty? %s\n", cds_stack_empty(&int_stack) ? "Yes" : "No");
    // Output: Stack created. Is it empty? Yes

    // Push elements onto the stack
    int val1 = 10, val2 = 20, val3 = 30;
    printf("Pushing %d\n", val1);
    cds_stack_push(&int_stack, &val1);
    if (!cds_stack_empty(&int_stack))
        printf("Top is: %d, Size: %zu\n", *(int*)cds_stack_top(&int_stack), cds_stack_size(&int_stack));

    printf("Pushing %d\n", val2);
    cds_stack_push(&int_stack, &val2);
    if (!cds_stack_empty(&int_stack))
        printf("Top is: %d, Size: %zu\n", *(int*)cds_stack_top(&int_stack), cds_stack_size(&int_stack));

    printf("Pushing %d\n", val3);
    cds_stack_push(&int_stack, &val3);
    if (!cds_stack_empty(&int_stack))
        printf("Top is: %d, Size: %zu\n", *(int*)cds_stack_top(&int_stack), cds_stack_size(&int_stack));
    /* Output:
    Pushing 10
    Top is: 10, Size: 1
    Pushing 20
    Top is: 20, Size: 2
    Pushing 30
    Top is: 30, Size: 3
    */

    printf("\nPopping elements from stack:\n");
    while (!cds_stack_empty(&int_stack)) {
        int *top_val = (int*)cds_stack_top(&int_stack);
        printf("Top: %d, Size before pop: %zu\n", *top_val, cds_stack_size(&int_stack));
        cds_stack_pop(&int_stack);
        printf("Size after pop: %zu\n", cds_stack_size(&int_stack));
    }
    /* Output:
    Popping elements from stack:
    Top: 30, Size before pop: 3
    Size after pop: 2
    Top: 20, Size before pop: 2
    Size after pop: 1
    Top: 10, Size before pop: 1
    Size after pop: 0
    */

    printf("\nIs stack empty now? %s\n", cds_stack_empty(&int_stack) ? "Yes" : "No");
    // Output: Is stack empty now? Yes

    // Clean up
    cds_stack_delete(&int_stack);
    printf("Stack deleted.\n");

    return 0;
}
```
