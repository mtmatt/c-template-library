#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string.h"
#include "array.h"

struct cds_string cds_string_new() {
  struct cds_string new_string = {
    .opt_data = {0},
    .data = NULL,
    .size = 0,
    .capacity = CDS_STRING_OPT_CAPACITY
  };
  return new_string;
}

struct cds_string cds_string_from(const char *data, size_t length) {
  struct cds_string new_string = {0};
  if (length + 1 > CDS_STRING_OPT_CAPACITY) {
    new_string.data = malloc(length + 1);
    memcpy(new_string.data, data, length);
    new_string.data[length] = '\0';
    new_string.size = length;
    new_string.capacity = length + 1;
  } else {
    new_string.size = length;
    new_string.capacity = CDS_STRING_OPT_CAPACITY;
    memcpy(new_string.opt_data, data, length);
    new_string.opt_data[length] = '\0';
  }
  return new_string;
}

void cds_string_move(struct cds_string *first,  struct cds_string *second) {
  if (first->data != NULL) {
    free(first->data);
  }
  first->data = second->data;
  memcpy(first->opt_data, second->opt_data, CDS_STRING_OPT_CAPACITY);
  first->size = second->size;
  first->capacity = second->capacity;
}

void cds_string_delete(struct cds_string *string) {
  if (string->data != NULL) {
    free(string->data);
  }
  memset(string->opt_data, 0, CDS_STRING_OPT_CAPACITY);
  string->size = string->capacity = 0;
}
 
const char* cds_string_get(const struct cds_string *string) {
  if (string->capacity != CDS_STRING_OPT_CAPACITY) {
    return string->data;
  }
  return string->opt_data;
}

size_t cds_string_size(const struct cds_string *string) {
  return string->size;
}

bool cds_string_empty(const struct cds_string *string) {
  return string->size == 0;
}

struct cds_array cds_string_split(const struct cds_string *string, char delimiters[]) {
  struct cds_array result_strings = cds_array_new(sizeof(struct cds_string));
  size_t del_len = 0;
  while (delimiters[del_len] != '\0') ++del_len;
  size_t current_start = 0, current_len = 0;
  for (size_t i = 0; i < string->size; ++i) {
    bool is_del = false;
    for (size_t del_index = 0; del_index < del_len; ++del_index) {
      if (cds_string_get(string)[i] == delimiters[del_index]) {
        is_del = true;
        break;
      }
    }
    if (is_del) {
      struct cds_string tp_string = cds_string_from(cds_string_get(string) + current_start, current_len);
      cds_array_push_back(&result_strings, (void*) &tp_string);
      current_start = i + 1;
      current_len = 0;
    } else {
      current_len++;
    }
  }
  if (current_len > 0) {
    struct cds_string tp_string = cds_string_from(cds_string_get(string) + current_start, current_len);
    cds_array_push_back(&result_strings, (void*) &tp_string);
  }
  return result_strings;
}

int cds_string_concat(struct cds_string *first, const struct cds_string *second) {
  size_t final_capacity = first->capacity;
  while (final_capacity < first->size + second->size + 1) final_capacity <<= 1;
  if (first->capacity == CDS_STRING_OPT_CAPACITY) {
    if (final_capacity == CDS_STRING_OPT_CAPACITY) {
      memcpy(first->opt_data + first->size, cds_string_get(second), second->size);
      first->size += second->size;
      first->opt_data[first->size] = '\0';
    } else {
      first->data = malloc(final_capacity);
      if (first->data == NULL) {
        return -1;
      }
      first->capacity = final_capacity;
      memcpy(first->data, first->opt_data, first->size);
      memset(first->opt_data, 0, CDS_STRING_OPT_CAPACITY);
      memcpy(first->data + first->size, cds_string_get(second), second->size);
      first->size += second->size;
      first->data[first->size] = '\0';
    }
  } else {
    if (first->data == NULL || final_capacity < first->capacity) {
      return -1;
    }
    if (final_capacity != first->capacity) {
      first->data = realloc(first->data, final_capacity);
      if (first->data == NULL) {
        return -1;
      }
      first->capacity = final_capacity;
    }
    memcpy(first->data + first->size, cds_string_get(second), second->size);
    first->size += second->size;
    first->data[first->size] = '\0';
  }
  return 0;
}
