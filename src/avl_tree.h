#ifndef CDS_AVL_TREE_H
#define CDS_AVL_TREE_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief Node structure for the AVL tree.
 */
struct cds_avl_node {
    void *key;                   /**< Pointer to the key. */
    void *value;                 /**< Pointer to the value. */
    struct cds_avl_node *left;   /**< Pointer to the left child. */
    struct cds_avl_node *right;  /**< Pointer to the right child. */
    struct cds_avl_node *parent; /**< Pointer to the parent node. */
    int height;                  /**< Height of the node. */
};

/**
 * @brief AVL tree structure.
 */
struct cds_avl_tree {
    struct cds_avl_node *root;  /**< Pointer to the root node of the tree. */
    size_t element_size;        /**< Size of the elements stored in the tree. */
    size_t key_size;            /**< Size of the keys used for comparison. */
    int (*cmp)(const void *key1, const void *key2); /**< Function pointer for key comparison. */
    size_t size;                /**< Number of elements in the tree. */
};

/**
 * @brief Initializes a new AVL tree.
 *
 * The caller is responsible for providing the memory for the `struct cds_avl_tree` itself.
 * This function initializes the provided structure. Internal contents (nodes, keys, values)
 * will be heap-allocated during operations like insert.
 *
 * @param element_size The size of the elements to be stored in the tree.
 * @param key_size The size of the keys used for comparison.
 * @param cmp Pointer to the comparison function for keys.
 * @return An initialized AVL tree structure (by value).
 */
struct cds_avl_tree cds_avl_tree_new(size_t element_size, size_t key_size, int (*cmp)(const void *, const void *));

/**
 * @brief Deletes the internal contents of an AVL tree and frees associated memory.
 *
 * This function frees all nodes, keys, and values stored within the AVL tree.
 * It does not free the `struct cds_avl_tree` itself. After calling this,
 * the tree structure will be empty and can be re-used or deallocated by the caller.
 *
 * @param tree Pointer to the AVL tree whose contents are to be deleted.
 */
void cds_avl_tree_delete(struct cds_avl_tree *tree);

/**
 * @brief Inserts a new key-value pair into the AVL tree.
 *
 * @param tree Pointer to the AVL tree.
 * @param key Pointer to the key to be inserted.
 * @param value Pointer to the value to be inserted.
 * @return 0 on success, non-zero on failure.
 */
int cds_avl_tree_insert(struct cds_avl_tree *tree, const void *key, const void *value);

/**
 * @brief Removes a key-value pair from the AVL tree.
 *
 * @param tree Pointer to the AVL tree.
 * @param key Pointer to the key to be removed.
 * @return 0 on success, non-zero if the key is not found.
 */
int cds_avl_tree_remove(struct cds_avl_tree *tree, const void *key);

/**
 * @brief Finds a value in the AVL tree by its key.
 *
 * @param tree Pointer to the AVL tree.
 * @param key Pointer to the key to search for.
 * @return Pointer to the value if found, NULL otherwise.
 */
void *cds_avl_tree_find(const struct cds_avl_tree *tree, const void *key);

/**
 * @brief Gets the height of the AVL tree.
 *
 * @param tree Pointer to the AVL tree.
 * @return The height of the tree.
 */
int cds_avl_tree_height(const struct cds_avl_tree *tree);

/**
 * @brief Gets the number of elements in the AVL tree.
 *
 * @param tree Pointer to the AVL tree.
 * @return The number of elements in the tree.
 */
size_t cds_avl_tree_size(const struct cds_avl_tree *tree);

/**
 * @brief Checks if the AVL tree is empty.
 *
 * @param tree Pointer to the AVL tree.
 * @return True if the tree is empty, false otherwise.
 */
bool cds_avl_tree_empty(const struct cds_avl_tree *tree);

#endif /* CDS_AVL_TREE_H */
