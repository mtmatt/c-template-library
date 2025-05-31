# String Data Structure (`CdsString`)

The `CdsString` in CDS is a dynamic string implementation that supports mutable operations. A key feature is its **Small String Optimization (SSO)**. For short strings (by default, up to 7 characters plus a null terminator), the string data is stored directly within the `CdsString` struct itself, avoiding heap allocations. For longer strings, memory is allocated on the heap.

## `CdsString` Struct

```c
#define CDS_STRING_OPT_CAPACITY 8 // Default optimization capacity

typedef struct cds_string {
  char opt_data[CDS_STRING_OPT_CAPACITY]; // Buffer for small string optimization
  char *data;                             // Pointer to heap-allocated data for longer strings
  size_t size;                            // Current length of the string (excluding null terminator)
  size_t capacity;                        // Current allocated capacity
} CdsString;
```

- `opt_data`: An array embedded in the struct to hold small strings. If the string's capacity is `CDS_STRING_OPT_CAPACITY`, the string data (including the null terminator) is stored here.
- `data`: A pointer to a dynamically allocated character array on the heap. This is used if the string's required capacity exceeds `CDS_STRING_OPT_CAPACITY`. If `data` is not `NULL`, it holds the string content.
- `size`: The current length of the string, not including the null terminator.
- `capacity`: The total storage capacity currently allocated for the string (including space for the null terminator). If `capacity == CDS_STRING_OPT_CAPACITY`, `opt_data` is used. Otherwise, `data` points to a heap block of this size.

## Functions

### `cds_string_new`

```c
struct cds_string cds_string_new();
```

**Description:** Creates a new, empty `CdsString`.
**Returns:** A `CdsString` instance, initialized for SSO (empty string stored in `opt_data`). `data` is `NULL`, `size` is 0, and `capacity` is `CDS_STRING_OPT_CAPACITY`.

### `cds_string_from`

```c
struct cds_string cds_string_from(const char *data, size_t length);
```

**Description:** Creates a new `CdsString` by copying data from a C-style string (char array).
**Arguments:**
- `data`: A pointer to the C-style string (character array).
- `length`: The number of characters to copy from `data`.
**Returns:** A `CdsString` instance. If `length + 1 <= CDS_STRING_OPT_CAPACITY`, the string is stored in `opt_data`. Otherwise, memory is allocated on the heap. The new string is null-terminated.

### `cds_string_move`

```c
void cds_string_move(struct cds_string *first, struct cds_string *second);
```

**Description:** Moves the content and ownership of `second` string to `first`.
**Arguments:**
- `first`: Pointer to the destination `CdsString`. Any existing heap-allocated data in `first` is freed.
- `second`: Pointer to the source `CdsString`. After the move, `second`'s `data` pointer is transferred to `first`. The content of `opt_data`, `size`, and `capacity` are also copied. `second` is not cleared but its `data` pointer should be considered invalid if it was heap-allocated.
**Notes:** The documentation comment says "The first string will be deleted after the function call," which seems incorrect based on the implementation. It should state that the resources *previously held* by `first` are freed if they were on the heap. `second`'s heap data is taken over, not its struct.

### `cds_string_delete`

```c
void cds_string_delete(struct cds_string *string);
```

**Description:** Frees the heap-allocated memory if the `CdsString` uses dynamic storage (`string->data != NULL`). Resets the string to an empty state.
**Arguments:**
- `string`: A pointer to the `CdsString` to be deleted.
**Notes:** If `string->data` is `NULL` (meaning it was using `opt_data`), only the struct fields are zeroed out.

### `cds_string_get`

```c
char* cds_string_get(const struct cds_string *string);
```

**Description:** Returns a pointer to the null-terminated character data of the string.
**Arguments:**
- `string`: A pointer to the `CdsString`.
**Returns:** A `char*` pointing to the beginning of the string data. This will be `string->opt_data` if SSO is active, or `string->data` if heap allocation is used.

### `cds_string_size`

```c
size_t cds_string_size(const struct cds_string *string);
```

**Description:** Returns the current length of the string (number of characters, excluding the null terminator).
**Arguments:**
- `string`: A pointer to the `CdsString`.
**Returns:** The length of the string.

### `cds_string_empty`

```c
bool cds_string_empty(const struct cds_string *string);
```

**Description:** Checks if the string is empty.
**Arguments:**
- `string`: A pointer to the `CdsString`.
**Returns:** `true` if the string length is 0, `false` otherwise.

### `cds_string_split`

```c
struct cds_array cds_string_split(const struct cds_string *string, char delimiters[]);
```

**Description:** Splits the input `string` into an array of `CdsString`s based on the provided `delimiters`.
**Arguments:**
- `string`: A pointer to the `CdsString` to be split.
- `delimiters`: A C-style string containing characters to be used as delimiters.
**Returns:** A `CdsArray` where each element is a `CdsString`. These new strings are created using `cds_string_from` and thus manage their own memory (potentially SSO or heap).
**Notes:** The original string is not modified. The caller is responsible for deleting the returned `CdsArray` and also for calling `cds_string_delete` on each `CdsString` element within that array once they are no longer needed.

### `cds_string_concat`

```c
int cds_string_concat(struct cds_string *first, const struct cds_string *second);
```

**Description:** Appends a copy of the `second` string to the end of the `first` string.
**Arguments:**
- `first`: A pointer to the `CdsString` to which `second` will be appended. This string may be reallocated.
- `second`: A pointer to the `CdsString` to append. This string is not modified.
**Returns:** `0` on success, `-1` on failure (e.g., memory allocation failure).
**Notes:** Handles transitions from `opt_data` to heap allocation if `first` was using SSO and the combined length exceeds `CDS_STRING_OPT_CAPACITY`.

### `cds_string_push`

```c
int cds_string_push(struct cds_string *string, char chr);
```

**Description:** Appends a single character `chr` to the end of the string.
**Arguments:**
- `string`: A pointer to the `CdsString`. May be reallocated.
- `chr`: The character to append.
**Returns:** `0` on success, `-1` on failure (e.g., memory allocation failure).
**Notes:** The implementation in `src/string.c` for `cds_string_push` has a bug. It always returns -1. It should return 0 on success. Also, if it transitions from `opt_data` to `data`, it correctly allocates and moves, but if it's already using `data` and needs to reallocate, the `memcpy` from `opt_data` is incorrect; it should copy from the existing `string->data`. This documentation will assume the intended correct behavior.

### `cds_string_pop`

```c
int cds_string_pop(struct cds_string *string);
```

**Description:** Removes the last character from the string.
**Arguments:**
- `string`: A pointer to the `CdsString`.
**Returns:** `0` on success, `-1` if the string is empty.
**Notes:** The capacity is not shrunk. The removed character is replaced with a null terminator.

### `cds_string_pop_length`

```c
int cds_string_pop_length(struct cds_string *string, size_t length);
```

**Description:** Removes `length` characters from the end of the string.
**Arguments:**
- `string`: A pointer to the `CdsString`.
- `length`: The number of characters to remove from the end.
**Returns:** `0` on success, `-1` if `string->size < length`.
**Notes:** The new end of the string is null-terminated. Capacity is not shrunk.

## Usage Example

```c
#include "cds/string.h"
#include "cds/array.h" // For handling split results
#include <stdio.h>

int main() {
    // Create an empty string (uses SSO)
    CdsString s1 = cds_string_new();
    printf("s1: '%s', size: %zu, capacity: %zu (SSO initially)\n",
           cds_string_get(&s1), cds_string_size(&s1), s1.capacity);

    // Create from C-string (still SSO)
    CdsString s2 = cds_string_from("Hello", 5);
    printf("s2: '%s', size: %zu, capacity: %zu (SSO)\n",
           cds_string_get(&s2), cds_string_size(&s2), s2.capacity);

    // Concatenate s2 to s1
    if (cds_string_concat(&s1, &s2) == 0) {
        printf("s1 after concat s2: '%s', size: %zu, capacity: %zu\n",
               cds_string_get(&s1), cds_string_size(&s1), s1.capacity);
    }

    // Push characters to s1 until it likely moves to heap
    printf("Pushing chars to s1...\n");
    for (char c = 'A'; c <= 'G'; ++c) { // "Hello" (5) + 7 chars = 12 chars. Default SSO is 8.
        cds_string_push(&s1, c); // Assumes corrected push that returns 0 on success
    }
    printf("s1 after pushes: '%s', size: %zu, capacity: %zu (likely heap)\n",
           cds_string_get(&s1), cds_string_size(&s1), s1.capacity);

    // Pop a character
    cds_string_pop(&s1);
    printf("s1 after pop: '%s', size: %zu\n", cds_string_get(&s1), cds_string_size(&s1));

    // Pop multiple characters
    cds_string_pop_length(&s1, 3);
    printf("s1 after pop_length(3): '%s', size: %zu\n", cds_string_get(&s1), cds_string_size(&s1));

    // Split example
    CdsString to_split = cds_string_from("apple,banana,orange", 19);
    printf("\nSplitting string: '%s' by ','\n", cds_string_get(&to_split));
    CdsArray parts = cds_string_split(&to_split, ",");

    for (size_t i = 0; i < cds_array_size(&parts); ++i) {
        CdsString *part_str = (CdsString*)cds_array_at(&parts, i);
        printf("Part %zu: '%s'\n", i, cds_string_get(part_str));
        cds_string_delete(part_str); // Important: delete each string from split
    }
    cds_array_delete(&parts); // Delete the array itself
    cds_string_delete(&to_split);


    // Clean up s1 and s2
    cds_string_delete(&s1);
    cds_string_delete(&s2);

    printf("\nStrings deleted.\n");

    return 0;
}
```
