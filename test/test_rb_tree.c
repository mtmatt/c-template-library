#include "test_rb_tree.h"
#include "cds/rb_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// --- Helper Functions ---

// Integer comparison function for RB tree
int cmp_int_rb(const void* a, const void* b) {
    int val_a = *(const int*)a;
    int val_b = *(const int*)b;
    if (val_a < val_b) return -1;
    if (val_a > val_b) return 1;
    return 0;
}

// Helper to print test names
static void print_test_name(const char* name) {
    printf("\n--- %s ---\n", name);
}

// --- Test Implementations ---

void test_rb_tree_basic_operations(void) {
    print_test_name("RB Tree: Basic Operations");
    struct cds_rb_tree tree;
    int key1 = 10, val1 = 100;
    int key2 = 20, val2 = 200;
    int key3 = 5, val3 = 50;
    int non_existent_key = 99;
    void* found_val;

    tree = cds_rb_tree_new(sizeof(int), sizeof(int), cmp_int_rb);
    printf("New tree created.\n");
    assert(tree.nil != NULL); // Check if sentinel was allocated
    assert(tree.cmp == cmp_int_rb); // Check if cmp function was set

    assert(cds_rb_tree_empty(&tree));
    printf("Tree is empty (size: %zu).\n", cds_rb_tree_size(&tree));
    assert(cds_rb_tree_size(&tree) == 0);

    // Insert key1
    assert(cds_rb_tree_insert(&tree, &key1, &val1) == 0);
    printf("Inserted key: %d, val: %d. Size: %zu\n", key1, val1, cds_rb_tree_size(&tree));
    assert(!cds_rb_tree_empty(&tree));
    assert(cds_rb_tree_size(&tree) == 1);
    found_val = cds_rb_tree_find(&tree, &key1);
    assert(found_val != NULL && *(int*)found_val == val1);

    // Insert key2
    assert(cds_rb_tree_insert(&tree, &key2, &val2) == 0);
    printf("Inserted key: %d, val: %d. Size: %zu\n", key2, val2, cds_rb_tree_size(&tree));
    assert(cds_rb_tree_size(&tree) == 2);
    found_val = cds_rb_tree_find(&tree, &key2);
    assert(found_val != NULL && *(int*)found_val == val2);

    // Insert key3
    assert(cds_rb_tree_insert(&tree, &key3, &val3) == 0);
    printf("Inserted key: %d, val: %d. Size: %zu\n", key3, val3, cds_rb_tree_size(&tree));
    assert(cds_rb_tree_size(&tree) == 3);
    found_val = cds_rb_tree_find(&tree, &key3);
    assert(found_val != NULL && *(int*)found_val == val3);

    // Find non-existent key
    found_val = cds_rb_tree_find(&tree, &non_existent_key);
    assert(found_val == NULL);
    printf("Find for non-existent key %d: OK (not found).\n", non_existent_key);

    // Remove key1
    assert(cds_rb_tree_remove(&tree, &key1) == 0);
    printf("Removed key: %d. Size: %zu\n", key1, cds_rb_tree_size(&tree));
    assert(cds_rb_tree_size(&tree) == 2);
    found_val = cds_rb_tree_find(&tree, &key1);
    assert(found_val == NULL);

    // Remove key3
    assert(cds_rb_tree_remove(&tree, &key3) == 0);
    printf("Removed key: %d. Size: %zu\n", key3, cds_rb_tree_size(&tree));
    assert(cds_rb_tree_size(&tree) == 1);
    found_val = cds_rb_tree_find(&tree, &key3);
    assert(found_val == NULL);

    // Remove key2
    assert(cds_rb_tree_remove(&tree, &key2) == 0);
    printf("Removed key: %d. Size: %zu\n", key2, cds_rb_tree_size(&tree));
    assert(cds_rb_tree_size(&tree) == 0);
    assert(cds_rb_tree_empty(&tree));
    found_val = cds_rb_tree_find(&tree, &key2);
    assert(found_val == NULL);

    cds_rb_tree_delete(&tree);
    printf("Tree deleted.\n");
    // After delete, tree struct is empty. Root and nil are NULL, size is 0.
    assert(tree.root == NULL);
    assert(tree.nil == NULL); 
    assert(tree.size == 0);
}

void test_rb_tree_insertion_fixup(void) {
    print_test_name("RB Tree: Insertion Fixup Cases");
    struct cds_rb_tree tree;
    
    // Keys to trigger various fixup cases.
    // Actual RB property verification is hard without internal checks.
    // We mainly test if operations complete and basic find works.
    int keys[] = {10, 20, 30, 5, 15, 25, 35, 40, 1, 2, 3, 4, 6, 7, 8, 9};
    int num_keys = sizeof(keys)/sizeof(int);
    int vals[num_keys];
    for(int i=0; i<num_keys; ++i) vals[i] = keys[i] * 10;

    tree = cds_rb_tree_new(sizeof(int), sizeof(int), cmp_int_rb);
    assert(tree.nil != NULL && tree.cmp != NULL);

    printf("Inserting keys to trigger fixup cases...\n");
    for (int i = 0; i < num_keys; ++i) {
        printf("Inserting %d...\n", keys[i]);
        assert(cds_rb_tree_insert(&tree, &keys[i], &vals[i]) == 0);
        assert(cds_rb_tree_size(&tree) == (size_t)(i + 1));
        // Verify all previously inserted keys are still findable
        for (int j = 0; j <= i; ++j) {
            void* val_ptr = cds_rb_tree_find(&tree, &keys[j]);
            assert(val_ptr != NULL);
            assert(*(int*)val_ptr == vals[j]);
        }
        // Here, one might call a cds_rb_tree_is_valid(&tree) if it existed.
    }
    printf("All %d keys inserted. Final size: %zu.\n", num_keys, cds_rb_tree_size(&tree));

    // Simple check: all inserted keys must be findable
    for (int i = 0; i < num_keys; ++i) {
        void* val_ptr = cds_rb_tree_find(&tree, &keys[i]);
        assert(val_ptr != NULL);
        assert(*(int*)val_ptr == vals[i]);
    }
    printf("Insertion fixup tests completed (based on findability and no crashes).\n");
    cds_rb_tree_delete(&tree);
}

void test_rb_tree_deletion_fixup(void) {
    print_test_name("RB Tree: Deletion Fixup Cases");
    struct cds_rb_tree tree;
    
    int keys[] = {10, 5, 20, 3, 7, 15, 25, 1, 4, 6, 8, 12, 17, 22, 27, 30, 2, 11, 13, 16, 18, 21, 23, 26, 28, 29};
    int num_keys = sizeof(keys)/sizeof(int);
    int vals[num_keys];
    bool present[num_keys];

    for(int i=0; i<num_keys; ++i) {
        vals[i] = keys[i] * 10;
        present[i] = true;
    }

    tree = cds_rb_tree_new(sizeof(int), sizeof(int), cmp_int_rb);
    assert(tree.nil != NULL && tree.cmp != NULL);

    printf("Building tree for deletion tests...\n");
    for(int i=0; i<num_keys; ++i) {
        assert(cds_rb_tree_insert(&tree, &keys[i], &vals[i]) == 0);
    }
    printf("Initial tree created. Size: %zu\n", cds_rb_tree_size(&tree));

    // Keys to delete, chosen somewhat to try to trigger various cases
    // (e.g., deleting root, nodes with red/black children, leaves etc.)
    // Order of deletion matters.
    int keys_to_delete_indices[] = {0, 7, 15, 20, num_keys-1, 5, 10, 12, 1, 3}; 
    int num_delete = sizeof(keys_to_delete_indices)/sizeof(int);

    for (int i = 0; i < num_delete; ++i) {
        int key_idx_to_del = keys_to_delete_indices[i];
        int key_del = keys[key_idx_to_del];
        
        if (!present[key_idx_to_del]) continue; // Already deleted by a previous step if indices overlap

        printf("Attempting to remove %d (original index %d)...\n", key_del, key_idx_to_del);
        assert(cds_rb_tree_remove(&tree, &key_del) == 0);
        present[key_idx_to_del] = false;
        printf("Removed %d. Current size: %zu\n", key_del, cds_rb_tree_size(&tree));
        
        // Verify all remaining keys are findable and deleted key is not
        for(int k_idx=0; k_idx<num_keys; ++k_idx) {
            if (present[k_idx]) {
                assert(cds_rb_tree_find(&tree, &keys[k_idx]) != NULL);
            } else {
                assert(cds_rb_tree_find(&tree, &keys[k_idx]) == NULL);
            }
        }
        // Again, cds_rb_tree_is_valid(&tree) would be great here.
    }
    
    printf("Deletion fixup tests completed (based on findability and no crashes).\n");
    cds_rb_tree_delete(&tree);
}

void test_rb_tree_edge_cases(void) {
    print_test_name("RB Tree: Edge Cases");
    struct cds_rb_tree tree;
    int key1 = 10, val1 = 100;
    int key2 = 10, val2 = 200; // For duplicate test, same key
    int non_existent_key = 99;

    // Empty tree operations
    tree = cds_rb_tree_new(sizeof(int), sizeof(int), cmp_int_rb);
    assert(tree.nil != NULL && tree.cmp != NULL);
    printf("Testing on empty tree...\n");
    assert(cds_rb_tree_empty(&tree));
    assert(cds_rb_tree_size(&tree) == 0);
    assert(cds_rb_tree_find(&tree, &key1) == NULL);
    assert(cds_rb_tree_remove(&tree, &key1) == -1); // Remove from empty
    printf("Empty tree operations OK.\n");

    // Single node tree operations
    printf("Testing on single node tree...\n");
    assert(cds_rb_tree_insert(&tree, &key1, &val1) == 0);
    assert(cds_rb_tree_size(&tree) == 1);
    assert(!cds_rb_tree_empty(&tree));
    assert(*(int*)cds_rb_tree_find(&tree, &key1) == val1);

    // Insert duplicate key
    printf("Testing duplicate key insertion...\n");
    assert(cds_rb_tree_insert(&tree, &key2, &val2) == -1); // Duplicate key1, different value
    assert(cds_rb_tree_size(&tree) == 1); // Size should not change
    assert(*(int*)cds_rb_tree_find(&tree, &key1) == val1); // Original value should remain
    printf("Duplicate key insertion OK (not inserted).\n");

    // Remove non-existent key from tree with one node
    printf("Testing removal of non-existent key...\n");
    assert(cds_rb_tree_remove(&tree, &non_existent_key) == -1);
    assert(cds_rb_tree_size(&tree) == 1);
    printf("Removal of non-existent key OK.\n");

    // Remove the single node
    printf("Testing removal of single node...\n");
    assert(cds_rb_tree_remove(&tree, &key1) == 0);
    assert(cds_rb_tree_empty(&tree));
    assert(cds_rb_tree_size(&tree) == 0);
    printf("Removal of single node OK.\n");

    // Test delete on an already empty tree (after operations)
    cds_rb_tree_delete(&tree);
    assert(tree.root == NULL && tree.nil == NULL && tree.size == 0);
    printf("Delete on empty tree OK.\n");

    // Test delete on a tree that was never used beyond new()
    tree = cds_rb_tree_new(sizeof(int), sizeof(int), cmp_int_rb);
    assert(tree.nil != NULL && tree.cmp != NULL);
    cds_rb_tree_delete(&tree);
    assert(tree.root == NULL && tree.nil == NULL && tree.size == 0);
    printf("Delete on fresh tree OK.\n");

    printf("Edge cases test completed.\n");
}

#define MAX_RB_RAND_NODES 1000
#define RB_RAND_OPERATIONS 5000
void test_rb_tree_random_operations(void) {
    print_test_name("RB Tree: Random Operations");
    struct cds_rb_tree tree;
    int keys[MAX_RB_RAND_NODES];
    int vals[MAX_RB_RAND_NODES];
    bool present[MAX_RB_RAND_NODES] = {false};
    int current_size = 0;

    srand((unsigned int)time(NULL));

    tree = cds_rb_tree_new(sizeof(int), sizeof(int), cmp_int_rb);
    assert(tree.nil != NULL && tree.cmp != NULL);

    for (int i = 0; i < MAX_RB_RAND_NODES; ++i) {
        keys[i] = rand() % (MAX_RB_RAND_NODES * 10); // Allow for some key collision potential if not distinct
                                                  // To make it simpler: keys[i] = i;
        vals[i] = keys[i] * 10;
    }
    // For simplicity and to ensure distinct keys for tracking:
    for (int i = 0; i < MAX_RB_RAND_NODES; ++i) keys[i] = i;


    printf("Performing %d random operations on up to %d distinct keys...\n", RB_RAND_OPERATIONS, MAX_RB_RAND_NODES);

    for (int i = 0; i < RB_RAND_OPERATIONS; ++i) {
        int op = rand() % 3; // 0: insert, 1: remove, 2: find
        int key_idx = rand() % MAX_RB_RAND_NODES;
        int current_key = keys[key_idx];
        int current_val = vals[key_idx];

        if (i % (RB_RAND_OPERATIONS/20) == 0) { printf("."); fflush(stdout); }

        if (op == 0) { // Insert
            int res = cds_rb_tree_insert(&tree, &current_key, &current_val);
            if (present[key_idx]) {
                assert(res == -1); 
            } else {
                assert(res == 0);
                present[key_idx] = true;
                current_size++;
            }
        } else if (op == 1) { // Remove
            int res = cds_rb_tree_remove(&tree, &current_key);
            if (present[key_idx]) {
                assert(res == 0);
                present[key_idx] = false;
                current_size--;
            } else {
                assert(res == -1); 
            }
        } else { // Find
            void* found_val = cds_rb_tree_find(&tree, &current_key);
            if (present[key_idx]) {
                assert(found_val != NULL);
                assert(*(int*)found_val == current_val);
            } else {
                assert(found_val == NULL);
            }
        }
        assert(cds_rb_tree_size(&tree) == (size_t)current_size);
        // A cds_rb_tree_is_valid(&tree) call would be very useful here.
    }
    printf("\nRandom operations consistency checks passed.\n");

    // Final check: find all that should be present
    for (int i = 0; i < MAX_RB_RAND_NODES; ++i) {
        void* found_val = cds_rb_tree_find(&tree, &keys[i]);
        if (present[i]) {
            assert(found_val != NULL && *(int*)found_val == vals[i]);
        } else {
            assert(found_val == NULL);
        }
    }
    printf("Final state verification OK.\n");

    cds_rb_tree_delete(&tree);
    printf("Random operations test completed.\n");
}
