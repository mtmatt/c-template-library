#include <stdio.h>
#include "test_hashtable.h"

int main() {
    printf("--- Running Hash Table Test Suite ---\n\n");

    test_hashtable_chaining();
    test_hashtable_linear_probing();
    test_hashtable_quadratic_probing();
    test_hashtable_double_hashing();
    test_hashtable_cuckoo();
    test_hashtable_hopscotch();

    printf("\n--- Hash Table Test Suite Complete ---\n");
    return 0;
}
