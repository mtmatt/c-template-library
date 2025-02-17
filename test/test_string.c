#include <assert.h>
#include <string.h>

#include <cds/string.h>
#include <cds/util.h>
#include "test_string.h"

void test_string() {
  struct cds_string str1 = cds_string_new();
  assert(cds_string_size(&str1) == 0);
  assert(cds_string_empty(&str1) == true);

  const char *data = "Hello, World!";
  struct cds_string str2 = cds_string_from(data, strlen(data));
  assert(cds_string_size(&str2) == strlen(data));
  assert(strcmp(cds_string_get(&str2), data) == 0);

  struct cds_string str3 = cds_string_from(" Goodbye!", 9);
  assert(cds_string_concat(&str2, &str3) == 0);
  assert(strcmp(cds_string_get(&str2), "Hello, World! Goodbye!") == 0);
  
  struct cds_string str4 = cds_string_from("Goodbye", 7);
  assert(strcmp(cds_string_get(&str4), "Goodbye") == 0);
  str4 = cds_string_from("edgecase", 8);
  assert(strcmp(cds_string_get(&str4), "edgecase") == 0);

  char *delimiters = " ,!";
  struct cds_array split_array = cds_string_split(&str2, delimiters);
  assert(split_array.size == 5);
  
  struct cds_string *split_str = (struct cds_string *) cds_array_get(&split_array, 0);
  assert(strcmp(cds_string_get(split_str), "Hello") == 0);
  split_str = (struct cds_string *) cds_array_get(&split_array, 1);
  assert(strcmp(cds_string_get(split_str), "") == 0);
  split_str = (struct cds_string *) cds_array_get(&split_array, 2);
  assert(strcmp(cds_string_get(split_str), "World") == 0);
  split_str = (struct cds_string *) cds_array_get(&split_array, 3);
  assert(strcmp(cds_string_get(split_str), "") == 0);
  split_str = (struct cds_string *) cds_array_get(&split_array, 4);
  assert(strcmp(cds_string_get(split_str), "Goodbye") == 0);

  cds_string_delete(&str1);
  cds_string_delete(&str2);
  cds_string_delete(&str3);
  for (size_t i = 0; i < split_array.size; ++i) {
    split_str = (struct cds_string *)cds_array_get(&split_array, i);
    cds_string_delete(split_str);
  }
  cds_array_delete(&split_array);
}
