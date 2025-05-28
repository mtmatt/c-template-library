#include <stdlib.h>
#include <string.h>

#include "cds/rb_tree.h"

static struct cds_rb_node *new_node(struct cds_rb_tree *tree, const void *key, const void *value);
static void free_node(struct cds_rb_node *node, struct cds_rb_tree *tree);
static void rotate_left(struct cds_rb_tree *tree, struct cds_rb_node *x);
static void rotate_right(struct cds_rb_tree *tree, struct cds_rb_node *y);
static void cds_rb_tree_insert_fixup(struct cds_rb_tree *tree, struct cds_rb_node *z);
static void cds_rb_tree_transplant(struct cds_rb_tree *tree, struct cds_rb_node *u, struct cds_rb_node *v);
static struct cds_rb_node *min_value_node(struct cds_rb_tree *tree, struct cds_rb_node *node);
static void cds_rb_tree_remove_fixup(struct cds_rb_tree *tree, struct cds_rb_node *x);
static void delete_nodes_recursive(struct cds_rb_tree *tree, struct cds_rb_node *node);

struct cds_rb_tree cds_rb_tree_new(size_t element_size, size_t key_size,
                                   int (*cmp)(const void *, const void *)) {
  struct cds_rb_tree tree;

  tree.nil = (struct cds_rb_node *)malloc(sizeof(struct cds_rb_node));
  if (tree.nil == NULL) {
    tree.cmp = NULL;
    tree.size = 0;
    tree.root = NULL;
    return tree;
  }

  tree.nil->color = CDS_RB_BLACK;
  tree.nil->key = NULL;
  tree.nil->value = NULL;
  tree.nil->left = tree.nil;
  tree.nil->right = tree.nil;
  tree.nil->parent = tree.nil;

  tree.root = tree.nil;
  tree.element_size = element_size;
  tree.key_size = key_size;
  tree.cmp = cmp;
  tree.size = 0;

  return tree;
}

static struct cds_rb_node *new_node(struct cds_rb_tree *tree, const void *key, const void *value) {
  struct cds_rb_node *node = (struct cds_rb_node *)malloc(sizeof(struct cds_rb_node));
  if (node == NULL)
    return NULL;

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
  node->parent = tree->nil;

  return node;
}

static void free_node(struct cds_rb_node *node, struct cds_rb_tree *tree) {
  if (node != tree->nil) {
    free(node->key);
    free(node->value);
    free(node);
  }
}

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

static void rotate_right(struct cds_rb_tree *tree, struct cds_rb_node *y) {
  struct cds_rb_node *x = y->left;
  y->left = x->right;
  if (x->right != tree->nil) {
    x->right->parent = y;
  }
  x->parent = y->parent;
  if (y->parent == tree->nil) {
    tree->root = x;
  } else if (y == y->parent->right) {
    y->parent->right = x;
  } else {
    y->parent->left = x;
  }
  x->right = y;
  y->parent = x;
}

static void cds_rb_tree_insert_fixup(struct cds_rb_tree *tree, struct cds_rb_node *z) {
  while (z->parent->color == CDS_RB_RED) {
    if (z->parent == z->parent->parent->left) {
      struct cds_rb_node *y = z->parent->parent->right;
      if (y->color == CDS_RB_RED) {
        z->parent->color = CDS_RB_BLACK;
        y->color = CDS_RB_BLACK;
        z->parent->parent->color = CDS_RB_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          z = z->parent;
          rotate_left(tree, z);
        }
        z->parent->color = CDS_RB_BLACK;
        z->parent->parent->color = CDS_RB_RED;
        rotate_right(tree, z->parent->parent);
      }
    } else {
      struct cds_rb_node *y = z->parent->parent->left;
      if (y->color == CDS_RB_RED) {
        z->parent->color = CDS_RB_BLACK;
        y->color = CDS_RB_BLACK;
        z->parent->parent->color = CDS_RB_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          rotate_right(tree, z);
        }
        z->parent->color = CDS_RB_BLACK;
        z->parent->parent->color = CDS_RB_RED;
        rotate_left(tree, z->parent->parent);
      }
    }
  }
  tree->root->color = CDS_RB_BLACK;
}

int cds_rb_tree_insert(struct cds_rb_tree *tree, const void *key, const void *value) {
  if (tree == NULL || key == NULL || value == NULL) {
    return -1;
  }

  struct cds_rb_node *z = new_node(tree, key, value);
  if (z == NULL) {
    return -1;
  }

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
      free_node(z, tree);
      return -1;
    }
  }

  z->parent = y;
  if (y == tree->nil) {
    tree->root = z;
  } else if (tree->cmp(key, y->key) < 0) {
    y->left = z;
  } else {
    y->right = z;
  }

  z->left = tree->nil;
  z->right = tree->nil;
  z->color = CDS_RB_RED;

  cds_rb_tree_insert_fixup(tree, z);
  tree->size++;
  return 0;
}

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

static struct cds_rb_node *min_value_node(struct cds_rb_tree *tree, struct cds_rb_node *node) {
  while (node->left != tree->nil) {
    node = node->left;
  }
  return node;
}

static void cds_rb_tree_remove_fixup(struct cds_rb_tree *tree, struct cds_rb_node *x) {
  while (x != tree->root && x->color == CDS_RB_BLACK) {
    if (x == x->parent->left) {
      struct cds_rb_node *w = x->parent->right;
      if (w->color == CDS_RB_RED) {
        w->color = CDS_RB_BLACK;
        x->parent->color = CDS_RB_RED;
        rotate_left(tree, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == CDS_RB_BLACK && w->right->color == CDS_RB_BLACK) {
        w->color = CDS_RB_RED;
        x = x->parent;
      } else {
        if (w->right->color == CDS_RB_BLACK) {
          w->left->color = CDS_RB_BLACK;
          w->color = CDS_RB_RED;
          rotate_right(tree, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = CDS_RB_BLACK;
        w->right->color = CDS_RB_BLACK;
        rotate_left(tree, x->parent);
        x = tree->root;
      }
    } else {
      struct cds_rb_node *w = x->parent->left;
      if (w->color == CDS_RB_RED) {
        w->color = CDS_RB_BLACK;
        x->parent->color = CDS_RB_RED;
        rotate_right(tree, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == CDS_RB_BLACK && w->left->color == CDS_RB_BLACK) {
        w->color = CDS_RB_RED;
        x = x->parent;
      } else {
        if (w->left->color == CDS_RB_BLACK) {
          w->right->color = CDS_RB_BLACK;
          w->color = CDS_RB_RED;
          rotate_left(tree, w);
          w = x->parent->left;
        }
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

int cds_rb_tree_remove(struct cds_rb_tree *tree, const void *key) {
  if (tree == NULL || key == NULL) {
    return -1;
  }

  struct cds_rb_node *z = tree->root;
  while (z != tree->nil && tree->cmp(key, z->key) != 0) {
    if (tree->cmp(key, z->key) < 0) {
      z = z->left;
    } else {
      z = z->right;
    }
  }

  if (z == tree->nil) {
    return -1;
  }

  struct cds_rb_node *y = z;
  struct cds_rb_node *x;
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
    x = y->right;
    if (y->parent == z) {
      x->parent = y;
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

  free_node(z, tree);
  tree->size--;
  return 0;
}

void *cds_rb_tree_find(const struct cds_rb_tree *tree, const void *key) {
  if (tree == NULL || key == NULL) {
    return NULL;
  }

  struct cds_rb_node *current = tree->root;
  while (current != tree->nil) {
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

static void delete_nodes_recursive(struct cds_rb_tree *tree, struct cds_rb_node *node) {
  if (node == tree->nil) {
    return;
  }
  delete_nodes_recursive(tree, node->left);
  delete_nodes_recursive(tree, node->right);
  free_node(node, tree);
}

void cds_rb_tree_delete(struct cds_rb_tree *tree) {
  if (tree == NULL) {
    return;
  }
  delete_nodes_recursive(tree, tree->root);
  free(tree->nil);
  tree->root = NULL;
  tree->nil = NULL;
  tree->size = 0;
}

size_t cds_rb_tree_size(const struct cds_rb_tree *tree) {
  if (tree == NULL) {
    return 0;
  }
  return tree->size;
}

bool cds_rb_tree_empty(const struct cds_rb_tree *tree) {
  if (tree == NULL) {
    return true;
  }
  return tree->size == 0;
}
