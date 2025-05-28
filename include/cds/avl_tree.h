#ifndef CDS_AVL_TREE_H
#define CDS_AVL_TREE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct cds_avl_node {
  void *key;
  void *value;
  struct cds_avl_node *left;
  struct cds_avl_node *right;
  struct cds_avl_node *parent;
  int height;
} CdsAvlNode;

typedef struct cds_avl_tree {
  struct cds_avl_node *root;
  size_t element_size;
  size_t key_size;
  int (*cmp)(const void *key1, const void *key2);
  size_t size;
} CdsAvlTree;

/*
 *********************************************************************************************************
 *
 *                                          CDS AVL TREE NEW
 * 
 * Description: Initializes a new AVL tree.
 * 
 * Arguments: element_size   The size of the elements to be stored in the tree.
 *            key_size       The size of the keys used for comparison.
 *            cmp            Pointer to the comparison function for keys.
 *
 * Returns: An initialized AVL tree structure (by value).
 * 
 * Notes: The caller is responsible for providing the memory for the struct cds_avl_tree itself.
 *        Internal contents (nodes, keys, values) will be heap-allocated during operations like insert.
 *********************************************************************************************************
 */
struct cds_avl_tree cds_avl_tree_new(size_t element_size, size_t key_size,
                                     int (*cmp)(const void *, const void *));

/*
 *********************************************************************************************************
 *
 *                                        CDS AVL TREE DELETE
 * 
 * Description: Deletes the internal contents of an AVL tree and frees associated memory.
 * 
 * Arguments: tree   Pointer to the AVL tree whose contents are to be deleted.
 *
 * Returns: none
 * 
 * Notes: This function frees all nodes, keys, and values stored within the AVL tree.
 *        It does not free the struct cds_avl_tree itself. After calling this,
 *        the tree structure will be empty and can be re-used or deallocated by the caller.
 *********************************************************************************************************
 */
void cds_avl_tree_delete(struct cds_avl_tree *tree);

/*
 *********************************************************************************************************
 *
 *                                        CDS AVL TREE INSERT
 * 
 * Description: Inserts a new key-value pair into the AVL tree.
 * 
 * Arguments: tree    Pointer to the AVL tree.
 *            key     Pointer to the key to be inserted.
 *            value   Pointer to the value to be inserted.
 *
 * Returns: 0 on success, non-zero on failure.
 * 
 * Notes: none
 *********************************************************************************************************
 */
int cds_avl_tree_insert(struct cds_avl_tree *tree, const void *key,
                        const void *value);

/*
 *********************************************************************************************************
 *
 *                                        CDS AVL TREE REMOVE
 * 
 * Description: Removes a key-value pair from the AVL tree.
 * 
 * Arguments: tree   Pointer to the AVL tree.
 *            key    Pointer to the key to be removed.
 *
 * Returns: 0 on success, non-zero if the key is not found.
 * 
 * Notes: none
 *********************************************************************************************************
 */
int cds_avl_tree_remove(struct cds_avl_tree *tree, const void *key);

/*
 *********************************************************************************************************
 *
 *                                         CDS AVL TREE FIND
 * 
 * Description: Finds a value in the AVL tree by its key.
 * 
 * Arguments: tree   Pointer to the AVL tree.
 *            key    Pointer to the key to search for.
 *
 * Returns: Pointer to the value if found, NULL otherwise.
 * 
 * Notes: none
 *********************************************************************************************************
 */
void *cds_avl_tree_find(const struct cds_avl_tree *tree, const void *key);

/*
 *********************************************************************************************************
 *
 *                                       CDS AVL TREE HEIGHT
 * 
 * Description: Gets the height of the AVL tree.
 * 
 * Arguments: tree   Pointer to the AVL tree.
 *
 * Returns: The height of the tree.
 * 
 * Notes: none
 *********************************************************************************************************
 */
int cds_avl_tree_height(const struct cds_avl_tree *tree);

/*
 *********************************************************************************************************
 *
 *                                        CDS AVL TREE SIZE
 * 
 * Description: Gets the number of elements in the AVL tree.
 * 
 * Arguments: tree   Pointer to the AVL tree.
 *
 * Returns: The number of elements in the tree.
 * 
 * Notes: none
 *********************************************************************************************************
 */
size_t cds_avl_tree_size(const struct cds_avl_tree *tree);

/*
 *********************************************************************************************************
 *
 *                                        CDS AVL TREE EMPTY
 * 
 * Description: Checks if the AVL tree is empty.
 * 
 * Arguments: tree   Pointer to the AVL tree.
 *
 * Returns: true if the tree is empty, false otherwise.
 * 
 * Notes: none
 *********************************************************************************************************
 */
bool cds_avl_tree_empty(const struct cds_avl_tree *tree);

#endif /* CDS_AVL_TREE_H */
