# Array Data Structure

The Array data structure in CDS is a dynamic array that can grow or shrink in size as needed. It stores elements of a fixed size, specified at the time of creation.

## `CdsArray` Struct

The `CdsArray` struct defines the structure of the dynamic array:

```c
typedef struct cds_array {
  char *data;
  size_t size, capacity, element_size;
} CdsArray;
```

- `data`: A pointer to the raw data stored in the array.
- `size`: The current number of elements in the array.
- `capacity`: The total number of elements the array can hold before needing to resize.
- `element_size`: The size of each element in the array, in bytes.

## Functions

### `cds_array_new`

```c
struct cds_array cds_array_new(size_t element_size);
```

**Description:** Creates a new dynamic array.

**Arguments:**
- `element_size`: The size of each element to be stored in the array.

**Returns:** A `CdsArray` struct. The `data` field might be `NULL` if memory allocation fails.

**Notes:** The caller is responsible for freeing the memory allocated for the array using `cds_array_delete`.

### `cds_array_delete`

```c
void cds_array_delete(struct cds_array *array);
```

**Description:** Frees the memory allocated for the dynamic array.

**Arguments:**
- `array`: A pointer to the `CdsArray` struct to be deleted.

**Returns:** None.

**Notes:** Ensure the `array` pointer is not `NULL` before calling this function.

### `cds_array_push_back`

```c
int cds_array_push_back(struct cds_array *array, const void *new_element);
```

**Description:** Adds a new element to the end of the dynamic array.

**Arguments:**
- `array`: A pointer to the `CdsArray` struct.
- `new_element`: A pointer to the element to be added.

**Returns:** `0` on success, `-1` on failure (e.g., memory allocation failure).

**Notes:** The array automatically resizes if its capacity is exceeded.

### `cds_array_pop_back`

```c
int cds_array_pop_back(struct cds_array *array);
```

**Description:** Removes the last element from the dynamic array.

**Arguments:**
- `array`: A pointer to the `CdsArray` struct.

**Returns:** `0` on success, `-1` on failure (e.g., if the array is empty).

**Notes:** The size of the array is decreased by one.

### `cds_array_at`

```c
void* cds_array_at(const struct cds_array *array, size_t index);
```

**Description:** Returns a pointer to the element at the specified `index` in the dynamic array. This function performs bounds checking.

**Arguments:**
- `array`: A pointer to the `CdsArray` struct.
- `index`: The index of the element to access.

**Returns:** A pointer to the element at the specified `index`, or `NULL` if the `index` is out of bounds.

### `cds_array_get`

```c
void *cds_array_get(const struct cds_array *array, size_t index);
```

**Description:** Returns a pointer to the element at the specified `index` in the dynamic array. This function does **not** perform bounds checking.

**Arguments:**
- `array`: A pointer to the `CdsArray` struct.
- `index`: The index of the element to access.

**Returns:** A pointer to the element at the specified `index`.

**Notes:** The caller must ensure that the `index` is within the bounds of the array to avoid undefined behavior.

### `cds_array_size`

```c
size_t cds_array_size(const struct cds_array *array);
```

**Description:** Returns the number of elements currently in the dynamic array.

**Arguments:**
- `array`: A pointer to the `CdsArray` struct.

**Returns:** The number of elements in the array.

**Notes:** Ensure the `array` pointer is not `NULL`.

### `cds_array_empty`

```c
bool cds_array_empty(const struct cds_array *array);
```

**Description:** Checks if the dynamic array is empty.

**Arguments:**
- `array`: A pointer to the `CdsArray` struct.

**Returns:** `true` if the array is empty, `false` otherwise.

**Notes:** Ensure the `array` pointer is not `NULL`.

## Usage Example

```c
#include "cds/array.h"
#include <stdio.h>

int main() {
    // Create an array to store integers
    CdsArray intArray = cds_array_new(sizeof(int));

    if (intArray.data == NULL) {
        fprintf(stderr, "Failed to create array\n");
        return 1;
    }

    // Add elements to the array
    for (int i = 0; i < 5; ++i) {
        cds_array_push_back(&intArray, &i);
    }

    // Print array elements
    printf("Array elements: ");
    for (size_t i = 0; i < cds_array_size(&intArray); ++i) {
        int *element = (int*)cds_array_at(&intArray, i);
        if (element != NULL) {
            printf("%d ", *element);
        }
    }
    printf("\n"); // Output: Array elements: 0 1 2 3 4

    // Remove the last element
    cds_array_pop_back(&intArray);

    printf("Array size after pop: %zu\n", cds_array_size(&intArray)); // Output: Array size after pop: 4

    // Check if array is empty
    if (cds_array_empty(&intArray)) {
        printf("Array is empty\n");
    } else {
        printf("Array is not empty\n"); // Output: Array is not empty
    }

    // Clean up
    cds_array_delete(&intArray);

    return 0;
}
```
