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

struct cds_string cds_string_from(char *data, size_t length);

void cds_string_move(struct cds_string *first,  struct cds_string *second);

void cds_string_delete(struct cds_string *string) {
  if (string->data != NULL) {
    free(string->data);
  }
  memset(string->opt_data, 0, CDS_STRING_OPT_CAPACITY);
  string->size = string->capacity = 0;
}
 
char* cds_string_get(struct cds_string *string) {
  if (string->capacity != CDS_STRING_OPT_CAPACITY) {
    return string->data;
  }
  return string->opt_data;
}

size_t cds_string_size(const struct cds_string *string);

bool cds_string_empty(const struct cds_string *string);

struct cds_array cds_string_split(const struct cds_string *string, char delimiters[]);

int cds_string_concat(struct cds_string *first, const struct cds_string *second);
