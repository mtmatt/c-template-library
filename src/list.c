#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cds/list.h"

struct cds_list_node* cds_list_node_new(void *data, const size_t element_size) {
  struct cds_list_node *new_node = malloc(sizeof(struct cds_list_node));
  if (new_node == NULL) {
    return NULL;
  }
  new_node->data = malloc(element_size);
  if (new_node->data == NULL) {
    free(new_node);
    return NULL;
  }
  memcpy(new_node->data, data, element_size);
  new_node->next = new_node->prev = NULL;
  return new_node;
}

void cds_list_node_delete(struct cds_list_node *node) {
  free(node->data);
  free(node);
}

void cds_list_node_set_data(struct cds_list_node *node, void *data, const size_t element_size) {
  memcpy(node->data, data, element_size);
}

void cds_list_node_set_next(struct cds_list_node *node, struct cds_list_node *next) {
  node->next = next;
}

void cds_list_node_set_prev(struct cds_list_node *node, struct cds_list_node *prev) {
  node->prev = prev;
}


struct cds_list cds_list_new(const size_t element_size) {
  struct cds_list new_list = {
    .head = NULL,
    .tail = NULL,
    .size = 0,
    .element_size = element_size};
  return new_list;
}

void cds_list_delete(struct cds_list *list) {
  struct cds_list_node *head = list->head;
  while (head != NULL) {
    struct cds_list_node *next = head->next;
    cds_list_node_delete(head);
    head = next;
  }
  list->head = list->tail = NULL;
  list->size = list->element_size = 0;
}

int cds_list_push_front(struct cds_list *list, void *new_element) {
  struct cds_list_node *new_node = cds_list_node_new(new_element, list->element_size);
  if (new_node == NULL) {
    return -1;
  }
  new_node->next = list->head;
  if (list->head != NULL) {
    list->head->prev = new_node;
  } else {
    list->tail = new_node;
  }
  list->head = new_node;
  list->size++;
  return 0;
}

int cds_list_push_back(struct cds_list *list, void *new_element) {
  struct cds_list_node *new_node = cds_list_node_new(new_element, list->element_size);
  if (new_node == NULL) {
    return -1;
  }
  new_node->prev = list->tail;
  if (list->tail != NULL) {
    list->tail->next = new_node;
  } else {
    list->head = new_node;
  }
  list->tail = new_node;
  list->size++;
  return 0;
}

int cds_list_pop_front(struct cds_list *list) {
  if (list->size == 0) {
    return -1;
  }
  struct cds_list_node *head = list->head;
  list->head = head->next;
  if (list->head != NULL) {
    list->head->prev = NULL;
  } else {
    list->tail = NULL;
  }
  if (head != NULL) {
    cds_list_node_delete(head);
  }
  list->size--;
  return 0;
}

int cds_list_pop_back(struct cds_list *list) {
  if (list->size == 0) {
    return -1;
  }
  struct cds_list_node *tail = list->tail;
  list->tail = tail->prev;
  if (list->tail != NULL) {
    list->tail->next = NULL;
  } else {
    list->head = NULL;
  }
  if (tail != NULL) {
    cds_list_node_delete(tail);
  }
  list->size--;
  return 0;
}

int cds_list_insert(struct cds_list *list, struct cds_list_node *position, void *new_element) {
  struct cds_list_node *new_node = cds_list_node_new(new_element, list->element_size);
  if (new_node == NULL) {
    return -1;
  }
  new_node->next = position->next;
  new_node->prev = position;
  position->next = new_node;
  if (position == list->tail) {
    list->tail = new_node;
  }
  list->size++;
  return 0;
}

int cds_list_remove(struct cds_list *list, struct cds_list_node *position) {
  if (position == NULL) {
    return -1;
  }
  struct cds_list_node *next = position->next, *prev = position->prev;
  if (next != NULL) {
    next->prev = prev;
  } else {
    list->tail = prev;
  }
  if (prev != NULL) {
    prev->next = next;
  } else {
    list->head = next;
  }
  cds_list_node_delete(position);
  list->size--;
  return 0;
}

int cds_list_concat(struct cds_list *first, struct cds_list *second) {
  if (first == NULL || second == NULL) {
    return -1;
  }
  first->size += second->size;
  if (first->tail != NULL) {
    first->tail->next = second->head;
  }
  if (second->head != NULL) {
    second->head->prev = first->tail;
    first->tail = second->tail;
  }
  second->head = second->tail = NULL;
  cds_list_delete(second);
  return 0;
}

void* cds_list_get_head(struct cds_list *list) {
  if (list->head == NULL) {
    return NULL;
  }
  return list->head->data;
}

void* cds_list_get_tail(struct cds_list *list) {
  if (list->tail == NULL) {
    return NULL;
  }
  return list->tail->data;
}

struct cds_list_node* cds_list_search(struct cds_list *list, void *data, 
    int (*cmp)(const void *, const void *)) {
  if (list == NULL || data == NULL || cmp == NULL) {
    return NULL;
  }
  struct cds_list_node *current = list->head;
  while (current != NULL) {
    if (cmp(current->data, data) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

size_t cds_list_size(struct cds_list *list) {
  return list->size;
}

bool cds_list_empty(struct cds_list *list) {
  return list->size == 0;
}
