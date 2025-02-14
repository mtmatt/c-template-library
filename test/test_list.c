#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <cds/list.h>
#include <cds/util.h>
#include "test_list.h"

void test_list_node() {
  int data = 42;
  struct cds_list_node *node = cds_list_node_new(&data, sizeof(data));
  assert(node != NULL);
  assert(CONV(int) node->data == 42);

  int new_data = 84;
  cds_list_node_set_data(node, &new_data, sizeof(new_data));
  assert(CONV(int) node->data == 84);

  struct cds_list_node *next_node = cds_list_node_new(&data, sizeof(data));
  cds_list_node_set_next(node, next_node);
  assert(node->next == next_node);

  struct cds_list_node *prev_node = cds_list_node_new(&data, sizeof(data));
  cds_list_node_set_prev(node, prev_node);
  assert(node->prev == prev_node);

  cds_list_node_delete(node);
  cds_list_node_delete(next_node);
  cds_list_node_delete(prev_node);
}

int cmp(const void *a, const void *b) {
  return CONV(int) a - CONV(int) b;
}

void test_list() {
  struct cds_list list = cds_list_new(sizeof(int));
  assert(list.size == 0);
  assert(list.head == NULL);
  assert(list.tail == NULL);

  int data1 = 1, data2 = 2, data3 = 3;
  assert(cds_list_push_front(&list, &data1) == 0);
  assert(list.size == 1);
  assert(CONV(int) list.head->data == 1);
  assert(list.head == list.tail);

  assert(cds_list_push_back(&list, &data2) == 0);
  assert(list.size == 2);
  assert(CONV(int) list.tail->data == 2);

  assert(cds_list_push_front(&list, &data3) == 0);
  assert(list.size == 3);
  assert(CONV(int) list.head->data == 3);

  assert(cds_list_pop_front(&list) == 0);
  assert(list.size == 2);
  assert(CONV(int) list.head->data == 1);

  assert(cds_list_pop_back(&list) == 0);
  assert(list.size == 1);
  assert(CONV(int) list.tail->data == 1);

  struct cds_list list2 = cds_list_new(sizeof(int));
  assert(cds_list_push_back(&list2, &data2) == 0);
  assert(cds_list_push_back(&list2, &data3) == 0);

  cds_list_concat(&list, &list2);
  assert(list.size == 3);
  assert(CONV(int) list.head->data == 1);
  assert(CONV(int) list.tail->data == 3);

  cds_list_delete(&list2);

  cds_list_delete(&list);
  assert(list.size == 0);
  assert(list.element_size == 0);
  assert(list.head == NULL);
  assert(list.tail == NULL);

  list = cds_list_new(sizeof(int));
  int data4 = 4;
  assert(cds_list_push_back(&list, &data4) == 0);
  assert(list.size == 1);
  assert(CONV(int) list.head->data == 4);
  assert(CONV(int) cds_list_get_head(&list) == 4);
  assert(CONV(int) cds_list_get_tail(&list) == 4);

  int data5 = 5;
  assert(cds_list_push_back(&list, &data5) == 0);
  assert(list.size == 2);
  assert(CONV(int) cds_list_get_head(&list) == 4);
  assert(CONV(int) cds_list_get_tail(&list) == 5);

  struct cds_list_node *found_node = cds_list_search(&list, &data5, cmp);
  assert(found_node != NULL);
  assert(CONV(int) found_node->data == 5);

  found_node = cds_list_search(&list, &data3, cmp);
  assert(found_node == NULL);

  assert(cds_list_size(&list) == 2);
  assert(!cds_list_empty(&list));

  assert(cds_list_pop_front(&list) == 0);
  assert(cds_list_size(&list) == 1);
  assert(CONV(int) cds_list_get_head(&list) == 5);

  assert(cds_list_pop_back(&list) == 0);
  assert(cds_list_size(&list) == 0);
  assert(cds_list_empty(&list));
  cds_list_delete(&list);
}