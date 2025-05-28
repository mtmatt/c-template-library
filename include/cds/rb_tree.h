#ifndef CDS_RB_TREE_H
#define CDS_RB_TREE_H

#include <stdbool.h>
#include <stddef.h>

enum cds_rb_color {
  CDS_RB_RED,
  CDS_RB_BLACK
};

typedef struct cds_rb_node {
  void *key;
  void *value;
  struct cds_rb_node *left;
  struct cds_rb_node *right;
  struct cds_rb_node *parent;
  enum cds_rb_color color;
} CdsRbNode;

typedef struct cds_rb_tree {
  struct cds_rb_node *root;
  struct cds_rb_node *nil;
  size_t element_size;
  size_t key_size;
  int (*cmp)(const void *key1, const void *key2);
  size_t size;
} CdsRbTree;

/*
 *********************************************************************************************************
 *
 *                                           CDS RB TREE NEW
 * 
 * Description: Initializes a new Red-Black tree.
 * 
 * Arguments: element_size   The size of the elements to be stored in the tree.
 *            key_size       The size of the keys used for comparison.
 *            cmp            Pointer to the comparison function for keys.
 *
 * Returns: An initialized Red-Black tree structure (by value).
 * 
 * Notes: The caller is responsible for providing the memory for the struct cds_rb_tree itself.
 *        This function initializes the provided structure, including setting up the sentinel NIL node.
 *        Internal contents (nodes, keys, values) will be heap-allocated during operations like insert.
 *********************************************************************************************************
 */
struct cds_rb_tree cds_rb_tree_new(size_t element_size, size_t key_size,
                                   int (*cmp)(const void *, const void *));

/*
 *********************************************************************************************************
 *
 *                                         CDS RB TREE DELETE
 * 
 * Description: Deletes the internal contents of a Red-Black tree and frees associated memory.
 * 
 * Arguments: tree   Pointer to the Red-Black tree whose contents are to be deleted.
 *
 * Returns: none
 * 
 * Notes: This function frees all nodes (excluding the sentinel NIL node if it's handled carefully),
 *        keys, and values stored within the Red-Black tree. It also frees the sentinel NIL node itself.
 *        It does not free the struct cds_rb_tree itself. After calling this, the tree structure
 *        will be empty and can be re-used or deallocated by the caller.
 *********************************************************************************************************
 */
void cds_rb_tree_delete(struct cds_rb_tree *tree);

/*
 *********************************************************************************************************
 *
 *                                         CDS RB TREE INSERT
 * 
 * Description: Inserts a new key-value pair into the Red-Black tree.
 * 
 * Arguments: tree    Pointer to the Red-Black tree.
 *            key     Pointer to the key to be inserted.
 *            value   Pointer to the value to be inserted.
 *
 * Returns: 0 on success, non-zero on failure (e.g., memory allocation error, key already exists).
 * 
 * Notes: Maintains Red-Black tree properties after insertion.
 *********************************************************************************************************
 */
int cds_rb_tree_insert(struct cds_rb_tree *tree, const void *key,
                       const void *value);

/*
 *********************************************************************************************************
 *
 *                                         CDS RB TREE REMOVE
 * 
 * Description: Removes a key-value pair from the Red-Black tree.
 * 
 * Arguments: tree   Pointer to the Red-Black tree.
 *            key    Pointer to the key to be removed.
 *
 * Returns: 0 on success, non-zero if the key is not found or on other errors.
 * 
 * Notes: Maintains Red-Black tree properties after removal.
 *********************************************************************************************************
 */
int cds_rb_tree_remove(struct cds_rb_tree *tree, const void *key);

/*
 *********************************************************************************************************
 *
 *                                          CDS RB TREE FIND
 * 
 * Description: Finds a value in the Red-Black tree by its key.
 * 
 * Arguments: tree   Pointer to the Red-Black tree.
 *            key    Pointer to the key to search for.
 *
 * Returns: Pointer to the value if found, NULL otherwise.
 * 
 * Notes: none
 *********************************************************************************************************
 */
void *cds_rb_tree_find(const struct cds_rb_tree *tree, const void *key);

/*
 *********************************************************************************************************
 *
 *                                          CDS RB TREE SIZE
 * 
 * Description: Gets the number of elements in the Red-Black tree.
 * 
 * Arguments: tree   Pointer to the Red-Black tree.
 *
 * Returns: The number of elements in the tree.
 * 
 * Notes: none
 *********************************************************************************************************
 */
size_t cds_rb_tree_size(const struct cds_rb_tree *tree);

/*
 *********************************************************************************************************
 *
 *                                         CDS RB TREE EMPTY
 * 
 * Description: Checks if the Red-Black tree is empty.
 * 
 * Arguments: tree   Pointer to the Red-Black tree.
 *
 * Returns: true if the tree is empty, false otherwise.
 * 
 * Notes: none
 *********************************************************************************************************
 */
bool cds_rb_tree_empty(const struct cds_rb_tree *tree);

#endif /* CDS_RB_TREE_H */
