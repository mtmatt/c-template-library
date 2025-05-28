#ifndef TEST_RB_TREE_H
#define TEST_RB_TREE_H

#include "cds/rb_tree.h" // For struct cds_rb_tree
#include <assert.h>     // For assert
#include <stdio.h>      // For printf in tests
#include <stdbool.h>    // For bool type in helpers if any

// Test function declarations
void test_rb_tree_basic_operations(void);
void test_rb_tree_insertion_fixup(void);
void test_rb_tree_deletion_fixup(void);
void test_rb_tree_edge_cases(void);
void test_rb_tree_random_operations(void);

// Helper: Integer comparison function
int cmp_int_rb(const void* a, const void* b);

// Optional: If we were to add a validation function (declared here, implemented in rb_tree.c or test_rb_tree.c)
// bool cds_rb_tree_is_valid_for_test(const struct cds_rb_tree *tree);


#endif /* TEST_RB_TREE_H */
