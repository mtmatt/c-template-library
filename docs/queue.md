# Queue Data Structure

The Queue data structure in CDS implements a First-In, First-Out (FIFO) collection. Elements are added to the rear (end) of the queue and removed from the front. This implementation uses a circular array (also known as a ring buffer) to store the elements.

## `CdsQueue` Struct (Array-based Implementation)

The current implementation of `CdsQueue` is array-based:

```c
typedef struct cds_queue {
  char *data;          // Pointer to the allocated memory for the queue elements
  size_t size;         // Current number of elements in the queue
  size_t capacity;     // Total number of elements the queue can hold before resizing
  size_t element_size; // Size of each element in bytes
  size_t front;        // Index of the front element in the circular array
} CdsQueue;
```

- `data`: A pointer to the block of memory holding the queue elements.
- `size`: The current count of elements in the queue.
- `capacity`: The allocated storage capacity of the array. This can grow dynamically.
- `element_size`: The fixed size (in bytes) of each element stored in the queue.
- `front`: An index that points to the first element of the queue within the `data` array. The rear of the queue is implicitly `(front + size -1) % capacity`.

## Functions

### `cds_queue_new`

```c
struct cds_queue cds_queue_new(const size_t element_size);
```

**Description:** Creates a new, empty queue.

**Arguments:**
- `element_size`: The size (in bytes) of each element that will be stored in the queue.

**Returns:** A `CdsQueue` struct. The `data` field might be `NULL` if the initial memory allocation fails.

**Notes:** The caller is responsible for freeing the memory allocated for the queue using `cds_queue_delete`.

### `cds_queue_delete`

```c
void cds_queue_delete(struct cds_queue *queue);
```

**Description:** Frees the memory allocated for the queue's data.

**Arguments:**
- `queue`: A pointer to the `CdsQueue` struct to be deleted.

**Returns:** None.

**Notes:** The `queue` pointer and its `data` field should not be used after calling this function.

### `cds_queue_push`

```c
int cds_queue_push(struct cds_queue *queue, const void *new_element);
```

**Description:** Adds a new element to the rear (end) of the queue.

**Arguments:**
- `queue`: A pointer to the `CdsQueue` struct.
- `new_element`: A pointer to the element to be added. The content of this element is copied into the queue.

**Returns:** `0` on success, `-1` on failure (e.g., if memory reallocation fails during resize).

**Notes:** The queue automatically resizes (doubles its capacity) if it's full. During resize, elements are re-arranged to be contiguous from the beginning of the new buffer if they wrapped around in the old buffer.

### `cds_queue_pop`

```c
int cds_queue_pop(struct cds_queue *queue);
```

**Description:** Removes the element from the front of the queue.

**Arguments:**
- `queue`: A pointer to the `CdsQueue` struct.

**Returns:** `0` on success, `-1` if the queue is already empty.

**Notes:** This operation advances the `front` index in the circular array.

### `cds_queue_front`

```c
void* cds_queue_front(const struct cds_queue *queue);
```

**Description:** Returns a pointer to the front element of the queue without removing it.

**Arguments:**
- `queue`: A pointer to the `CdsQueue` struct.

**Returns:** A pointer to the front element. If the queue is empty, the behavior is undefined by the function signature (it would point to `queue->data + queue->front * queue->element_size` which is valid memory but not a logical element). It's recommended to check `cds_queue_empty` before calling.

### `cds_queue_size`

```c
size_t cds_queue_size(const struct cds_queue *queue);
```

**Description:** Returns the number of elements currently in the queue.

**Arguments:**
- `queue`: A pointer to the `CdsQueue` struct.

**Returns:** The number of elements in the queue.

### `cds_queue_empty`

```c
bool cds_queue_empty(const struct cds_queue *queue);
```

**Description:** Checks if the queue is empty.

**Arguments:**
- `queue`: A pointer to the `CdsQueue` struct.

**Returns:** `true` if the queue is empty, `false` otherwise.

## Implementation Note

The header file `include/cds/queue.h` shows a preprocessor directive `CDS_QUEUE_IMPL_WITH_ARRAY`. There's a commented-out section for a linked-list based queue implementation, but the source file `src/queue.c` only provides the array-based (circular buffer) implementation. The linked-list version is marked with a `TODO`. This documentation describes the existing array-based implementation.

## Usage Example

```c
#include "cds/queue.h"
#include <stdio.h>

int main() {
    // Create a queue to store integers
    CdsQueue int_queue = cds_queue_new(sizeof(int));

    if (int_queue.data == NULL) {
        fprintf(stderr, "Failed to create queue\n");
        return 1;
    }

    printf("Queue created. Is it empty? %s\n", cds_queue_empty(&int_queue) ? "Yes" : "No");
    // Output: Queue created. Is it empty? Yes

    // Push elements into the queue
    int val1 = 10, val2 = 20, val3 = 30;
    printf("Pushing %d\n", val1);
    cds_queue_push(&int_queue, &val1);
    printf("Front is: %d, Size: %zu\n", *(int*)cds_queue_front(&int_queue), cds_queue_size(&int_queue));

    printf("Pushing %d\n", val2);
    cds_queue_push(&int_queue, &val2);
    printf("Front is: %d, Size: %zu\n", *(int*)cds_queue_front(&int_queue), cds_queue_size(&int_queue));

    printf("Pushing %d\n", val3);
    cds_queue_push(&int_queue, &val3);
    printf("Front is: %d, Size: %zu\n", *(int*)cds_queue_front(&int_queue), cds_queue_size(&int_queue));
    // Output (order of front may vary slightly if initial capacity is 1 and resizes happen):
    // Pushing 10
    // Front is: 10, Size: 1
    // Pushing 20
    // Front is: 10, Size: 2
    // Pushing 30
    // Front is: 10, Size: 3

    printf("\nPopping elements from queue:\n");
    while (!cds_queue_empty(&int_queue)) {
        int *front_val = (int*)cds_queue_front(&int_queue);
        printf("Front: %d, Size before pop: %zu\n", *front_val, cds_queue_size(&int_queue));
        cds_queue_pop(&int_queue);
        printf("Size after pop: %zu\n", cds_queue_size(&int_queue));
    }
    /* Example Output:
    Popping elements from queue:
    Front: 10, Size before pop: 3
    Size after pop: 2
    Front: 20, Size before pop: 2
    Size after pop: 1
    Front: 30, Size before pop: 1
    Size after pop: 0
    */

    printf("\nIs queue empty now? %s\n", cds_queue_empty(&int_queue) ? "Yes" : "No");
    // Output: Is queue empty now? Yes

    // Clean up
    cds_queue_delete(&int_queue);
    printf("Queue deleted.\n");

    return 0;
}
```
