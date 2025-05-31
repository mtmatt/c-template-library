# List Data Structure

The List data structure in CDS is a doubly linked list. This means that each element (node) in the list contains a pointer to both the next and the previous node in the sequence. This allows for efficient traversal in both directions and efficient insertion and deletion operations at arbitrary points in the list, provided a pointer to the node is available.

## Structs

### `CdsListNode`

The `CdsListNode` struct defines the structure of a single node in the doubly linked list:

```c
typedef struct cds_list_node {
  void *data;
  struct cds_list_node *next, *prev;
} CdsListNode;
```

- `data`: A pointer to the actual data stored in the node. The data is copied into memory managed by the node.
- `next`: A pointer to the next node in the list. It is `NULL` if this is the last node.
- `prev`: A pointer to the previous node in the list. It is `NULL` if this is the first node.

### `CdsList`

The `CdsList` struct defines the overall list:

```c
typedef struct cds_list {
  struct cds_list_node *head, *tail;
  size_t size, element_size;
} CdsList;
```

- `head`: A pointer to the first node in the list. It is `NULL` if the list is empty.
- `tail`: A pointer to the last node in the list. It is `NULL` if the list is empty.
- `size`: The number of nodes currently in the list.
- `element_size`: The size (in bytes) of each element's data stored in the list. This is fixed when the list is created.

## Node Functions

These functions operate on individual `CdsListNode` instances.

### `cds_list_node_new`

```c
struct cds_list_node* cds_list_node_new(void *data, const size_t element_size);
```

**Description:** Creates a new list node and copies the provided data into it.

**Arguments:**
- `data`: A pointer to the data to be stored in the new node.
- `element_size`: The size of the data to be copied.

**Returns:** A pointer to the newly created `CdsListNode`. Returns `NULL` if memory allocation for the node or its data fails.

**Notes:** The caller is responsible for freeing the node using `cds_list_node_delete` if it's not subsequently managed by a `CdsList`.

### `cds_list_node_delete`

```c
void cds_list_node_delete(struct cds_list_node *node);
```

**Description:** Deletes a list node and frees the memory allocated for its data and the node itself.

**Arguments:**
- `node`: A pointer to the `CdsListNode` to be deleted.

**Returns:** None.

### `cds_list_node_set_data`

```c
void cds_list_node_set_data(struct cds_list_node *node, void *data, const size_t element_size);
```

**Description:** Updates the data stored in an existing list node by copying new data into it.

**Arguments:**
- `node`: A pointer to the list node whose data is to be set.
- `data`: A pointer to the new data to be assigned.
- `element_size`: The size of the new data. This should match the original `element_size` the node was created with to avoid issues.

**Returns:** None.

### `cds_list_node_set_next`

```c
void cds_list_node_set_next(struct cds_list_node *node, struct cds_list_node *next);
```

**Description:** Sets the `next` pointer of a given list node.

**Arguments:**
- `node`: A pointer to the list node.
- `next`: A pointer to the list node that should become the `next` node.

**Returns:** None.

### `cds_list_node_set_prev`

```c
void cds_list_node_set_prev(struct cds_list_node *node, struct cds_list_node *prev);
```

**Description:** Sets the `prev` pointer of a given list node.

**Arguments:**
- `node`: A pointer to the list node.
- `prev`: A pointer to the list node that should become the `prev` node.

**Returns:** None.

## List Functions

These functions operate on the `CdsList` structure.

### `cds_list_new`

```c
struct cds_list cds_list_new(const size_t element_size);
```

**Description:** Creates a new, empty doubly linked list.

**Arguments:**
- `element_size`: The size (in bytes) of data that each element in the list will hold.

**Returns:** An initialized `CdsList` struct.

**Notes:** The caller is responsible for deleting the list using `cds_list_delete` to free all its nodes.

### `cds_list_delete`

```c
void cds_list_delete(struct cds_list *list);
```

**Description:** Deletes all nodes in the list and frees their associated memory. Resets the list structure to an empty state.

**Arguments:**
- `list`: A pointer to the `CdsList` to be deleted.

**Returns:** None.

### `cds_list_push_front`

```c
int cds_list_push_front(struct cds_list *list, void *new_element);
```

**Description:** Inserts a new element at the beginning (head) of the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.
- `new_element`: A pointer to the data for the new element. The data is copied.

**Returns:** `0` on success, `-1` on failure (e.g., memory allocation error for the new node).

### `cds_list_push_back`

```c
int cds_list_push_back(struct cds_list *list, void *new_element);
```

**Description:** Inserts a new element at the end (tail) of the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.
- `new_element`: A pointer to the data for the new element. The data is copied.

**Returns:** `0` on success, `-1` on failure (e.g., memory allocation error for the new node).

### `cds_list_pop_front`

```c
int cds_list_pop_front(struct cds_list *list);
```

**Description:** Removes the element from the beginning (head) of the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.

**Returns:** `0` on success, `-1` if the list is empty.

### `cds_list_pop_back`

```c
int cds_list_pop_back(struct cds_list *list);
```

**Description:** Removes the element from the end (tail) of the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.

**Returns:** `0` on success, `-1` if the list is empty.

### `cds_list_insert`

```c
int cds_list_insert(struct cds_list *list, struct cds_list_node *position, void *new_element);
```

**Description:** Inserts a new element *after* the specified `position` node in the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.
- `position`: A pointer to an existing `CdsListNode` in the list after which the new element will be inserted.
- `new_element`: A pointer to the data for the new element. The data is copied.

**Returns:** `0` on success, `-1` on failure (e.g., memory allocation error). If `position` is the tail, the new node becomes the new tail.

### `cds_list_remove`

```c
int cds_list_remove(struct cds_list *list, struct cds_list_node *position);
```

**Description:** Removes the node at the specified `position` from the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.
- `position`: A pointer to the `CdsListNode` to be removed.

**Returns:** `0` on success, `-1` if `position` is `NULL`.

### `cds_list_concat`

```c
int cds_list_concat(struct cds_list *first, struct cds_list *second);
```

**Description:** Concatenates the `second` list to the end of the `first` list. The nodes from `second` are moved to `first`.

**Arguments:**
- `first`: A pointer to the destination `CdsList`.
- `second`: A pointer to the source `CdsList`. This list will be empty after the operation, and its nodes become part of `first`.

**Returns:** `0` on success, `-1` if either list pointer is `NULL`.

**Notes:** The `second` list is effectively deleted (its structure reset, nodes moved) by this operation. `element_size` of both lists should ideally be the same for data consistency, though the function itself doesn't enforce this.

### `cds_list_get_head`

```c
void* cds_list_get_head(struct cds_list *list);
```

**Description:** Retrieves a pointer to the data of the first element (head) in the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.

**Returns:** A pointer to the data at the head, or `NULL` if the list is empty.

### `cds_list_get_tail`

```c
void* cds_list_get_tail(struct cds_list *list);
```

**Description:** Retrieves a pointer to the data of the last element (tail) in the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.

**Returns:** A pointer to the data at the tail, or `NULL` if the list is empty.

### `cds_list_search`

```c
struct cds_list_node* cds_list_search(struct cds_list *list, void *data, int (*cmp)(const void *, const void *));
```

**Description:** Searches the list for an element matching the provided data using a custom comparison function.

**Arguments:**
- `list`: A pointer to the `CdsList`.
- `data`: A pointer to the data to search for.
- `cmp`: A comparison function. It should take two pointers to elements and return `0` if they are considered equal, and a non-zero value otherwise.

**Returns:** A pointer to the first `CdsListNode` whose data matches, or `NULL` if not found, or if `list`, `data`, or `cmp` is `NULL`.

### `cds_list_size`

```c
size_t cds_list_size(struct cds_list *list);
```

**Description:** Returns the number of elements in the list.

**Arguments:**
- `list`: A pointer to the `CdsList`.

**Returns:** The number of elements in the list.

### `cds_list_empty`

```c
bool cds_list_empty(struct cds_list *list);
```

**Description:** Checks if the list is empty.

**Arguments:**
- `list`: A pointer to the `CdsList`.

**Returns:** `true` if the list is empty, `false` otherwise.

## Usage Example

```c
#include "cds/list.h"
#include <stdio.h>
#include <string.h> // For strcmp

// Example comparison function for integers
int compare_ints(const void *a, const void *b) {
    int int_a = *(const int*)a;
    int int_b = *(const int*)b;
    if (int_a == int_b) return 0;
    return 1; // Non-zero if not equal
}

int main() {
    // Create a list to store integers
    CdsList int_list = cds_list_new(sizeof(int));

    printf("List created. Size: %zu, Empty: %s\n",
           cds_list_size(&int_list), cds_list_empty(&int_list) ? "Yes" : "No");

    // Push elements
    int val1 = 10, val2 = 20, val3 = 30;
    cds_list_push_back(&int_list, &val1);
    cds_list_push_front(&int_list, &val2); // List: 20, 10
    cds_list_push_back(&int_list, &val3);  // List: 20, 10, 30

    printf("After pushes. Size: %zu\n", cds_list_size(&int_list));
    printf("Head data: %d, Tail data: %d\n",
           *(int*)cds_list_get_head(&int_list), *(int*)cds_list_get_tail(&int_list));

    // Iterate and print (forward)
    printf("List elements (forward): ");
    CdsListNode *current = int_list.head;
    while (current != NULL) {
        printf("%d ", *(int*)current->data);
        current = current->next;
    }
    printf("\n");

    // Search for an element
    int search_val = 10;
    CdsListNode *found_node = cds_list_search(&int_list, &search_val, compare_ints);
    if (found_node) {
        printf("Found element: %d\n", *(int*)found_node->data);
        // Insert after found_node
        int val4 = 15;
        cds_list_insert(&int_list, found_node, &val4); // List: 20, 10, 15, 30
        printf("Inserted 15 after 10. Size: %zu\n", cds_list_size(&int_list));
    }

    // Pop elements
    cds_list_pop_front(&int_list); // Removes 20. List: 10, 15, 30
    cds_list_pop_back(&int_list);  // Removes 30. List: 10, 15
    printf("After pops. Size: %zu. Head: %d, Tail: %d\n",
        cds_list_size(&int_list),
        *(int*)cds_list_get_head(&int_list),
        *(int*)cds_list_get_tail(&int_list)
    );

    // Clean up
    cds_list_delete(&int_list);
    printf("List deleted. Size: %zu, Empty: %s\n",
           cds_list_size(&int_list), cds_list_empty(&int_list) ? "Yes" : "No");

    return 0;
}
```
