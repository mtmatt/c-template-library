#include <stdio.h>

#include "test_array.h"
#include "test_queue.h"
#include "test_stack.h"
#include "test_list.h"
#include "test_string.h"
#include "test_heap.h"
#include "test_avl_tree.h"
#include "test_rb_tree.h"
#include "test_sort.h"

int main(void) {
  printf("**************************************************\n");
  printf("*                  TEST STARTS                   *\n");
  printf("**************************************************\n\n");
  test_sort();
  test_stack();
  test_array();
  test_queue();
  test_list_node();
  test_list();
  test_string();
  test_heap();

  // AVL Tree Tests
  printf("\n--- Starting AVL Tree Tests ---\n");
  test_avl_tree_basic_operations();
  test_avl_tree_insertion_balancing();
  test_avl_tree_deletion_balancing();
  test_avl_tree_edge_cases();
  test_avl_tree_random_operations();
  printf("--- AVL Tree Tests Passed ---\n");

  // RB Tree Tests
  printf("\n--- Starting RB Tree Tests ---\n");
  test_rb_tree_basic_operations();
  test_rb_tree_insertion_fixup();
  test_rb_tree_deletion_fixup();
  test_rb_tree_edge_cases();
  test_rb_tree_random_operations();
  printf("--- RB Tree Tests Passed ---\n");

  printf("\n**************************************************\n");
  printf("*                ALL TEST PASSED                 *\n");
  printf("**************************************************\n");
}