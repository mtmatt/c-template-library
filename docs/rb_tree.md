# Red-Black Tree Data Structure

The Red-Black Tree (RB Tree) in CDS is a self-balancing binary search tree. It maintains balance by adhering to a set of properties that ensure that no path from the root to any leaf is more than twice as long as any other path. This guarantees that operations like search, insert, and delete have a worst-case time complexity of O(log n).

Red-Black Tree Properties:
1. Every node is either red or black.
2. The root is always black.
3. Every leaf (NIL node) is black. (In this implementation, a single sentinel `nil` node is used for all logical leaves).
4. If a node is red, then both its children are black.
5. For each node, all simple paths from the node to descendant leaves contain the same number of black nodes.

## Structs and Enums

### `cds_rb_color` Enum

Defines the possible colors for a Red-Black Tree node:

```c
enum cds_rb_color {
  CDS_RB_RED,
  CDS_RB_BLACK
};
```

### `CdsRbNode`

The `CdsRbNode` struct defines the structure of a node in the Red-Black tree:

```c
typedef struct cds_rb_node {
  void *key;
  void *value;
  struct cds_rb_node *left;
  struct cds_rb_node *right;
  struct cds_rb_node *parent;
  enum cds_rb_color color;
} CdsRbNode;
```

- `key`: A pointer to the key of the node.
- `value`: A pointer to the value associated with the key.
- `left`: A pointer to the left child of the node.
- `right`: A pointer to the right child of the node.
- `parent`: A pointer to the parent of the node.
- `color`: The color of the node (`CDS_RB_RED` or `CDS_RB_BLACK`).

### `CdsRbTree`

The `CdsRbTree` struct defines the structure of the Red-Black tree itself:

```c
typedef struct cds_rb_tree {
  struct cds_rb_node *root;     // Pointer to the root node of the tree
  struct cds_rb_node *nil;      // Pointer to a sentinel NIL node
  size_t element_size;        // Size of each value (element) in bytes
  size_t key_size;            // Size of each key in bytes
  int (*cmp)(const void *key1, const void *key2); // Key comparison function
  size_t size;                // Number of nodes in the tree
} CdsRbTree;
```

- `root`: A pointer to the root node. Initially points to the `nil` node for an empty tree.
- `nil`: A special sentinel node. All leaves (external nodes) and the parent of the root point to this `nil` node. The `nil` node is always black. This simplifies the implementation of RB Tree algorithms.
- `element_size`: The size of values stored in the tree.
- `key_size`: The size of keys stored in the tree.
- `cmp`: A function pointer for comparing two keys. It should return:
    - A negative value if key1 < key2.
    - Zero if key1 == key2.
    - A positive value if key1 > key2.
- `size`: The current number of internal nodes in the tree.

## Functions

### `cds_rb_tree_new`

```c
struct cds_rb_tree cds_rb_tree_new(size_t element_size, size_t key_size,
                                   int (*cmp)(const void *, const void *));
```

**Description:** Initializes a new, empty Red-Black tree. This includes creating and configuring the sentinel `nil` node.

**Arguments:**
- `element_size`: The size of the values to be stored.
- `key_size`: The size of the keys to be stored.
- `cmp`: A pointer to the key comparison function.

**Returns:** An initialized `CdsRbTree` struct. If the sentinel `nil` node allocation fails, the `cmp` field will be `NULL`, `root` will be `NULL`, and `size` will be 0, indicating an invalid tree state.

**Notes:** The caller provides memory for the `CdsRbTree` struct itself. Nodes, keys, and values are allocated on the heap during insertion.

### `cds_rb_tree_delete`

```c
void cds_rb_tree_delete(struct cds_rb_tree *tree);
```

**Description:** Deletes all internal nodes (keys, values) within the Red-Black tree and frees their associated memory. It also frees the sentinel `nil` node.

**Arguments:**
- `tree`: A pointer to the `CdsRbTree` struct whose contents are to be deleted.

**Returns:** None.

**Notes:** This function does not free the `CdsRbTree` struct itself. After calling, the tree structure's pointers (`root`, `nil`) are set to `NULL` and `size` to 0.

### `cds_rb_tree_insert`

```c
int cds_rb_tree_insert(struct cds_rb_tree *tree, const void *key,
                       const void *value);
```

**Description:** Inserts a new key-value pair into the Red-Black tree. After insertion, it performs fixup operations (rotations and color changes) to maintain Red-Black tree properties.

**Arguments:**
- `tree`: A pointer to the `CdsRbTree` struct.
- `key`: A pointer to the key to be inserted.
- `value`: A pointer to the value to be inserted.

**Returns:** `0` on successful insertion. Returns `-1` on failure (e.g., memory allocation error for the new node, or if the key already exists in the tree).

### `cds_rb_tree_remove`

```c
int cds_rb_tree_remove(struct cds_rb_tree *tree, const void *key);
```

**Description:** Removes a key-value pair from the Red-Black tree based on the provided key. After removal, it performs fixup operations to maintain Red-Black tree properties.

**Arguments:**
- `tree`: A pointer to the `CdsRbTree` struct.
- `key`: A pointer to the key of the node to be removed.

**Returns:** `0` on successful removal. Returns `-1` if the key is not found or if `tree` or `key` is `NULL`.

### `cds_rb_tree_find`

```c
void *cds_rb_tree_find(const struct cds_rb_tree *tree, const void *key);
```

**Description:** Finds and returns a pointer to the value associated with the given key in the Red-Black tree.

**Arguments:**
- `tree`: A pointer to the `CdsRbTree` struct.
- `key`: A pointer to the key to search for.

**Returns:** A pointer to the value if the key is found, otherwise `NULL`.

### `cds_rb_tree_size`

```c
size_t cds_rb_tree_size(const struct cds_rb_tree *tree);
```

**Description:** Gets the number of internal nodes currently in the Red-Black tree.

**Arguments:**
- `tree`: A pointer to the `CdsRbTree` struct.

**Returns:** The number of elements in the tree. Returns `0` if `tree` is `NULL`.

### `cds_rb_tree_empty`

```c
bool cds_rb_tree_empty(const struct cds_rb_tree *tree);
```

**Description:** Checks if the Red-Black tree is empty (contains no internal nodes).

**Arguments:**
- `tree`: A pointer to the `CdsRbTree` struct.

**Returns:** `true` if the tree is empty or if `tree` is `NULL`, `false` otherwise.

## Usage Example

```c
#include "cds/rb_tree.h"
#include <stdio.h>
#include <string.h> // For strcmp (if using string keys)

// Comparison function for integer keys
int compare_int_keys_rb(const void *key1, const void *key2) {
    int int1 = *(const int*)key1;
    int int2 = *(const int*)key2;
    if (int1 < int2) return -1;
    if (int1 > int2) return 1;
    return 0;
}

int main() {
    // Initialize an RB tree for int keys and int values
    CdsRbTree int_rb_tree = cds_rb_tree_new(sizeof(int), sizeof(int), compare_int_keys_rb);

    if (int_rb_tree.nil == NULL) { // Check if nil node allocation failed
        fprintf(stderr, "Failed to initialize RB Tree (nil node allocation failed).\n");
        return 1;
    }

    printf("Initial tree size: %zu, Is empty: %s\n",
           cds_rb_tree_size(&int_rb_tree),
           cds_rb_tree_empty(&int_rb_tree) ? "true" : "false");
    // Output: Initial tree size: 0, Is empty: true

    // Insert some key-value pairs
    int keys[] = {10, 20, 5, 15, 25, 3, 30, 1};
    int values[] = {100, 200, 50, 150, 250, 30, 300, 10};
    int num_elements = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < num_elements; ++i) {
        if (cds_rb_tree_insert(&int_rb_tree, &keys[i], &values[i]) == 0) {
            printf("Inserted key: %d, value: %d. Tree size: %zu\n",
                   keys[i], values[i], cds_rb_tree_size(&int_rb_tree));
        } else {
            fprintf(stderr, "Failed to insert key: %d (or key already exists)\n", keys[i]);
        }
    }
    /* Example output (order of messages might vary due to rebalancing):
    Inserted key: 10, value: 100. Tree size: 1
    Inserted key: 20, value: 200. Tree size: 2
    Inserted key: 5, value: 50. Tree size: 3
    Inserted key: 15, value: 150. Tree size: 4
    Inserted key: 25, value: 250. Tree size: 5
    Inserted key: 3, value: 30. Tree size: 6
    Inserted key: 30, value: 300. Tree size: 7
    Inserted key: 1, value: 10. Tree size: 8
    */

    // Find an element
    int key_to_find = 15;
    int *found_value_rb = (int*)cds_rb_tree_find(&int_rb_tree, &key_to_find);
    if (found_value_rb != NULL) {
        printf("Found key %d, value: %d\n", key_to_find, *found_value_rb); // Output: Found key 15, value: 150
    } else {
        printf("Key %d not found\n", key_to_find);
    }

    // Remove an element
    int key_to_remove = 5;
    if (cds_rb_tree_remove(&int_rb_tree, &key_to_remove) == 0) {
        printf("Removed key: %d. Tree size: %zu\n",
               key_to_remove, cds_rb_tree_size(&int_rb_tree)); // Example Output: Removed key: 5. Tree size: 7
    } else {
        printf("Failed to remove key: %d (or key not found)\n", key_to_remove);
    }

    key_to_remove = 99; // Key not in tree
    if (cds_rb_tree_remove(&int_rb_tree, &key_to_remove) == 0) {
        printf("Removed key: %d. Tree size: %zu\n", key_to_remove, cds_rb_tree_size(&int_rb_tree));
    } else {
        printf("Failed to remove key: %d (or key not found)\n", key_to_remove); // Output: Failed to remove key: 99 (or key not found)
    }

    printf("Final tree size: %zu, Is empty: %s\n",
           cds_rb_tree_size(&int_rb_tree),
           cds_rb_tree_empty(&int_rb_tree) ? "true" : "false");
    // Example Output: Final tree size: 7, Is empty: false

    // Delete the tree (frees all nodes, keys, values, and the nil node)
    cds_rb_tree_delete(&int_rb_tree);
    printf("Tree deleted. Size: %zu, Is empty: %s\n",
           cds_rb_tree_size(&int_rb_tree), // Should be 0
           cds_rb_tree_empty(&int_rb_tree)); // Should be true
    // Output: Tree deleted. Size: 0, Is empty: true

    return 0;
}
```
