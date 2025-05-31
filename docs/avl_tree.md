# AVL Tree Data Structure

The AVL Tree data structure in CDS is a self-balancing binary search tree. It maintains a balanced height by performing rotations when an insertion or deletion causes an imbalance. This ensures that operations like search, insert, and delete have a time complexity of O(log n).

## Structs

### `CdsAvlNode`

The `CdsAvlNode` struct defines the structure of a node in the AVL tree:

```c
typedef struct cds_avl_node {
  void *key;
  void *value;
  struct cds_avl_node *left;
  struct cds_avl_node *right;
  struct cds_avl_node *parent;
  int height;
} CdsAvlNode;
```

- `key`: A pointer to the key of the node.
- `value`: A pointer to the value associated with the key.
- `left`: A pointer to the left child of the node.
- `right`: A pointer to the right child of the node.
- `parent`: A pointer to the parent of the node.
- `height`: The height of the subtree rooted at this node.

### `CdsAvlTree`

The `CdsAvlTree` struct defines the structure of the AVL tree itself:

```c
typedef struct cds_avl_tree {
  struct cds_avl_node *root;
  size_t element_size;
  size_t key_size;
  int (*cmp)(const void *key1, const void *key2);
  size_t size;
} CdsAvlTree;
```

- `root`: A pointer to the root node of the AVL tree.
- `element_size`: The size of each value (element) stored in the tree, in bytes.
- `key_size`: The size of each key stored in the tree, in bytes.
- `cmp`: A function pointer to a comparison function that takes two keys and returns:
    - A negative value if key1 < key2.
    - Zero if key1 == key2.
    - A positive value if key1 > key2.
- `size`: The number of nodes in the AVL tree.

## Functions

### `cds_avl_tree_new`

```c
struct cds_avl_tree cds_avl_tree_new(size_t element_size, size_t key_size,
                                     int (*cmp)(const void *, const void *));
```

**Description:** Initializes a new, empty AVL tree.

**Arguments:**
- `element_size`: The size of the values to be stored in the tree.
- `key_size`: The size of the keys to be stored in the tree.
- `cmp`: A pointer to the key comparison function.

**Returns:** An initialized `CdsAvlTree` struct.

**Notes:** The caller provides the memory for the `CdsAvlTree` struct itself. Nodes, keys, and values are allocated on the heap during insertion.

### `cds_avl_tree_delete`

```c
void cds_avl_tree_delete(struct cds_avl_tree *tree);
```

**Description:** Deletes all nodes, keys, and values within the AVL tree and frees their associated memory.

**Arguments:**
- `tree`: A pointer to the `CdsAvlTree` struct whose contents are to be deleted.

**Returns:** None.

**Notes:** This function does not free the `CdsAvlTree` struct itself. After calling, the tree will be empty and can be reused or deallocated by the caller.

### `cds_avl_tree_insert`

```c
int cds_avl_tree_insert(struct cds_avl_tree *tree, const void *key,
                        const void *value);
```

**Description:** Inserts a new key-value pair into the AVL tree. If the key already exists, the insertion typically fails (as per the provided source code, which returns -1 if success_flag is not 1). The tree rebalances itself if necessary.

**Arguments:**
- `tree`: A pointer to the `CdsAvlTree` struct.
- `key`: A pointer to the key to be inserted.
- `value`: A pointer to the value to be inserted.

**Returns:** `0` on successful insertion, non-zero on failure (e.g., memory allocation error, or if the key already exists).

### `cds_avl_tree_remove`

```c
int cds_avl_tree_remove(struct cds_avl_tree *tree, const void *key);
```

**Description:** Removes a key-value pair from the AVL tree based on the provided key. The tree rebalances itself if necessary.

**Arguments:**
- `tree`: A pointer to the `CdsAvlTree` struct.
- `key`: A pointer to the key of the node to be removed.

**Returns:** `0` on successful removal, non-zero if the key is not found or if an error occurs during removal (e.g. memory allocation failure during rebalancing).

### `cds_avl_tree_find`

```c
void *cds_avl_tree_find(const struct cds_avl_tree *tree, const void *key);
```

**Description:** Finds and returns a pointer to the value associated with the given key in the AVL tree.

**Arguments:**
- `tree`: A pointer to the `CdsAvlTree` struct.
- `key`: A pointer to the key to search for.

**Returns:** A pointer to the value if the key is found, otherwise `NULL`.

### `cds_avl_tree_height`

```c
int cds_avl_tree_height(const struct cds_avl_tree *tree);
```

**Description:** Gets the height of the AVL tree. The height of an empty tree is 0. The height of a non-empty tree is the height of its root node.

**Arguments:**
- `tree`: A pointer to the `CdsAvlTree` struct.

**Returns:** The height of the AVL tree.

### `cds_avl_tree_size`

```c
size_t cds_avl_tree_size(const struct cds_avl_tree *tree);
```

**Description:** Gets the number of nodes (key-value pairs) currently in the AVL tree.

**Arguments:**
- `tree`: A pointer to the `CdsAvlTree` struct.

**Returns:** The number of elements in the tree.

### `cds_avl_tree_empty`

```c
bool cds_avl_tree_empty(const struct cds_avl_tree *tree);
```

**Description:** Checks if the AVL tree is empty.

**Arguments:**
- `tree`: A pointer to the `CdsAvlTree` struct.

**Returns:** `true` if the tree is empty (contains no nodes), `false` otherwise.

## Usage Example

```c
#include "cds/avl_tree.h"
#include <stdio.h>
#include <string.h> // For strcmp

// Comparison function for integers
int compare_int_keys(const void *key1, const void *key2) {
    int int1 = *(int*)key1;
    int int2 = *(int*)key2;
    if (int1 < int2) return -1;
    if (int1 > int2) return 1;
    return 0;
}

int main() {
    // Initialize an AVL tree for int keys and int values
    CdsAvlTree intTree = cds_avl_tree_new(sizeof(int), sizeof(int), compare_int_keys);

    printf("Initial tree size: %zu, height: %d, empty: %s\n",
           cds_avl_tree_size(&intTree),
           cds_avl_tree_height(&intTree),
           cds_avl_tree_empty(&intTree) ? "true" : "false");
    // Output: Initial tree size: 0, height: 0, empty: true

    // Insert some key-value pairs
    int keys[] = {10, 20, 5, 15, 25, 3};
    int values[] = {100, 200, 50, 150, 250, 30};
    int num_elements = sizeof(keys) / sizeof(keys[0]);

    for (int i = 0; i < num_elements; ++i) {
        if (cds_avl_tree_insert(&intTree, &keys[i], &values[i]) == 0) {
            printf("Inserted key: %d, value: %d. Tree size: %zu, height: %d\n",
                   keys[i], values[i], cds_avl_tree_size(&intTree), cds_avl_tree_height(&intTree));
        } else {
            fprintf(stderr, "Failed to insert key: %d\n", keys[i]);
        }
    }
    /* Example Output:
    Inserted key: 10, value: 100. Tree size: 1, height: 1
    Inserted key: 20, value: 200. Tree size: 2, height: 2
    Inserted key: 5, value: 50. Tree size: 3, height: 2
    Inserted key: 15, value: 150. Tree size: 4, height: 3
    Inserted key: 25, value: 250. Tree size: 5, height: 3
    Inserted key: 3, value: 30. Tree size: 6, height: 3
    */

    // Find an element
    int key_to_find = 15;
    int *found_value = (int*)cds_avl_tree_find(&intTree, &key_to_find);
    if (found_value != NULL) {
        printf("Found key %d, value: %d\n", key_to_find, *found_value); // Output: Found key 15, value: 150
    } else {
        printf("Key %d not found\n", key_to_find);
    }

    key_to_find = 100; // A key not in the tree
    found_value = (int*)cds_avl_tree_find(&intTree, &key_to_find);
    if (found_value != NULL) {
        printf("Found key %d, value: %d\n", key_to_find, *found_value);
    } else {
        printf("Key %d not found\n", key_to_find); // Output: Key 100 not found
    }

    // Remove an element
    int key_to_remove = 5;
    if (cds_avl_tree_remove(&intTree, &key_to_remove) == 0) {
        printf("Removed key: %d. Tree size: %zu, height: %d\n",
               key_to_remove, cds_avl_tree_size(&intTree), cds_avl_tree_height(&intTree));
        // Example Output: Removed key: 5. Tree size: 5, height: 3
    } else {
        printf("Failed to remove key: %d\n", key_to_remove);
    }

    printf("Final tree size: %zu, height: %d, empty: %s\n",
           cds_avl_tree_size(&intTree),
           cds_avl_tree_height(&intTree),
           cds_avl_tree_empty(&intTree) ? "true" : "false");
    // Example Output: Final tree size: 5, height: 3, empty: false

    // Delete the tree (frees all nodes, keys, values)
    cds_avl_tree_delete(&intTree);
    printf("Tree deleted. Size: %zu, height: %d, empty: %s\n",
           cds_avl_tree_size(&intTree),
           cds_avl_tree_height(&intTree),
           cds_avl_tree_empty(&intTree) ? "true" : "false");
    // Output: Tree deleted. Size: 0, height: 0, empty: true

    return 0;
}
```
