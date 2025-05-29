#include <assert.h>
#include <stdint.h>

#include <cds/array.h>
#include <cds/util.h>
#include "test_array.h"
#include <string.h>
#include <stdlib.h>

// Comparison function for integers
static int compare_int(const void *a, const void *b) {
  int int_a = *(const int *)a;
  int int_b = *(const int *)b;
  if (int_a < int_b) return -1;
  if (int_a > int_b) return 1;
  return 0;
}

// Comparison function for strings (char*)
static int compare_string(const void *a, const void *b) {
  const char *str_a = *(const char **)a;
  const char *str_b = *(const char **)b;
  return strcmp(str_a, str_b);
}

void test_array() {
  struct cds_array array = cds_array_new(sizeof(int32_t));
  for (int32_t i = 0; i < 10; ++i) {
    cds_array_push_back(&array, &i);
  }
  for (int32_t i = 0; i < 10; ++i) {
    assert(CONV(int32_t) cds_array_at(&array, i) == i);
  }
  for (int32_t i = 0; i < 10; ++i) {
    CONV(int32_t) cds_array_at(&array, i) = 100;
  }
  assert(cds_array_size(&array) == 10);
  for (int32_t i = 0; i < 10; ++i) {
    assert(CONV(int32_t) cds_array_at(&array, i) == 100);
  }
  cds_array_pop_back(&array);
  assert(cds_array_at(&array, 9) == NULL);
  cds_array_delete(&array);
  assert(array.data == NULL);
  assert(array.size == 0);
  assert(array.capacity == 0);
  assert(array.element_size == 0);
}

// Test suite for cds_array_sort
void test_array_sort() {
  // Test case 1: Sorting an empty array
  {
    struct cds_array array = cds_array_new(sizeof(int));
    int result = cds_array_sort(&array, compare_int);
    assert(result == 0); // Should return 0 for success
    assert(cds_array_size(&array) == 0);
    cds_array_delete(&array);
  }

  // Test case 2: Sorting an array with a single element
  {
    struct cds_array array = cds_array_new(sizeof(int));
    int val = 42;
    cds_array_push_back(&array, &val);
    int result = cds_array_sort(&array, compare_int);
    assert(result == 0);
    assert(cds_array_size(&array) == 1);
    assert(*(int *)cds_array_at(&array, 0) == 42);
    cds_array_delete(&array);
  }

  // Test case 3: Sorting an already sorted array
  {
    struct cds_array array = cds_array_new(sizeof(int));
    for (int i = 0; i < 10; ++i) {
      cds_array_push_back(&array, &i);
    }
    int result = cds_array_sort(&array, compare_int);
    assert(result == 0);
    for (int i = 0; i < 10; ++i) {
      assert(*(int *)cds_array_at(&array, i) == i);
    }
    cds_array_delete(&array);
  }

  // Test case 4: Sorting a reverse sorted array
  {
    struct cds_array array = cds_array_new(sizeof(int));
    for (int i = 9; i >= 0; --i) {
      cds_array_push_back(&array, &i);
    }
    int result = cds_array_sort(&array, compare_int);
    assert(result == 0);
    for (int i = 0; i < 10; ++i) {
      assert(*(int *)cds_array_at(&array, i) == i);
    }
    cds_array_delete(&array);
  }

  // Test case 5: Sorting an array with duplicate elements
  {
    struct cds_array array = cds_array_new(sizeof(int));
    int values[] = {5, 2, 8, 2, 5, 5, 1, 8, 9, 0};
    int sorted_values[] = {0, 1, 2, 2, 5, 5, 5, 8, 8, 9};
    for (size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
      cds_array_push_back(&array, &values[i]);
    }
    int result = cds_array_sort(&array, compare_int);
    assert(result == 0);
    for (size_t i = 0; i < cds_array_size(&array); ++i) {
      assert(*(int *)cds_array_at(&array, i) == sorted_values[i]);
    }
    cds_array_delete(&array);
  }

  // Test case 6: Sorting an array with random integer elements
  {
    struct cds_array array = cds_array_new(sizeof(int));
    int n = 20; // Number of random elements
    for (int i = 0; i < n; ++i) {
      int val = rand() % 1000; // Random integers between 0 and 999
      cds_array_push_back(&array, &val);
    }
    
    // Create a copy for comparing after sort (stdlib qsort for reference)
    struct cds_array array_copy_for_qsort = cds_array_new(sizeof(int));
    for(int i=0; i<n; ++i) {
        cds_array_push_back(&array_copy_for_qsort, cds_array_at(&array, i));
    }

    int result = cds_array_sort(&array, compare_int);
    assert(result == 0);

    // Sort the copy using stdlib qsort to compare
    qsort(array_copy_for_qsort.data, array_copy_for_qsort.size, array_copy_for_qsort.element_size, compare_int);

    for (int i = 0; i < n; ++i) {
      assert(*(int *)cds_array_at(&array, i) == *(int *)cds_array_at(&array_copy_for_qsort, i));
    }
    cds_array_delete(&array);
    cds_array_delete(&array_copy_for_qsort);
  }

  // Test case 7: Sorting an array with string elements
  {
    struct cds_array array = cds_array_new(sizeof(char *)); // Array of char*
    char *values[] = {"banana", "apple", "orange", "grape", "kiwi"};
    char *sorted_values[] = {"apple", "banana", "grape", "kiwi", "orange"};
    int n = sizeof(values) / sizeof(values[0]);

    for (int i = 0; i < n; ++i) {
      cds_array_push_back(&array, &values[i]);
    }

    int result = cds_array_sort(&array, compare_string);
    assert(result == 0);

    for (int i = 0; i < n; ++i) {
      assert(strcmp(*(char **)cds_array_at(&array, i), sorted_values[i]) == 0);
    }
    cds_array_delete(&array);
  }

  // Test case 8: Sorting a large array of integers
  {
    struct cds_array array = cds_array_new(sizeof(int));
    int n = 1000; // Large number of elements
    for (int i = 0; i < n; ++i) {
      int val = rand() % 10000; // Random integers
      cds_array_push_back(&array, &val);
    }

    struct cds_array array_copy_for_qsort = cds_array_new(sizeof(int));
    for(int i=0; i<n; ++i) {
        cds_array_push_back(&array_copy_for_qsort, cds_array_at(&array, i));
    }

    int result = cds_array_sort(&array, compare_int);
    assert(result == 0);

    qsort(array_copy_for_qsort.data, array_copy_for_qsort.size, array_copy_for_qsort.element_size, compare_int);

    for (int i = 0; i < n; ++i) {
      assert(*(int *)cds_array_at(&array, i) == *(int *)cds_array_at(&array_copy_for_qsort, i));
    }
    cds_array_delete(&array);
    cds_array_delete(&array_copy_for_qsort);
  }
}