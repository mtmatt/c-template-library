#include <stdlib.h>
#include <string.h>
#include <stdio.h> // For debugging, remove later if not needed

#include "avl_tree.h" // Should be "cds/avl_tree.h" if build system handles include paths
// Or "../include/cds/avl_tree.h" if compiling from src directly without include path setup.
// Assuming "cds/avl_tree.h" is correct for the build environment.

// --- Helper Functions ---

/**
 * @brief Returns the height of a node.
 * @param node The node.
 * @return Height of the node, 0 if NULL.
 */
static int height(struct cds_avl_node *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

/**
 * @brief Returns the maximum of two integers.
 */
static int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * @brief Updates the height of a node based on its children's heights.
 * @param node The node to update.
 */
static void update_height(struct cds_avl_node *node) {
    if (node != NULL) {
        node->height = 1 + max(height(node->left), height(node->right));
    }
}

/**
 * @brief Calculates the balance factor of a node.
 * @param node The node.
 * @return Balance factor (height of left child - height of right child).
 */
static int balance_factor(struct cds_avl_node *node) {
    if (node == NULL) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

/**
 * @brief Allocates and initializes a new AVL node.
 *
 * Copies the key and value into the node.
 * Height is initialized to 1 (for a new leaf).
 *
 * @param key Pointer to the key.
 * @param value Pointer to the value.
 * @param key_size Size of the key.
 * @param element_size Size of the value/element.
 * @param parent Pointer to the parent node.
 * @return Pointer to the new node, or NULL on memory allocation failure.
 */
static struct cds_avl_node* new_node(const void *key, const void *value, size_t key_size, size_t element_size, struct cds_avl_node *parent) {
    struct cds_avl_node *node = (struct cds_avl_node *)malloc(sizeof(struct cds_avl_node));
    if (node == NULL) {
        return NULL;
    }

    node->key = malloc(key_size);
    if (node->key == NULL) {
        free(node);
        return NULL;
    }
    memcpy(node->key, key, key_size);

    node->value = malloc(element_size);
    if (node->value == NULL) {
        free(node->key);
        free(node);
        return NULL;
    }
    memcpy(node->value, value, element_size);

    node->left = NULL;
    node->right = NULL;
    node->parent = parent;
    node->height = 1; // New node is initially added at leaf

    return node;
}

/**
 * @brief Frees an AVL node and its copied key and value.
 * @param node The node to free.
 */
static void free_node(struct cds_avl_node *node) {
    if (node != NULL) {
        free(node->key);
        free(node->value);
        free(node);
    }
}

// --- Rotation Functions ---

/**
 * @brief Performs a left rotation on the subtree rooted with x.
 * Updates parent pointers.
 * @param x The node to rotate.
 * @return The new root of the subtree.
 */
static struct cds_avl_node* rotate_left(struct cds_avl_node *x) {
    struct cds_avl_node *y = x->right;
    struct cds_avl_node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update parent pointers
    y->parent = x->parent;
    x->parent = y;
    if (T2 != NULL) {
        T2->parent = x;
    }

    // Update heights
    update_height(x);
    update_height(y);

    return y; // New root
}

/**
 * @brief Performs a right rotation on the subtree rooted with y.
 * Updates parent pointers.
 * @param y The node to rotate.
 * @return The new root of the subtree.
 */
static struct cds_avl_node* rotate_right(struct cds_avl_node *y) {
    struct cds_avl_node *x = y->left;
    struct cds_avl_node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update parent pointers
    x->parent = y->parent;
    y->parent = x;
    if (T2 != NULL) {
        T2->parent = y;
    }

    // Update heights
    update_height(y);
    update_height(x);

    return x; // New root
}

// --- Balancing Function ---

/**
 * @brief Balances a node and performs rotations if necessary.
 * @param node The node to balance.
 * @param key_inserted_or_removed The key that was just inserted or removed (used for LR/RL tie-breaking if needed, though not strictly required for standard AVL).
 * @param cmp Comparison function.
 * @return The new root of the balanced subtree.
 */
static struct cds_avl_node* balance(struct cds_avl_node *node) {
    if (node == NULL) return node;

    update_height(node);
    int bf = balance_factor(node);

    // Left Heavy
    if (bf > 1) {
        // Left-Right Case
        if (balance_factor(node->left) < 0) {
            node->left = rotate_left(node->left);
            // Update parent of the new node->left if it changed
            if(node->left) node->left->parent = node;
        }
        // Left-Left Case (or after LR correction)
        return rotate_right(node);
    }
    // Right Heavy
    if (bf < -1) {
        // Right-Left Case
        if (balance_factor(node->right) > 0) {
            node->right = rotate_right(node->right);
            // Update parent of the new node->right if it changed
            if(node->right) node->right->parent = node;
        }
        // Right-Right Case (or after RL correction)
        return rotate_left(node);
    }

    return node; // Already balanced
}


// --- Public API Functions ---

/**
 * @brief Initializes a new AVL tree.
 */
struct cds_avl_tree cds_avl_tree_new(size_t element_size, size_t key_size, int (*cmp)(const void *, const void *)) {
    struct cds_avl_tree tree;
    tree.root = NULL;
    tree.element_size = element_size;
    tree.key_size = key_size;
    tree.cmp = cmp;
    tree.size = 0;
    return tree;
}

/**
 * @brief Recursive helper to delete all nodes in the tree.
 */
static void delete_nodes_recursive(struct cds_avl_node *node) {
    if (node == NULL) {
        return;
    }
    delete_nodes_recursive(node->left);
    delete_nodes_recursive(node->right);
    free_node(node);
}

/**
 * @brief Deletes the internal contents of an AVL tree and frees associated memory.
 */
void cds_avl_tree_delete(struct cds_avl_tree *tree) {
    if (tree == NULL) {
        return;
    }
    delete_nodes_recursive(tree->root);
    tree->root = NULL;
    tree->size = 0;
}


/**
 * @brief Recursive helper for inserting a key-value pair.
 *
 * @param node Current node in the recursion.
 * @param key Key to insert.
 * @param value Value to insert.
 * @param key_size Size of the key.
 * @param element_size Size of the value.
 * @param cmp Comparison function.
 * @param parent Parent of the current node (for new_node).
 * @param success_flag Set to 1 if insertion is successful, 0 if key already exists, -1 for memory error.
 * @return Root of the modified subtree.
 */
static struct cds_avl_node* insert_recursive(struct cds_avl_node *node, const void *key, const void *value,
                                           size_t key_size, size_t element_size,
                                           int (*cmp)(const void*, const void*),
                                           struct cds_avl_node *parent, int *success_flag) {
    if (node == NULL) {
        struct cds_avl_node *new_n = new_node(key, value, key_size, element_size, parent);
        if (new_n == NULL) {
            *success_flag = -1; // Memory error
            return NULL;
        }
        *success_flag = 1; // Successful insertion
        return new_n;
    }

    int cmp_res = cmp(key, node->key);

    if (cmp_res < 0) {
        node->left = insert_recursive(node->left, key, value, key_size, element_size, cmp, node, success_flag);
    } else if (cmp_res > 0) {
        node->right = insert_recursive(node->right, key, value, key_size, element_size, cmp, node, success_flag);
    } else {
        // Key already exists. As per current API, this is an error or no-op.
        // For now, let's treat as "key already exists", not an update.
        *success_flag = 0; // Key already exists
        return node; // No change
    }

    // If insertion failed in a child call, propagate failure
    if (*success_flag == -1) return node; // Or potentially clean up? For now, just propagate.
                                         // The node itself wasn't modified if child insert failed.

    // Update parent pointers for children if they are not NULL
    // This is crucial because rotations might change children, and insert_recursive returns the new child.
    if (node->left != NULL) node->left->parent = node;
    if (node->right != NULL) node->right->parent = node;
    
    return balance(node);
}

/**
 * @brief Inserts a new key-value pair into the AVL tree.
 */
int cds_avl_tree_insert(struct cds_avl_tree *tree, const void *key, const void *value) {
    if (tree == NULL || key == NULL || value == NULL) {
        return -1; // Invalid arguments
    }

    int success_flag = 0; // 0: not inserted (e.g. duplicate), 1: inserted, -1: mem error
    struct cds_avl_node *new_root = insert_recursive(tree->root, key, value, tree->key_size, tree->element_size, tree->cmp, NULL, &success_flag);

    if (success_flag == 1) {
        tree->root = new_root;
        if (tree->root != NULL) { // new_root could be NULL if first insert failed
             tree->root->parent = NULL; // Root's parent is always NULL
        }
        tree->size++;
        return 0; // Success
    } else if (success_flag == -1) {
        return -1; // Memory allocation failure
    } else { // success_flag == 0 (key already exists)
        return -1; // Or a different error code for "key exists" e.g. -2
                   // For now, stick to -1 for general failure as per problem.
    }
}

/**
 * @brief Finds the node with the minimum key value in a subtree.
 * @param node Root of the subtree.
 * @return Node with the minimum key value.
 */
static struct cds_avl_node* min_value_node(struct cds_avl_node *node) {
    struct cds_avl_node *current = node;
    while (current != NULL && current->left != NULL) {
        current = current->left;
    }
    return current;
}


/**
 * @brief Recursive helper for removing a key-value pair.
 *
 * @param node Current node in the recursion.
 * @param key Key to remove.
 * @param cmp Comparison function.
 * @param success_flag Set to 1 if removal is successful, 0 if key not found.
 * @param key_size For copying key in two-child case.
 * @param element_size For copying value in two-child case.
 * @return Root of the modified subtree.
 */
static struct cds_avl_node* remove_recursive(struct cds_avl_node *node, const void *key,
                                           int (*cmp)(const void*, const void*),
                                           int *success_flag,
                                           size_t key_size, size_t element_size) {
    if (node == NULL) {
        *success_flag = 0; // Key not found
        return NULL;
    }

    int cmp_res = cmp(key, node->key);

    if (cmp_res < 0) {
        node->left = remove_recursive(node->left, key, cmp, success_flag, key_size, element_size);
    } else if (cmp_res > 0) {
        node->right = remove_recursive(node->right, key, cmp, success_flag, key_size, element_size);
    } else { // Key found
        *success_flag = 1;
        struct cds_avl_node *temp = NULL;
        // Node with only one child or no child
        if ((node->left == NULL) || (node->right == NULL)) {
            temp = node->left ? node->left : node->right;

            // No child case
            if (temp == NULL) {
                temp = node; // Store node to be freed
                node = NULL; // Effectively removes the node from the tree
                free_node(temp);
            } else { // One child case
                // Copy contents of the non-empty child to this node
                // This is tricky with parent pointers and allocated key/value.
                // A cleaner way is to replace node with temp, and then free the original node.
                // For now, let's directly free 'node' and return 'temp' to be the new child of node's parent.
                // The parent of 'temp' will be updated by the caller of remove_recursive.
                
                // We need to free the old node.
                // 'temp' will take its place.
                // The key/value in 'node' are about to be lost.
                free(node->key);
                free(node->value);
                // Effectively copy temp to node's memory location, then free temp. Or just make node's parent point to temp.
                // The current recursive structure makes the latter easier: return 'temp', and the caller
                // will set its child pointer (e.g., parent->left = temp).
                // The original 'node' must be freed.
                
                struct cds_avl_node* node_to_free = node;
                if (node->left) temp = node->left; else temp = node->right;

                // Update parent pointer of child that will replace 'node'
                if(temp) temp->parent = node->parent; // Crucial: temp's new parent is node's old parent

                free_node(node_to_free); // Free the found node
                node = temp; // 'node' is now the child that replaced it.
            }
        } else { // Node with two children: Get the inorder successor (smallest in the right subtree)
            struct cds_avl_node *inorder_successor = min_value_node(node->right);

            // Free old key/value in current node
            free(node->key);
            free(node->value);

            // Copy the inorder successor's key and value to this node
            node->key = malloc(key_size);
            node->value = malloc(element_size);
            // TODO: Handle malloc failure for key/value copy
            if (node->key == NULL || node->value == NULL) {
                 // This is a problematic state. If key malloc fails, value is still old.
                 // If value malloc fails, key is new.
                 // Best to signal critical error. For now, this might lead to crash or corruption.
                 // A robust implementation might need to revert or have a more complex recovery.
                 if (node->key) free(node->key);
                 if (node->value) free(node->value);
                 // Cannot easily recover the original node's key/value here as they are freed.
                 // This indicates a need for temporary storage if recovery is desired.
                 // For now, assume mallocs succeed or live with potential issues on failure.
                 *success_flag = -2; // Indicate critical internal error
                 return node; // Or NULL to signify error, but tree is now inconsistent
            }

            memcpy(node->key, inorder_successor->key, key_size);
            memcpy(node->value, inorder_successor->value, element_size);

            // Delete the inorder successor from the right subtree
            int dummy_flag; // Success of this sub-deletion is guaranteed as node exists
            node->right = remove_recursive(node->right, inorder_successor->key, cmp, &dummy_flag, key_size, element_size);
        }
    }

    if (node == NULL) { // Tree might have become empty if the root was removed
        return NULL;
    }
    
    // Update parent pointers for children if they are not NULL
    // This is crucial because rotations or child replacement might change children.
    if (node->left != NULL) node->left->parent = node;
    if (node->right != NULL) node->right->parent = node;

    return balance(node);
}


/**
 * @brief Removes a key-value pair from the AVL tree.
 */
int cds_avl_tree_remove(struct cds_avl_tree *tree, const void *key) {
    if (tree == NULL || key == NULL || tree->root == NULL) {
        return -1; // Or specific error for empty tree / key not found
    }

    int success_flag = 0; // 0: not found, 1: removed, -2: internal error during copy
    tree->root = remove_recursive(tree->root, key, tree->cmp, &success_flag, tree->key_size, tree->element_size);
    
    if (tree->root != NULL) {
        tree->root->parent = NULL; // Root's parent is always NULL
    }

    if (success_flag == 1) {
        tree->size--;
        return 0; // Success
    } else if (success_flag == -2) {
        // A critical error occurred during key/value copy in two-child remove case.
        // Tree might be in an inconsistent state.
        return -2; // Or some other error code indicating this specific failure
    }
    
    return -1; // Key not found or other error
}


/**
 * @brief Finds a value in the AVL tree by its key.
 */
void *cds_avl_tree_find(const struct cds_avl_tree *tree, const void *key) {
    if (tree == NULL || key == NULL) {
        return NULL;
    }

    struct cds_avl_node *current = tree->root;
    while (current != NULL) {
        int cmp_res = tree->cmp(key, current->key);
        if (cmp_res < 0) {
            current = current->left;
        } else if (cmp_res > 0) {
            current = current->right;
        } else {
            return current->value; // Key found
        }
    }
    return NULL; // Key not found
}

/**
 * @brief Gets the height of the AVL tree.
 */
int cds_avl_tree_height(const struct cds_avl_tree *tree) {
    if (tree == NULL || tree->root == NULL) {
        return 0; // Or -1 if an empty tree's height is considered -1
                  // Standardly, height of NULL node is 0 or -1, height of leaf is 0 or 1.
                  // Our `height` helper returns 0 for NULL, so an empty tree has height 0.
                  // A tree with one node has height 1.
    }
    return tree->root->height;
}

/**
 * @brief Gets the number of elements in the AVL tree.
 */
size_t cds_avl_tree_size(const struct cds_avl_tree *tree) {
    if (tree == NULL) {
        return 0;
    }
    return tree->size;
}

/**
 * @brief Checks if the AVL tree is empty.
 */
bool cds_avl_tree_empty(const struct cds_avl_tree *tree) {
    if (tree == NULL) {
        return true; // Or handle as an error, but typically NULL tree is empty.
    }
    return tree->size == 0;
    // Alternative: return tree->root == NULL; (should be consistent with size)
}
