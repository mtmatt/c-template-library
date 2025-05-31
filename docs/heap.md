# Heap Data Structure

The Heap data structure in CDS is a binary heap, specifically a **min-heap**. It uses an underlying dynamic array for storage. A min-heap maintains the property that for any given node, its value is less than or equal to the values of its children. The smallest element is always at the root of the heap.

## `cds_heap` Struct

The `cds_heap` struct defines the structure of the heap:

```c
struct cds_heap {
  struct cds_array data; // Underlying dynamic array to store heap elements
  int (*cmp)(const void *, const void *); // Comparison function
};
```

- `data`: A `CdsArray` struct that stores the elements of the heap. The heap internally uses 1-based indexing for convenience in parent/child calculations, so the 0th index of the array is unused.
- `cmp`: A function pointer to a comparison function that determines the order of elements. For the min-heap implementation, this function should return:
    - A negative value if the first argument is less than the second.
    - Zero if the first argument is equal to the second.
    - A positive value if the first argument is greater than the second.

## Functions

### `cds_heap_new`

```c
struct cds_heap cds_heap_new(size_t element_size, int (*cmp)(const void *, const void *));
```

**Description:** Creates a new, empty min-heap.

**Arguments:**
- `element_size`: The size of each element to be stored in the heap (in bytes).
- `cmp`: A pointer to the comparison function.

**Returns:** A `cds_heap` struct. The `data.data` field (from the underlying `CdsArray`) might be `NULL` if memory allocation fails during array initialization.

**Notes:** The caller is responsible for freeing the memory allocated for the heap using `cds_heap_delete`.

### `cds_heap_delete`

```c
void cds_heap_delete(struct cds_heap *heap);
```

**Description:** Deletes the heap and frees the memory allocated for its underlying array.

**Arguments:**
- `heap`: A pointer to the `cds_heap` struct to be deleted.

**Returns:** None.

**Notes:** The `heap` pointer should not be used after calling this function.

### `cds_heap_push`

```c
int cds_heap_push(struct cds_heap *heap, const void *new_element);
```

**Description:** Inserts a new element into the min-heap, maintaining the heap property.

**Arguments:**
- `heap`: A pointer to the `cds_heap` struct.
- `new_element`: A pointer to the element to be added. The element's content is copied into the heap.

**Returns:** `0` on success, `-1` on failure (e.g., memory allocation failure in the underlying array or if the temporary buffer in `cds_heap_increase_key` is too small for `element_size`).

### `cds_heap_pop`

```c
int cds_heap_pop(struct cds_heap *heap);
```

**Description:** Removes the smallest element (the root) from the min-heap and readjusts the heap to maintain its property.

**Arguments:**
- `heap`: A pointer to the `cds_heap` struct.

**Returns:** `0` on success, `-1` on failure (e.g., if the heap is empty or if the temporary buffer in `cds_heap_min_heapify` is too small).

### `cds_heap_top`

```c
void* cds_heap_top(const struct cds_heap *heap);
```

**Description:** Retrieves a pointer to the smallest element (the root) of the min-heap without removing it.

**Arguments:**
- `heap`: A pointer to the `cds_heap` struct.

**Returns:** A pointer to the top element, or `NULL` if the heap is empty. The pointer refers to data stored within the heap.

### `cds_heap_size`

```c
size_t cds_heap_size(const struct cds_heap *heap);
```

**Description:** Returns the number of elements currently in the heap.

**Arguments:**
- `heap`: A pointer to the `cds_heap` struct.

**Returns:** The number of elements in the heap.

### `cds_heap_empty`

```c
bool cds_heap_empty(const struct cds_heap *heap);
```

**Description:** Checks if the heap is empty.

**Arguments:**
- `heap`: A pointer to the `cds_heap` struct.

**Returns:** `true` if the heap is empty, `false` otherwise.

## Internal Helper Functions (Conceptual)

While not part of the public API, the implementation uses helper functions that are crucial for heap operations:
- `cds_heap_increase_key` (misleading name, actually `heapify_up` or `bubble_up`): After adding an element to the end of the array, this function moves it up the heap to its correct position to maintain the min-heap property.
- `cds_heap_min_heapify` (or `heapify_down`, `bubble_down`): After replacing the root with the last element (during pop), this function moves the new root down the heap to its correct position to maintain the min-heap property.
- `cds_heap_get_smallest`: A helper to find the smallest among a node and its children, used by `cds_heap_min_heapify`.

**Important Note on `element_size`:** The internal swapping mechanism in `cds_heap_increase_key` and `cds_heap_min_heapify` uses a fixed-size temporary buffer (`static char tp[1024]`). If `element_size` is larger than 1024 bytes, these operations might lead to buffer overflows or incorrect behavior. The `cds_heap_increase_key` function has a check and returns -1 if `element_size` is too large, but `cds_heap_min_heapify` does not explicitly show such a check in the provided `src/heap.c` and might be unsafe for large elements.

## Usage Example

```c
#include "cds/heap.h"
#include <stdio.h>

// Comparison function for integers (for a min-heap)
int compare_ints(const void *a, const void *b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    if (int_a < int_b) return -1;
    if (int_a > int_b) return 1;
    return 0;
}

int main() {
    // Create a min-heap for integers
    struct cds_heap int_heap = cds_heap_new(sizeof(int), compare_ints);

    if (int_heap.data.data == NULL) { // Check if underlying array allocation failed
        fprintf(stderr, "Failed to create heap\n");
        return 1;
    }

    printf("Heap created. Is it empty? %s\n", cds_heap_empty(&int_heap) ? "Yes" : "No");
    // Output: Heap created. Is it empty? Yes

    // Push elements onto the heap
    int elements[] = {10, 5, 20, 3, 8};
    int num_elements = sizeof(elements) / sizeof(elements[0]);

    for (int i = 0; i < num_elements; ++i) {
        printf("Pushing: %d\n", elements[i]);
        if (cds_heap_push(&int_heap, &elements[i]) != 0) {
            fprintf(stderr, "Error pushing %d\n", elements[i]);
        }
        if (!cds_heap_empty(&int_heap)) {
            printf("Top element is now: %d, Size: %zu\n", *(int*)cds_heap_top(&int_heap), cds_heap_size(&int_heap));
        }
    }
    /* Example Output:
    Pushing: 10
    Top element is now: 10, Size: 1
    Pushing: 5
    Top element is now: 5, Size: 2
    Pushing: 20
    Top element is now: 5, Size: 3
    Pushing: 3
    Top element is now: 3, Size: 4
    Pushing: 8
    Top element is now: 3, Size: 5
    */

    printf("\nPopping elements from heap:\n");
    while (!cds_heap_empty(&int_heap)) {
        int *top_val = (int*)cds_heap_top(&int_heap);
        printf("Top: %d, Size before pop: %zu\n", *top_val, cds_heap_size(&int_heap));
        cds_heap_pop(&int_heap);
        printf("Size after pop: %zu\n", cds_heap_size(&int_heap));
    }
    /* Example Output:
    Popping elements from heap:
    Top: 3, Size before pop: 5
    Size after pop: 4
    Top: 5, Size before pop: 4
    Size after pop: 3
    Top: 8, Size before pop: 3
    Size after pop: 2
    Top: 10, Size before pop: 2
    Size after pop: 1
    Top: 20, Size before pop: 1
    Size after pop: 0
    */

    printf("\nIs heap empty now? %s\n", cds_heap_empty(&int_heap) ? "Yes" : "No");
    // Output: Is heap empty now? Yes

    // Clean up
    cds_heap_delete(&int_heap);
    printf("Heap deleted.\n");

    return 0;
}
```
