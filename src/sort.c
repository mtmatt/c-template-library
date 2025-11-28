#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cds/sort.h"
#include "cds/array.h"

static void swap(void *a, void *b, size_t size) {
    char buffer[1024];
    if (size <= 1024) {
        memcpy(buffer, a, size);
        memcpy(a, b, size);
        memcpy(b, buffer, size);
    } else {
        char *temp = malloc(size);
        if (temp) {
            memcpy(temp, a, size);
            memcpy(a, b, size);
            memcpy(b, temp, size);
            free(temp);
        }
    }
}

static void insertion_sort(char *data, size_t n, size_t element_size, cds_cmp_func cmp) {
    for (size_t i = 1; i < n; i++) {
        char *key = malloc(element_size);
        if (!key) return; // Allocation failure

        memcpy(key, data + i * element_size, element_size);
        int j = i - 1;

        while (j >= 0 && cmp(data + j * element_size, key) > 0) {
            memcpy(data + (j + 1) * element_size, data + j * element_size, element_size);
            j = j - 1;
        }
        memcpy(data + (j + 1) * element_size, key, element_size);
        free(key);
    }
}

static void heapify(char *data, size_t n, size_t i, size_t element_size, cds_cmp_func cmp) {
    size_t largest = i;
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;

    if (left < n && cmp(data + left * element_size, data + largest * element_size) > 0)
        largest = left;

    if (right < n && cmp(data + right * element_size, data + largest * element_size) > 0)
        largest = right;

    if (largest != i) {
        swap(data + i * element_size, data + largest * element_size, element_size);
        heapify(data, n, largest, element_size, cmp);
    }
}

static void heapsort(char *data, size_t n, size_t element_size, cds_cmp_func cmp) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(data, n, i, element_size, cmp);

    for (int i = n - 1; i > 0; i--) {
        swap(data, data + i * element_size, element_size);
        heapify(data, i, 0, element_size, cmp);
    }
}

static size_t partition(char *data, int low, int high, size_t element_size, cds_cmp_func cmp) {
    char *pivot = data + high * element_size;
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (cmp(data + j * element_size, pivot) < 0) {
            i++;
            swap(data + i * element_size, data + j * element_size, element_size);
        }
    }
    swap(data + (i + 1) * element_size, data + high * element_size, element_size);
    return (i + 1);
}

static void introsort_util(char *data, int low, int high, int depth_limit, size_t element_size, cds_cmp_func cmp) {
    size_t n = high - low + 1;
    if (n <= 16) {
        insertion_sort(data + low * element_size, n, element_size, cmp);
        return;
    }

    if (depth_limit == 0) {
        heapsort(data + low * element_size, n, element_size, cmp);
        return;
    }

    size_t p = partition(data, low, high, element_size, cmp);
    introsort_util(data, low, p - 1, depth_limit - 1, element_size, cmp);
    introsort_util(data, p + 1, high, depth_limit - 1, element_size, cmp);
}

void cds_array_sort(struct cds_array *array, cds_cmp_func cmp) {
    if (!array || array->size <= 1) return;

    int depth_limit = 2 * log(array->size);
    introsort_util(array->data, 0, array->size - 1, depth_limit, array->element_size, cmp);
}

void *cds_array_search_linear(const struct cds_array *array, const void *key, cds_cmp_func cmp) {
    if (!array || !key || !cmp) return NULL;

    for (size_t i = 0; i < array->size; i++) {
        void *element = cds_array_get(array, i);
        if (cmp(element, key) == 0) {
            return element;
        }
    }
    return NULL;
}

void *cds_array_search_binary(const struct cds_array *array, const void *key, cds_cmp_func cmp) {
    if (!array || !key || !cmp || array->size == 0) return NULL;

    int low = 0;
    int high = array->size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        void *mid_element = cds_array_get(array, mid);
        int result = cmp(mid_element, key);

        if (result == 0) {
            return mid_element;
        }

        if (result < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return NULL;
}
