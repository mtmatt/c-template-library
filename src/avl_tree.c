#include <stdlib.h>
#include <string.h>

#include "cds/avl_tree.h"

static int height(struct cds_avl_node *node) {
  if (node == NULL) {
    return 0;
  }
  return node->height;
}

static int max(int a, int b) { 
  return (a > b) ? a : b; 
}

static void update_height(struct cds_avl_node *node) {
  if (node != NULL) {
    node->height = 1 + max(height(node->left), height(node->right));
  }
}

static int balance_factor(struct cds_avl_node *node) {
  if (node == NULL) {
    return 0;
  }
  return height(node->left) - height(node->right);
}

static struct cds_avl_node *new_node(const void *key, const void *value, size_t key_size, 
                                     size_t element_size, struct cds_avl_node *parent) {
  struct cds_avl_node *node = (struct cds_avl_node*) malloc(sizeof(struct cds_avl_node));
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
  node->height = 1;

  return node;
}

static void free_node(struct cds_avl_node *node) {
  if (node != NULL) {
    free(node->key);
    free(node->value);
    free(node);
  }
}

static struct cds_avl_node *rotate_left(struct cds_avl_node *x) {
  struct cds_avl_node *y = x->right;
  struct cds_avl_node *T2 = y->left;

  y->left = x;
  x->right = T2;

  y->parent = x->parent;
  x->parent = y;
  if (T2 != NULL) {
    T2->parent = x;
  }

  update_height(x);
  update_height(y);

  return y;
}

static struct cds_avl_node *rotate_right(struct cds_avl_node *y) {
  struct cds_avl_node *x = y->left;
  struct cds_avl_node *T2 = x->right;

  x->right = y;
  y->left = T2;

  x->parent = y->parent;
  y->parent = x;
  if (T2 != NULL) {
    T2->parent = y;
  }

  update_height(y);
  update_height(x);

  return x;
}

static struct cds_avl_node *balance(struct cds_avl_node *node) {
  if (node == NULL)
    return node;

  update_height(node);
  int bf = balance_factor(node);

  if (bf > 1) {
    if (balance_factor(node->left) < 0) {
      node->left = rotate_left(node->left);
      if (node->left)
        node->left->parent = node;
    }
    return rotate_right(node);
  }
  if (bf < -1) {
    if (balance_factor(node->right) > 0) {
      node->right = rotate_right(node->right);
      if (node->right)
        node->right->parent = node;
    }
    return rotate_left(node);
  }

  return node;
}

struct cds_avl_tree cds_avl_tree_new(size_t element_size, size_t key_size, int (*cmp)(const void *, const void *)) {
  struct cds_avl_tree tree;
  tree.root = NULL;
  tree.element_size = element_size;
  tree.key_size = key_size;
  tree.cmp = cmp;
  tree.size = 0;
  return tree;
}

static void delete_nodes_recursive(struct cds_avl_node *node) {
  if (node == NULL) {
    return;
  }
  delete_nodes_recursive(node->left);
  delete_nodes_recursive(node->right);
  free_node(node);
}

void cds_avl_tree_delete(struct cds_avl_tree *tree) {
  if (tree == NULL) {
    return;
  }
  delete_nodes_recursive(tree->root);
  tree->root = NULL;
  tree->size = 0;
}

static struct cds_avl_node *insert_recursive(struct cds_avl_node *node, const void *key, const void *value,
                                             size_t key_size, size_t element_size,
                                             int (*cmp)(const void *, const void *),
                                             struct cds_avl_node *parent, int *success_flag) {
  if (node == NULL) {
    struct cds_avl_node *new_n = new_node(key, value, key_size, element_size, parent);
    if (new_n == NULL) {
      *success_flag = -1;
      return NULL;
    }
    *success_flag = 1;
    return new_n;
  }

  int cmp_res = cmp(key, node->key);

  if (cmp_res < 0) {
    node->left = insert_recursive(node->left, key, value, key_size,
                                  element_size, cmp, node, success_flag);
  } else if (cmp_res > 0) {
    node->right = insert_recursive(node->right, key, value, key_size,
                                   element_size, cmp, node, success_flag);
  } else {
    *success_flag = 0;
    return node;
  }

  if (*success_flag == -1)
    return node;

  if (node->left != NULL)
    node->left->parent = node;
  if (node->right != NULL)
    node->right->parent = node;

  return balance(node);
}

int cds_avl_tree_insert(struct cds_avl_tree *tree, const void *key, const void *value) {
  if (tree == NULL || key == NULL || value == NULL) {
    return -1;
  }

  int success_flag = 0;
  struct cds_avl_node *new_root = insert_recursive(tree->root, key, value, tree->key_size,
                       tree->element_size, tree->cmp, NULL, &success_flag);

  if (success_flag == 1) {
    tree->root = new_root;
    if (tree->root != NULL) {
      tree->root->parent = NULL;
    }
    tree->size++;
    return 0;
  } else if (success_flag == -1) {
    return -1;
  } else {
    return -1;
  }
}

static struct cds_avl_node *min_value_node(struct cds_avl_node *node) {
  struct cds_avl_node *current = node;
  while (current != NULL && current->left != NULL) {
    current = current->left;
  }
  return current;
}

static struct cds_avl_node *remove_recursive(struct cds_avl_node *node, const void *key,
                 int (*cmp)(const void *, const void *), int *success_flag,
                 size_t key_size, size_t element_size) {
  if (node == NULL) {
    *success_flag = 0;
    return NULL;
  }

  int cmp_res = cmp(key, node->key);

  if (cmp_res < 0) {
    node->left = remove_recursive(node->left, key, cmp, success_flag, key_size, element_size);
  } else if (cmp_res > 0) {
    node->right = remove_recursive(node->right, key, cmp, success_flag, key_size, element_size);
  } else {
    *success_flag = 1;
    struct cds_avl_node *temp = NULL;
    if ((node->left == NULL) || (node->right == NULL)) {
      temp = node->left ? node->left : node->right;

      if (temp == NULL) {
        temp = node;
        node = NULL;
        free_node(temp);
      } else {
        struct cds_avl_node *node_to_free = node;
        if (node->left)
          temp = node->left;
        else
          temp = node->right;

        if (temp)
          temp->parent = node->parent;

        free_node(node_to_free);
        node = temp;
      }
    } else {
      struct cds_avl_node *inorder_successor = min_value_node(node->right);

      free(node->key);
      free(node->value);

      node->key = malloc(key_size);
      if (node->key == NULL) {
        *success_flag = -2;
        return node;
      }
      memcpy(node->key, inorder_successor->key, key_size);

      node->value = malloc(element_size);
      if (node->value == NULL) {
        free(node->key);
        node->key = NULL;
        *success_flag = -2;
        return node;
      }
      memcpy(node->value, inorder_successor->value, element_size);

      int dummy_flag;
      node->right = remove_recursive(node->right, inorder_successor->key, cmp,
                                     &dummy_flag, key_size, element_size);
    }
  }

  if (node == NULL) {
    return NULL;
  }

  if (node->left != NULL)
    node->left->parent = node;
  if (node->right != NULL)
    node->right->parent = node;

  return balance(node);
}

int cds_avl_tree_remove(struct cds_avl_tree *tree, const void *key) {
  if (tree == NULL || key == NULL || tree->root == NULL) {
    return -1;
  }

  int success_flag = 0;
  tree->root = remove_recursive(tree->root, key, tree->cmp, &success_flag,
                                tree->key_size, tree->element_size);

  if (tree->root != NULL) {
    tree->root->parent = NULL;
  }

  if (success_flag == 1) {
    tree->size--;
    return 0;
  } else if (success_flag == -2) {
    return -2;
  }

  return -1;
}

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
      return current->value;
    }
  }
  return NULL;
}

int cds_avl_tree_height(const struct cds_avl_tree *tree) {
  if (tree == NULL || tree->root == NULL) {
    return 0;
  }
  return tree->root->height;
}

size_t cds_avl_tree_size(const struct cds_avl_tree *tree) {
  if (tree == NULL) {
    return 0;
  }
  return tree->size;
}

bool cds_avl_tree_empty(const struct cds_avl_tree *tree) {
  if (tree == NULL) {
    return true;
  }
  return tree->size == 0;
}
