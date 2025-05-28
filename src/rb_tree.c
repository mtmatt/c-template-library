#include <stdlib.h>
#include <string.h>
#include "rb_tree.h"

// --- Static Helper Prototypes ---
static struct cds_rb_node* new_node(struct cds_rb_tree *tree, const void *key, const void *value);
static void free_node(struct cds_rb_node *node, struct cds_rb_tree *tree); // tree needed to check against nil
static void rotate_left(struct cds_rb_tree *tree, struct cds_rb_node *x);
static void rotate_right(struct cds_rb_tree *tree, struct cds_rb_node *y);
static void cds_rb_tree_insert_fixup(struct cds_rb_tree *tree, struct cds_rb_node *z);
static void cds_rb_tree_transplant(struct cds_rb_tree *tree, struct cds_rb_node *u, struct cds_rb_node *v);
static struct cds_rb_node* min_value_node(struct cds_rb_tree *tree, struct cds_rb_node *node);
static void cds_rb_tree_remove_fixup(struct cds_rb_tree *tree, struct cds_rb_node *x);
static void delete_nodes_recursive(struct cds_rb_tree *tree, struct cds_rb_node *node);

// --- Public API Functions ---

/**
 * @brief Initializes a new Red-Black tree.
 */
struct cds_rb_tree cds_rb_tree_new(size_t element_size, size_t key_size, int (*cmp)(const void *, const void *)) {
    struct cds_rb_tree tree;

    tree.nil = (struct cds_rb_node*)malloc(sizeof(struct cds_rb_node));
    // TODO: Check malloc failure for nil
    if (tree.nil == NULL) {
        // This is problematic as the function must return a struct cds_rb_tree.
        // The API doesn't allow returning a failure indicator here easily.
        // For now, assume nil allocation succeeds. A robust solution might involve
        // a tree_is_valid flag or changing API to return pointer/status.
        // Or, the caller could check tree.nil == NULL after this call.
        // Let's proceed with the assumption it works, or set size to a specific error value.
        // For now, if nil fails, root will be NULL, and other ops might fail.
        // A practical approach: set cmp to NULL to indicate an invalid tree.
        tree.cmp = NULL; // Indicate failure
        tree.size = 0;
        tree.root = NULL; // Cannot set to tree.nil if tree.nil is NULL
        return tree;
    }

    tree.nil->color = CDS_RB_BLACK;
    tree.nil->key = NULL;
    tree.nil->value = NULL;
    tree.nil->left = tree.nil;   // Sentinel's children point to itself
    tree.nil->right = tree.nil;  // Sentinel's children point to itself
    tree.nil->parent = tree.nil; // Sentinel's parent can also point to itself

    tree.root = tree.nil;
    tree.element_size = element_size;
    tree.key_size = key_size;
    tree.cmp = cmp;
    tree.size = 0;

    return tree;
}

/**
 * @brief Allocates and initializes a new node.
 * Key and value are copied. New nodes are RED. Children point to tree->nil.
 */
static struct cds_rb_node* new_node(struct cds_rb_tree *tree, const void *key, const void *value) {
    struct cds_rb_node *node = (struct cds_rb_node*)malloc(sizeof(struct cds_rb_node));
    if (node == NULL) return NULL;

    node->key = malloc(tree->key_size);
    if (node->key == NULL) {
        free(node);
        return NULL;
    }
    memcpy(node->key, key, tree->key_size);

    node->value = malloc(tree->element_size);
    if (node->value == NULL) {
        free(node->key);
        free(node);
        return NULL;
    }
    memcpy(node->value, value, tree->element_size);

    node->color = CDS_RB_RED;
    node->left = tree->nil;
    node->right = tree->nil;
    node->parent = tree->nil; // Parent will be set by caller (insert)

    return node;
}

/**
 * @brief Frees a node and its copied key/value. Does not free tree->nil.
 */
static void free_node(struct cds_rb_node *node, struct cds_rb_tree *tree) {
    if (node == NULL || node == tree->nil) { // Do not free the sentinel
        return;
    }
    free(node->key);
    free(node->value);
    free(node);
}

/**
 * @brief Performs a left rotation on the subtree rooted with x.
 */
static void rotate_left(struct cds_rb_tree *tree, struct cds_rb_node *x) {
    struct cds_rb_node *y = x->right;
    x->right = y->left;

    if (y->left != tree->nil) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

/**
 * @brief Performs a right rotation on the subtree rooted with y.
 */
static void rotate_right(struct cds_rb_tree *tree, struct cds_rb_node *x) { // x is the node to rotate (was y in some texts)
    struct cds_rb_node *y = x->left;
    x->left = y->right;

    if (y->right != tree->nil) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == tree->nil) {
        tree->root = y;
    } else if (x == x->parent->right) {
        x->parent->right = y;
    } else {
        x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
}

/**
 * @brief Restores Red-Black tree properties after insertion.
 */
static void cds_rb_tree_insert_fixup(struct cds_rb_tree *tree, struct cds_rb_node *z) {
    struct cds_rb_node *y; // Uncle
    while (z->parent->color == CDS_RB_RED) {
        if (z->parent == z->parent->parent->left) { // Parent is a left child
            y = z->parent->parent->right; // Uncle
            if (y->color == CDS_RB_RED) { // Case 1: Uncle is RED
                z->parent->color = CDS_RB_BLACK;
                y->color = CDS_RB_BLACK;
                z->parent->parent->color = CDS_RB_RED;
                z = z->parent->parent;
            } else { // Uncle is BLACK
                if (z == z->parent->right) { // Case 2: z is a right child (LR)
                    z = z->parent;
                    rotate_left(tree, z);
                }
                // Case 3: z is a left child (LL)
                z->parent->color = CDS_RB_BLACK;
                z->parent->parent->color = CDS_RB_RED;
                rotate_right(tree, z->parent->parent);
            }
        } else { // Parent is a right child (symmetric to above)
            y = z->parent->parent->left; // Uncle
            if (y->color == CDS_RB_RED) { // Case 1
                z->parent->color = CDS_RB_BLACK;
                y->color = CDS_RB_BLACK;
                z->parent->parent->color = CDS_RB_RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) { // Case 2 (RL)
                    z = z->parent;
                    rotate_right(tree, z);
                }
                // Case 3 (RR)
                z->parent->color = CDS_RB_BLACK;
                z->parent->parent->color = CDS_RB_RED;
                rotate_left(tree, z->parent->parent);
            }
        }
    }
    tree->root->color = CDS_RB_BLACK;
}

/**
 * @brief Inserts a new key-value pair into the Red-Black tree.
 */
int cds_rb_tree_insert(struct cds_rb_tree *tree, const void *key, const void *value) {
    if (tree == NULL || tree->nil == NULL || tree->cmp == NULL) return -1; // Invalid tree

    struct cds_rb_node *y = tree->nil;
    struct cds_rb_node *x = tree->root;

    while (x != tree->nil) {
        y = x;
        int cmp_res = tree->cmp(key, x->key);
        if (cmp_res < 0) {
            x = x->left;
        } else if (cmp_res > 0) {
            x = x->right;
        } else {
            return -1; // Key already exists
        }
    }

    struct cds_rb_node *z = new_node(tree, key, value);
    if (z == NULL) {
        return -1; // Memory allocation failure
    }

    z->parent = y;
    if (y == tree->nil) {
        tree->root = z; // Tree was empty
    } else if (tree->cmp(z->key, y->key) < 0) {
        y->left = z;
    } else {
        y->right = z;
    }

    // z->left and z->right are already tree->nil from new_node
    // z->color is already CDS_RB_RED from new_node

    cds_rb_tree_insert_fixup(tree, z);
    tree->size++;
    return 0;
}

/**
 * @brief Replaces subtree rooted at u with subtree rooted at v.
 */
static void cds_rb_tree_transplant(struct cds_rb_tree *tree, struct cds_rb_node *u, struct cds_rb_node *v) {
    if (u->parent == tree->nil) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    v->parent = u->parent;
}

/**
 * @brief Finds the node with the minimum key value in a subtree rooted at node.
 */
static struct cds_rb_node* min_value_node(struct cds_rb_tree *tree, struct cds_rb_node *node) {
    while (node->left != tree->nil) {
        node = node->left;
    }
    return node;
}

/**
 * @brief Restores Red-Black tree properties after removal.
 */
static void cds_rb_tree_remove_fixup(struct cds_rb_tree *tree, struct cds_rb_node *x) {
    struct cds_rb_node *w; // Sibling of x
    while (x != tree->root && x->color == CDS_RB_BLACK) {
        if (x == x->parent->left) { // x is left child
            w = x->parent->right; // Sibling
            if (w->color == CDS_RB_RED) { // Case 1: Sibling w is red
                w->color = CDS_RB_BLACK;
                x->parent->color = CDS_RB_RED;
                rotate_left(tree, x->parent);
                w = x->parent->right; // New sibling
            }
            // Case 2: Sibling w is black, and both of w's children are black
            if (w->left->color == CDS_RB_BLACK && w->right->color == CDS_RB_BLACK) {
                w->color = CDS_RB_RED;
                x = x->parent; // Move up the tree
            } else {
                // Case 3: Sibling w is black, w->left is red, w->right is black
                if (w->right->color == CDS_RB_BLACK) {
                    w->left->color = CDS_RB_BLACK;
                    w->color = CDS_RB_RED;
                    rotate_right(tree, w);
                    w = x->parent->right; // New sibling
                }
                // Case 4: Sibling w is black, w->right is red
                w->color = x->parent->color;
                x->parent->color = CDS_RB_BLACK;
                w->right->color = CDS_RB_BLACK;
                rotate_left(tree, x->parent);
                x = tree->root; // Fixup is complete
            }
        } else { // x is right child (symmetric to above)
            w = x->parent->left; // Sibling
            if (w->color == CDS_RB_RED) { // Case 1
                w->color = CDS_RB_BLACK;
                x->parent->color = CDS_RB_RED;
                rotate_right(tree, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == CDS_RB_BLACK && w->left->color == CDS_RB_BLACK) { // Case 2
                w->color = CDS_RB_RED;
                x = x->parent;
            } else {
                if (w->left->color == CDS_RB_BLACK) { // Case 3
                    w->right->color = CDS_RB_BLACK;
                    w->color = CDS_RB_RED;
                    rotate_left(tree, w);
                    w = x->parent->left;
                }
                // Case 4
                w->color = x->parent->color;
                x->parent->color = CDS_RB_BLACK;
                w->left->color = CDS_RB_BLACK;
                rotate_right(tree, x->parent);
                x = tree->root;
            }
        }
    }
    x->color = CDS_RB_BLACK;
}

/**
 * @brief Removes a key-value pair from the Red-Black tree.
 */
int cds_rb_tree_remove(struct cds_rb_tree *tree, const void *key) {
    if (tree == NULL || tree->nil == NULL || tree->cmp == NULL || tree->root == tree->nil) return -1;

    struct cds_rb_node *z = tree->root;
    struct cds_rb_node *node_to_delete = tree->nil;

    // Find the node to delete
    while(z != tree->nil) {
        int cmp_res = tree->cmp(key, z->key);
        if (cmp_res == 0) {
            node_to_delete = z;
            break;
        } else if (cmp_res < 0) {
            z = z->left;
        } else {
            z = z->right;
        }
    }

    if (node_to_delete == tree->nil) {
        return -1; // Key not found
    }
    
    z = node_to_delete; // z is the node to be deleted

    struct cds_rb_node *y = z; // y is the node either removed or moved within the tree
    struct cds_rb_node *x;     // x is the child that replaces y
    enum cds_rb_color y_original_color = y->color;

    if (z->left == tree->nil) {
        x = z->right;
        cds_rb_tree_transplant(tree, z, z->right);
    } else if (z->right == tree->nil) {
        x = z->left;
        cds_rb_tree_transplant(tree, z, z->left);
    } else {
        y = min_value_node(tree, z->right);
        y_original_color = y->color;
        x = y->right; // x could be tree->nil

        if (y->parent == z) {
            x->parent = y; // Important if x is tree->nil
        } else {
            cds_rb_tree_transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        cds_rb_tree_transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    if (y_original_color == CDS_RB_BLACK) {
        cds_rb_tree_remove_fixup(tree, x);
    }

    free_node(z, tree); // z is always the node with the target key
    tree->size--;
    return 0;
}

/**
 * @brief Finds a value in the Red-Black tree by its key.
 */
void *cds_rb_tree_find(const struct cds_rb_tree *tree, const void *key) {
    if (tree == NULL || tree->nil == NULL || tree->cmp == NULL) return NULL;

    struct cds_rb_node *current = tree->root;
    while (current != tree->nil) {
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
 * @brief Recursive helper to delete all actual data nodes.
 */
static void delete_nodes_recursive(struct cds_rb_tree *tree, struct cds_rb_node *node) {
    if (node == tree->nil) {
        return;
    }
    delete_nodes_recursive(tree, node->left);
    delete_nodes_recursive(tree, node->right);
    free_node(node, tree);
}

/**
 * @brief Deletes the internal contents of a Red-Black tree.
 */
void cds_rb_tree_delete(struct cds_rb_tree *tree) {
    if (tree == NULL) return;

    if (tree->nil != NULL) { // Check if tree was successfully initialized
        if (tree->root != NULL) { // Check if root is valid (could be tree->nil)
             delete_nodes_recursive(tree, tree->root);
        }
        free(tree->nil); // Free the sentinel node itself
    }

    tree->root = NULL; // Or tree->nil, but nil is now freed.
                       // Setting to NULL is safer to indicate unusable state.
    tree->nil = NULL;  // Mark nil as freed
    tree->size = 0;
    // tree->cmp remains, tree->element_size, tree->key_size remain
    // The tree struct itself is not freed.
}

/**
 * @brief Gets the number of elements in the Red-Black tree.
 */
size_t cds_rb_tree_size(const struct cds_rb_tree *tree) {
    if (tree == NULL || tree->cmp == NULL) { // Check cmp to see if tree was properly initialized
        return 0;
    }
    return tree->size;
}

/**
 * @brief Checks if the Red-Black tree is empty.
 */
bool cds_rb_tree_empty(const struct cds_rb_tree *tree) {
    if (tree == NULL || tree->cmp == NULL) {
        return true;
    }
    return tree->size == 0;
}
