#ifndef CDS_RB_TREE_H
#define CDS_RB_TREE_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Color enumeration for Red-Black tree nodes.
 */
enum cds_rb_color {
    CDS_RB_RED,  /**< Red color. */
    CDS_RB_BLACK /**< Black color. */
};

/**
 * @brief Node structure for the Red-Black tree.
 */
struct cds_rb_node {
    void *key;                   /**< Pointer to the key. */
    void *value;                 /**< Pointer to the value. */
    struct cds_rb_node *left;   /**< Pointer to the left child. */
    struct cds_rb_node *right;  /**< Pointer to the right child. */
    struct cds_rb_node *parent; /**< Pointer to the parent node. */
    enum cds_rb_color color;     /**< Color of the node (RED or BLACK). */
};

/**
 * @brief Red-Black tree structure.
 */
struct cds_rb_tree {
    struct cds_rb_node *root;  /**< Pointer to the root node of the tree. */
    struct cds_rb_node *nil;   /**< Pointer to the sentinel NIL node (black, shared by all leaves). */
    size_t element_size;        /**< Size of the elements stored in the tree. */
    size_t key_size;            /**< Size of the keys used for comparison. */
    int (*cmp)(const void *key1, const void *key2); /**< Function pointer for key comparison. */
    size_t size;                /**< Number of elements in the tree. */
};

/**
 * @brief Initializes a new Red-Black tree.
 *
 * The caller is responsible for providing the memory for the `struct cds_rb_tree` itself.
 * This function initializes the provided structure, including setting up the sentinel NIL node.
 * Internal contents (nodes, keys, values) will be heap-allocated during operations like insert.
 *
 * @param element_size The size of the elements to be stored in the tree.
 * @param key_size The size of the keys used for comparison.
 * @param cmp Pointer to the comparison function for keys.
 * @return An initialized Red-Black tree structure (by value).
 */
struct cds_rb_tree cds_rb_tree_new(size_t element_size, size_t key_size, int (*cmp)(const void *, const void *));

/**
 * @brief Deletes the internal contents of a Red-Black tree and frees associated memory.
 *
 * This function frees all nodes (excluding the sentinel NIL node if it's handled carefully),
 * keys, and values stored within the Red-Black tree. It also frees the sentinel NIL node itself.
 * It does not free the `struct cds_rb_tree` itself. After calling this,
 * the tree structure will be empty and can be re-used or deallocated by the caller.
 *
 * @param tree Pointer to the Red-Black tree whose contents are to be deleted.
 */
void cds_rb_tree_delete(struct cds_rb_tree *tree);

/**
 * @brief Inserts a new key-value pair into the Red-Black tree.
 *
 * Maintains Red-Black tree properties after insertion.
 *
 * @param tree Pointer to the Red-Black tree.
 * @param key Pointer to the key to be inserted.
 * @param value Pointer to the value to be inserted.
 * @return 0 on success, non-zero on failure (e.g., memory allocation error, key already exists).
 */
int cds_rb_tree_insert(struct cds_rb_tree *tree, const void *key, const void *value);

/**
 * @brief Removes a key-value pair from the Red-Black tree.
 *
 * Maintains Red-Black tree properties after removal.
 *
 * @param tree Pointer to the Red-Black tree.
 * @param key Pointer to the key to be removed.
 * @return 0 on success, non-zero if the key is not found or on other errors.
 */
int cds_rb_tree_remove(struct cds_rb_tree *tree, const void *key);

/**
 * @brief Finds a value in the Red-Black tree by its key.
 *
 * @param tree Pointer to the Red-Black tree.
 * @param key Pointer to the key to search for.
 * @return Pointer to the value if found, NULL otherwise.
 */
void *cds_rb_tree_find(const struct cds_rb_tree *tree, const void *key);

/**
 * @brief Gets the number of elements in the Red-Black tree.
 *
 * @param tree Pointer to the Red-Black tree.
 * @return The number of elements in the tree.
 */
size_t cds_rb_tree_size(const struct cds_rb_tree *tree);

/**
 * @brief Checks if the Red-Black tree is empty.
 *
 * @param tree Pointer to the Red-Black tree.
 * @return True if the tree is empty, false otherwise.
 */
bool cds_rb_tree_empty(const struct cds_rb_tree *tree);

#endif /* CDS_RB_TREE_H */
