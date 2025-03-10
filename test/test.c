#include <stdio.h>

#include "test_array.h"
#include "test_queue.h"
#include "test_stack.h"
#include "test_list.h"
#include "test_string.h"
#include "test_heap.h"

int main(void) {
  printf("**************************************************\n");
  printf("*                  TEST STARTS                   *\n");
  printf("**************************************************\n\n");
  test_stack();
  test_array();
  test_queue();
  test_list_node();
  test_list();
  test_string();
  test_heap();
  printf("**************************************************\n");
  printf("*                ALL TEST PASSED                 *\n");
  printf("**************************************************\n");
}