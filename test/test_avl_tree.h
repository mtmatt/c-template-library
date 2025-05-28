#ifndef TEST_AVL_TREE_H
#define TEST_AVL_TREE_H

#include "cds/avl_tree.h" // For struct cds_avl_tree
#include <assert.h>     // For assert
#include <stdio.h>      // For printf in tests

// Test function declarations
void test_avl_tree_basic_operations(void);
void test_avl_tree_insertion_balancing(void);
void test_avl_tree_deletion_balancing(void);
void test_avl_tree_edge_cases(void);
void test_avl_tree_random_operations(void);

// Helper: Integer comparison function
int cmp_int_avl(const void* a, const void* b);

#endif /* TEST_AVL_TREE_H */
