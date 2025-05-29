#include <stdlib.h>
#include <string.h>

#include "cds/array.h"

struct cds_array cds_array_new(const size_t element_size) {
  struct cds_array new_array = {
    .data = (char*) malloc(element_size),
    .size = 0,
    .capacity = 1,
    .element_size = element_size};
  return new_array;
}

void cds_array_delete(struct cds_array *array) {
  if (array->data != NULL) free(array->data);
  array->data = NULL;
  array->size = array->capacity = 0;
  array->element_size = 0;
}

int cds_array_push_back(struct cds_array *array, const void *new_element) {
  if (array->size == array->capacity) {
    array->capacity <<= 1;  // cap * 2
    array->data = (char*) realloc(array->data, array->capacity * array->element_size);
    if (array->data == NULL) {
      return -1;
    }
  }
  memcpy(array->data + array->size * array->element_size, new_element, array->element_size);
  array->size++;
  return 0;
}

int cds_array_pop_back(struct cds_array *array) {
  if (array->size == 0) {
    return -1;
  }
  array->size--;
  return 0;
}

void* cds_array_at(const struct cds_array *array, const size_t index) {
  if (index < 0 || index >= array->size) {
    return NULL;
  }
  return (void*) (array->data + index * array->element_size);
}

void* cds_array_get(const struct cds_array *array, const size_t index) {
  return (void*) (array->data + index * array->element_size);
}

size_t cds_array_size(const struct cds_array *array) {
  return array->size;
}

bool cds_array_empty(const struct cds_array *array) {
  return array->size == 0;
}

static void cds_array_swap(struct cds_array *array, size_t i, size_t j, void *tmp) {
  if (i == j) {
    return;
  }
  memcpy(tmp, cds_array_get(array, i), array->element_size);
  memcpy(cds_array_get(array, i), cds_array_get(array, j), array->element_size);
  memcpy(cds_array_get(array, j), tmp, array->element_size);
}

static void cds_array_insertion_sort(struct cds_array *array, size_t left, size_t right, int (*compare)(const void *, const void *), void *tmp) {
  for (size_t i = left + 1; i <= right; i++) {
    memcpy(tmp, cds_array_get(array, i), array->element_size); // Use tmp as buffer for the key
    size_t j = i - 1;
    // Move elements of array[left..i-1], that are greater than key, to one position ahead of their current position
    while (j >= left && compare(cds_array_get(array, j), tmp) > 0) {
      memcpy(cds_array_get(array, j + 1), cds_array_get(array, j), array->element_size);
      if (j == 0) { // prevent underflow when left is 0
        break;
      }
      j--;
    }
    memcpy(cds_array_get(array, j + 1), tmp, array->element_size);
  }
}

// Public function to sort the array
int cds_array_sort(struct cds_array *array, int (*compare)(const void *, const void *)) {
  if (array == NULL || array->data == NULL || array->size <= 1 || compare == NULL) {
    return 0; // Nothing to sort or invalid input, considered success.
  }

  // Calculate max_depth for Introsort (2 * log2(n))
  size_t n = array->size;
  int max_depth = 0;
  if (n > 0) {
    size_t temp_n = n;
    while (temp_n > 0) {
      temp_n >>= 1;
      max_depth++;
    }
    max_depth *= 2;
  }
  
  // Allocate temporary buffer for swaps
  void *tmp = malloc(array->element_size);
  if (tmp == NULL) {
    // Memory allocation failed
    return -1; 
  }

  cds_array_sort_recursive(array, 0, array->size - 1, compare, max_depth, tmp);

  // Free the temporary buffer
  free(tmp);
  return 0; // Success
}

static void cds_array_sort_recursive(struct cds_array *array, size_t left, size_t right, int (*compare)(const void *, const void *), int max_depth, void *tmp) {
  if (left >= right) {
    return;
  }

  if (right - left + 1 < 16) { // Small arrays: use insertion sort
    cds_array_insertion_sort(array, left, right, compare, tmp);
  } else if (max_depth == 0) { // Max depth reached: use heapsort
    cds_array_heap_sort(array, left, right, compare, tmp);
  } else { // Otherwise: use quicksort
    size_t pivot_index = cds_array_partition(array, left, right, compare, tmp);
    // Correctly handle potential underflow if pivot_index is 0
    if (pivot_index > 0) {
        cds_array_sort_recursive(array, left, pivot_index - 1, compare, max_depth - 1, tmp);
    }
    cds_array_sort_recursive(array, pivot_index + 1, right, compare, max_depth - 1, tmp);
  }
}

static size_t cds_array_partition(struct cds_array *array, size_t left, size_t right, int (*compare)(const void *, const void *), void *tmp) {
  void *pivot = cds_array_get(array, right); // Choose the last element as pivot
  size_t i = left - 1; // Index of smaller element

  for (size_t j = left; j < right; j++) {
    // If current element is smaller than or equal to pivot
    if (compare(cds_array_get(array, j), pivot) <= 0) {
      i++;
      cds_array_swap(array, i, j, tmp);
    }
  }
  cds_array_swap(array, i + 1, right, tmp);
  return i + 1;
}

static void cds_array_heapify(struct cds_array *array, size_t n, size_t i, int (*compare)(const void *, const void *), void *tmp) {
  size_t largest = i;
  size_t left = 2 * i + 1;
  size_t right = 2 * i + 2;

  if (left < n && compare(cds_array_get(array, left), cds_array_get(array, largest)) > 0) {
    largest = left;
  }

  if (right < n && compare(cds_array_get(array, right), cds_array_get(array, largest)) > 0) {
    largest = right;
  }

  if (largest != i) {
    cds_array_swap(array, i, largest, tmp);
    cds_array_heapify(array, n, largest, compare, tmp);
  }
}

static void cds_array_heap_sort(struct cds_array *array, size_t left, size_t right, int (*compare)(const void *, const void *), void *tmp) {
  size_t n = right - left + 1;
  // Build heap (rearrange array)
  for (size_t i = n / 2 - 1; ; i--) {
    cds_array_heapify(array, n, i, compare, tmp);
    if (i == 0) { // prevent underflow
      break;
    }
  }

  // One by one extract an element from heap
  for (size_t i = n - 1; ; i--) {
    cds_array_swap(array, left, left + i, tmp); // Move current root to end
    cds_array_heapify(array, i, 0, compare, tmp); // call max heapify on the reduced heap
    if (i == 0) { // prevent underflow
      break;
    }
  }
}