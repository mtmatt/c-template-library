#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cds/array.h"
#include "cds/sort.h"
#include "test_sort.h"

static int cmp_int(const void *a, const void *b) {
    int arg1 = *(const int *)a;
    int arg2 = *(const int *)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

void test_sort_int() {
    printf("Testing Sort Int...\n");
    struct cds_array array = cds_array_new(sizeof(int));
    int nums[] = {5, 2, 9, 1, 5, 6};
    for (int i = 0; i < 6; i++) {
        cds_array_push_back(&array, &nums[i]);
    }

    cds_array_sort(&array, cmp_int);

    int *p0 = (int *)cds_array_get(&array, 0);
    int *p1 = (int *)cds_array_get(&array, 1);
    int *p2 = (int *)cds_array_get(&array, 2);
    int *p3 = (int *)cds_array_get(&array, 3);
    int *p4 = (int *)cds_array_get(&array, 4);
    int *p5 = (int *)cds_array_get(&array, 5);

    assert(*p0 == 1);
    assert(*p1 == 2);
    assert(*p2 == 5);
    assert(*p3 == 5);
    assert(*p4 == 6);
    assert(*p5 == 9);

    cds_array_delete(&array);
    printf("Sort Int Passed\n");
}

void test_search_int() {
    printf("Testing Search Int...\n");
    struct cds_array array = cds_array_new(sizeof(int));
    int nums[] = {1, 2, 5, 5, 6, 9};
    for (int i = 0; i < 6; i++) {
        cds_array_push_back(&array, &nums[i]);
    }

    int key = 6;
    int *res = (int *)cds_array_search_linear(&array, &key, cmp_int);
    assert(res != NULL && *res == 6);

    res = (int *)cds_array_search_binary(&array, &key, cmp_int);
    assert(res != NULL && *res == 6);

    key = 10;
    res = (int *)cds_array_search_linear(&array, &key, cmp_int);
    assert(res == NULL);

    res = (int *)cds_array_search_binary(&array, &key, cmp_int);
    assert(res == NULL);

    cds_array_delete(&array);
    printf("Search Int Passed\n");
}

void test_large_array() {
    printf("Testing Large Array...\n");
    struct cds_array array = cds_array_new(sizeof(int));
    for (int i = 1000; i > 0; i--) {
        cds_array_push_back(&array, &i);
    }

    cds_array_sort(&array, cmp_int);

    for (int i = 0; i < 1000; i++) {
        int *val = (int *)cds_array_get(&array, i);
        assert(*val == i + 1);
    }
    cds_array_delete(&array);
    printf("Large Array Passed\n");
}

void test_sort(void) {
    test_sort_int();
    test_search_int();
    test_large_array();
}
