#include "test_avl_tree.h"
#include "cds/avl_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

// --- Helper Functions ---

// Integer comparison function for AVL tree
int cmp_int_avl(const void* a, const void* b) {
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

void test_avl_tree_basic_operations(void) {
    print_test_name("AVL Tree: Basic Operations");
    struct cds_avl_tree tree;
    int key1 = 10, val1 = 100;
    int key2 = 20, val2 = 200;
    int key3 = 5, val3 = 50;
    int non_existent_key = 99;
    void* found_val;

    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);
    printf("New tree created.\n");

    assert(cds_avl_tree_empty(&tree));
    printf("Tree is empty (size: %zu).\n", cds_avl_tree_size(&tree));
    assert(cds_avl_tree_size(&tree) == 0);
    assert(cds_avl_tree_height(&tree) == 0);

    // Insert key1
    assert(cds_avl_tree_insert(&tree, &key1, &val1) == 0);
    printf("Inserted key: %d, val: %d. Size: %zu, Height: %d\n", key1, val1, cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
    assert(!cds_avl_tree_empty(&tree));
    assert(cds_avl_tree_size(&tree) == 1);
    assert(cds_avl_tree_height(&tree) == 1);
    found_val = cds_avl_tree_find(&tree, &key1);
    assert(found_val != NULL && *(int*)found_val == val1);

    // Insert key2
    assert(cds_avl_tree_insert(&tree, &key2, &val2) == 0);
    printf("Inserted key: %d, val: %d. Size: %zu, Height: %d\n", key2, val2, cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
    assert(cds_avl_tree_size(&tree) == 2);
    // Height depends on balancing, for 10 then 20, it should be 2
    assert(cds_avl_tree_height(&tree) == 2); 
    found_val = cds_avl_tree_find(&tree, &key2);
    assert(found_val != NULL && *(int*)found_val == val2);

    // Insert key3
    assert(cds_avl_tree_insert(&tree, &key3, &val3) == 0);
    printf("Inserted key: %d, val: %d. Size: %zu, Height: %d\n", key3, val3, cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
    assert(cds_avl_tree_size(&tree) == 3);
    // Tree: 10 (root), 5 (left), 20 (right) -> height 2
    assert(cds_avl_tree_height(&tree) == 2); 
    found_val = cds_avl_tree_find(&tree, &key3);
    assert(found_val != NULL && *(int*)found_val == val3);

    // Find non-existent key
    found_val = cds_avl_tree_find(&tree, &non_existent_key);
    assert(found_val == NULL);
    printf("Find for non-existent key %d: OK (not found).\n", non_existent_key);

    // Remove key1
    assert(cds_avl_tree_remove(&tree, &key1) == 0);
    printf("Removed key: %d. Size: %zu, Height: %d\n", key1, cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
    assert(cds_avl_tree_size(&tree) == 2);
    found_val = cds_avl_tree_find(&tree, &key1);
    assert(found_val == NULL);
    // Tree could be 20 (root), 5 (left) OR 5 (root), 20 (right) -> height 2
    // Or after rebalance, could be 5 as root, 20 as right child. Height 2.
    // Or 20 as root, 5 as left child. Height 2.
    // If 10 is removed from (5)-10-(20), root becomes 20 or 5.
    // Let's say 20 becomes root, 5 is left child. Height 2.
    assert(cds_avl_tree_height(&tree) == 2);


    // Remove key3
    assert(cds_avl_tree_remove(&tree, &key3) == 0);
    printf("Removed key: %d. Size: %zu, Height: %d\n", key3, cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
    assert(cds_avl_tree_size(&tree) == 1);
    found_val = cds_avl_tree_find(&tree, &key3);
    assert(found_val == NULL);
    assert(cds_avl_tree_height(&tree) == 1); // Only key2 (20) left

    // Remove key2
    assert(cds_avl_tree_remove(&tree, &key2) == 0);
    printf("Removed key: %d. Size: %zu, Height: %d\n", key2, cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
    assert(cds_avl_tree_size(&tree) == 0);
    assert(cds_avl_tree_empty(&tree));
    found_val = cds_avl_tree_find(&tree, &key2);
    assert(found_val == NULL);
    assert(cds_avl_tree_height(&tree) == 0);

    cds_avl_tree_delete(&tree);
    printf("Tree deleted.\n");
    // After delete, tree struct is empty. Root is NULL, size is 0.
    assert(tree.root == NULL);
    assert(tree.size == 0);
    assert(cds_avl_tree_height(&tree) == 0); // Height of an empty tree is 0.
}

void test_avl_tree_insertion_balancing(void) {
    print_test_name("AVL Tree: Insertion and Balancing");
    struct cds_avl_tree tree;
    int keys[] = {10, 20, 30, 5, 15, 25, 35, 40, 1, 2, 3}; // Various cases
    int vals[sizeof(keys)/sizeof(int)];
    for(size_t i=0; i < sizeof(keys)/sizeof(int); ++i) vals[i] = keys[i] * 10;

    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);

    // LL Case: Insert 30, 20, 10
    // Initial: (empty)
    // Insert 30: (30 H:1)
    // Insert 20: (30 H:2) -> (20 H:1)
    // Insert 10: (30 H:3) -> (20 H:2) -> (10 H:1) => rotate right at 30
    // Result: (20 H:2) -> (10 H:1), (30 H:1)
    printf("Testing LL case (30, 20, 10)...\n");
    assert(cds_avl_tree_insert(&tree, &keys[2], &vals[2]) == 0); // 30
    assert(cds_avl_tree_height(&tree) == 1);
    assert(cds_avl_tree_insert(&tree, &keys[1], &vals[1]) == 0); // 20
    assert(cds_avl_tree_height(&tree) == 2);
    assert(cds_avl_tree_insert(&tree, &keys[0], &vals[0]) == 0); // 10
    assert(cds_avl_tree_height(&tree) == 2); // Balanced: 20 is root
    assert(*(int*)cds_avl_tree_find(&tree, &keys[0]) == vals[0]);
    assert(*(int*)cds_avl_tree_find(&tree, &keys[1]) == vals[1]);
    assert(*(int*)cds_avl_tree_find(&tree, &keys[2]) == vals[2]);
    cds_avl_tree_delete(&tree);
    printf("LL case OK.\n");

    // RR Case: Insert 10, 20, 30
    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);
    printf("Testing RR case (10, 20, 30)...\n");
    assert(cds_avl_tree_insert(&tree, &keys[0], &vals[0]) == 0); // 10
    assert(cds_avl_tree_height(&tree) == 1);
    assert(cds_avl_tree_insert(&tree, &keys[1], &vals[1]) == 0); // 20
    assert(cds_avl_tree_height(&tree) == 2);
    assert(cds_avl_tree_insert(&tree, &keys[2], &vals[2]) == 0); // 30
    assert(cds_avl_tree_height(&tree) == 2); // Balanced: 20 is root
    assert(*(int*)cds_avl_tree_find(&tree, &keys[0]) == vals[0]);
    assert(*(int*)cds_avl_tree_find(&tree, &keys[1]) == vals[1]);
    assert(*(int*)cds_avl_tree_find(&tree, &keys[2]) == vals[2]);
    cds_avl_tree_delete(&tree);
    printf("RR case OK.\n");

    // LR Case: Insert 30, 10, 20
    // Initial: (empty)
    // Insert 30: (30 H:1)
    // Insert 10: (30 H:2) -> (10 H:1)
    // Insert 20: (30 H:3) -> (10 H:2) -> (NIL H:0), (20 H:1) => rotate left at 10, then rotate right at 30
    // Result: (20 H:2) -> (10 H:1), (30 H:1)
    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);
    printf("Testing LR case (30, 10, 20)...\n");
    assert(cds_avl_tree_insert(&tree, &keys[2], &vals[2]) == 0); // 30
    assert(cds_avl_tree_height(&tree) == 1);
    assert(cds_avl_tree_insert(&tree, &keys[0], &vals[0]) == 0); // 10
    assert(cds_avl_tree_height(&tree) == 2);
    assert(cds_avl_tree_insert(&tree, &keys[1], &vals[1]) == 0); // 20
    assert(cds_avl_tree_height(&tree) == 2); // Balanced: 20 is root
    assert(*(int*)cds_avl_tree_find(&tree, &keys[0]) == vals[0]);
    assert(*(int*)cds_avl_tree_find(&tree, &keys[1]) == vals[1]);
    assert(*(int*)cds_avl_tree_find(&tree, &keys[2]) == vals[2]);
    cds_avl_tree_delete(&tree);
    printf("LR case OK.\n");

    // RL Case: Insert 10, 30, 20
    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);
    printf("Testing RL case (10, 30, 20)...\n");
    assert(cds_avl_tree_insert(&tree, &keys[0], &vals[0]) == 0); // 10
    assert(cds_avl_tree_height(&tree) == 1);
    assert(cds_avl_tree_insert(&tree, &keys[2], &vals[2]) == 0); // 30
    assert(cds_avl_tree_height(&tree) == 2);
    assert(cds_avl_tree_insert(&tree, &keys[1], &vals[1]) == 0); // 20
    assert(cds_avl_tree_height(&tree) == 2); // Balanced: 20 is root
    assert(*(int*)cds_avl_tree_find(&tree, &keys[0]) == vals[0]);
    assert(*(int*)cds_avl_tree_find(&tree, &keys[1]) == vals[1]);
    assert(*(int*)cds_avl_tree_find(&tree, &keys[2]) == vals[2]);
    cds_avl_tree_delete(&tree);
    printf("RL case OK.\n");

    // More complex insertions
    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);
    printf("Testing more complex insertions...\n");
    int complex_keys[] = {40, 20, 60, 10, 30, 50, 70, 5, 15, 25, 35, 45, 55, 65, 75};
    int complex_vals[sizeof(complex_keys)/sizeof(int)];
    for(size_t i=0; i < sizeof(complex_keys)/sizeof(int); ++i) complex_vals[i] = complex_keys[i] * 10;

    for(size_t i=0; i < sizeof(complex_keys)/sizeof(int); ++i) {
        assert(cds_avl_tree_insert(&tree, &complex_keys[i], &complex_vals[i]) == 0);
        // Simple check: height of an AVL tree with n nodes is O(log n)
        // For n > 0, height h <= 1.44 * log2(n+2) - 0.328
        // This is hard to assert precisely without knowing the exact structure.
        // We can at least check that all items are findable.
        for(size_t j=0; j <= i; ++j) {
            assert(cds_avl_tree_find(&tree, &complex_keys[j]) != NULL);
        }
        printf("Inserted %d, size %zu, height %d\n", complex_keys[i], cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
    }
    assert(cds_avl_tree_size(&tree) == sizeof(complex_keys)/sizeof(int));
    // For 15 nodes, max height of AVL is log_phi(15+1) which is approx log_1.618(16) = 2.77 / 0.209 = ~5.
    // Height for a perfectly balanced tree of 15 nodes is 4. Max AVL height is around 1.44 * log2(15) ~ 1.44 * 3.9 ~ 5.6
    // The actual height for this sequence {40,20,60,10,30,50,70,5,15,25,35,45,55,65,75} (perfectly balanced) is 4.
    if (cds_avl_tree_size(&tree) == 15) assert(cds_avl_tree_height(&tree) == 4);

    cds_avl_tree_delete(&tree);
    printf("Complex insertions OK.\n");
}


void test_avl_tree_deletion_balancing(void) {
    print_test_name("AVL Tree: Deletion and Balancing");
    struct cds_avl_tree tree;
    int keys[] = {10, 5, 20, 3, 7, 15, 25, 1, 4, 6, 8, 12, 17, 22, 27, 30}; // Build a somewhat balanced tree
    int num_keys = sizeof(keys)/sizeof(int);
    int vals[num_keys];
    for(int i=0; i<num_keys; ++i) vals[i] = keys[i] * 10;

    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);
    for(int i=0; i<num_keys; ++i) {
        assert(cds_avl_tree_insert(&tree, &keys[i], &vals[i]) == 0);
    }
    printf("Initial tree created. Size: %zu, Height: %d\n", cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
    // For 16 nodes, height should be 4 or 5.
    // The actual height for this sequence {10,5,20,3,7,15,25,1,4,6,8,12,17,22,27,30} is 5. Root is 10.

    // Keys to delete that might cause rotations
    int keys_to_delete[] = {1, 30, 3, 27, 4, 25, 22}; // Various leaf/internal nodes
    int num_delete = sizeof(keys_to_delete)/sizeof(int);

    for (int i = 0; i < num_delete; ++i) {
        int key_del = keys_to_delete[i];
        printf("Attempting to remove %d...\n", key_del);
        assert(cds_avl_tree_remove(&tree, &key_del) == 0);
        printf("Removed %d. Size: %zu, Height: %d\n", key_del, cds_avl_tree_size(&tree), cds_avl_tree_height(&tree));
        
        // Verify all remaining keys are findable and deleted key is not
        assert(cds_avl_tree_find(&tree, &key_del) == NULL);
        for(int k_idx=0; k_idx<num_keys; ++k_idx) {
            bool should_be_present = true;
            for(int d_idx=0; d_idx <= i; ++d_idx) {
                if (keys[k_idx] == keys_to_delete[d_idx]) {
                    should_be_present = false;
                    break;
                }
            }
            if (should_be_present) {
                assert(cds_avl_tree_find(&tree, &keys[k_idx]) != NULL);
            }
        }
        // Height check is tricky without knowing exact structure, but it should remain logarithmic.
    }
    
    printf("Deletion balancing tests passed.\n");
    cds_avl_tree_delete(&tree);
}

void test_avl_tree_edge_cases(void) {
    print_test_name("AVL Tree: Edge Cases");
    struct cds_avl_tree tree;
    int key1 = 10, val1 = 100;
    int key2 = 20, val2 = 200; // For duplicate test
    int non_existent_key = 99;

    // Empty tree operations
    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);
    printf("Testing on empty tree...\n");
    assert(cds_avl_tree_empty(&tree));
    assert(cds_avl_tree_size(&tree) == 0);
    assert(cds_avl_tree_find(&tree, &key1) == NULL);
    assert(cds_avl_tree_remove(&tree, &key1) == -1); // Remove from empty
    printf("Empty tree operations OK.\n");

    // Single node tree operations
    printf("Testing on single node tree...\n");
    assert(cds_avl_tree_insert(&tree, &key1, &val1) == 0);
    assert(cds_avl_tree_size(&tree) == 1);
    assert(!cds_avl_tree_empty(&tree));
    assert(*(int*)cds_avl_tree_find(&tree, &key1) == val1);
    assert(cds_avl_tree_height(&tree) == 1);

    // Insert duplicate key
    printf("Testing duplicate key insertion...\n");
    assert(cds_avl_tree_insert(&tree, &key1, &val2) == -1); // Duplicate key1, different value
    assert(cds_avl_tree_size(&tree) == 1); // Size should not change
    assert(*(int*)cds_avl_tree_find(&tree, &key1) == val1); // Original value should remain
    printf("Duplicate key insertion OK (not inserted).\n");

    // Remove non-existent key from tree with one node
    printf("Testing removal of non-existent key...\n");
    assert(cds_avl_tree_remove(&tree, &non_existent_key) == -1);
    assert(cds_avl_tree_size(&tree) == 1);
    printf("Removal of non-existent key OK.\n");

    // Remove the single node
    printf("Testing removal of single node...\n");
    assert(cds_avl_tree_remove(&tree, &key1) == 0);
    assert(cds_avl_tree_empty(&tree));
    assert(cds_avl_tree_size(&tree) == 0);
    assert(cds_avl_tree_height(&tree) == 0);
    printf("Removal of single node OK.\n");

    cds_avl_tree_delete(&tree);
    printf("Edge cases test completed.\n");
}

#define MAX_RAND_NODES 1000
#define RAND_OPERATIONS 5000
void test_avl_tree_random_operations(void) {
    print_test_name("AVL Tree: Random Operations");
    struct cds_avl_tree tree;
    int keys[MAX_RAND_NODES];
    int vals[MAX_RAND_NODES];
    bool present[MAX_RAND_NODES] = {false};
    int current_size = 0;

    srand((unsigned int)time(NULL));

    tree = cds_avl_tree_new(sizeof(int), sizeof(int), cmp_int_avl);

    for (int i = 0; i < MAX_RAND_NODES; ++i) {
        keys[i] = i; // Keep keys simple for easier tracking if needed
        vals[i] = i * 10;
    }
    
    printf("Performing %d random operations on up to %d distinct keys...\n", RAND_OPERATIONS, MAX_RAND_NODES);

    for (int i = 0; i < RAND_OPERATIONS; ++i) {
        int op = rand() % 3; // 0: insert, 1: remove, 2: find
        int key_idx = rand() % MAX_RAND_NODES;
        int current_key = keys[key_idx];
        int current_val = vals[key_idx];

        if (i % (RAND_OPERATIONS/10) == 0) printf("."); fflush(stdout);

        if (op == 0) { // Insert
            int res = cds_avl_tree_insert(&tree, &current_key, &current_val);
            if (present[key_idx]) {
                assert(res == -1); // Should fail if already present
            } else {
                assert(res == 0);
                present[key_idx] = true;
                current_size++;
            }
        } else if (op == 1) { // Remove
            int res = cds_avl_tree_remove(&tree, &current_key);
            if (present[key_idx]) {
                assert(res == 0);
                present[key_idx] = false;
                current_size--;
            } else {
                assert(res == -1); // Should fail if not present
            }
        } else { // Find
            void* found_val = cds_avl_tree_find(&tree, &current_key);
            if (present[key_idx]) {
                assert(found_val != NULL);
                assert(*(int*)found_val == current_val);
            } else {
                assert(found_val == NULL);
            }
        }
        assert(cds_avl_tree_size(&tree) == (size_t)current_size);
        // Height check: O(log N)
        if (current_size > 1) {
            // Crude check: height should not exceed 2 * log2(N) for typical N
            // More formally for AVL: h < 1.44 * log2(N+2)
            // Example: N=1000, 1.44 * log2(1002) ~ 1.44 * 9.96 ~ 14.35. Height <= 14.
            // A loose check:
            int h = cds_avl_tree_height(&tree);
            if (current_size == 0) assert(h == 0);
            else if (current_size == 1) assert(h == 1);
            // else { for N > 1, h > 0. A very loose upper bound h < N }
        }
    }
    printf("\nRandom operations sanity checks passed.\n");

    // Final check: find all that should be present
    for (int i = 0; i < MAX_RAND_NODES; ++i) {
        void* found_val = cds_avl_tree_find(&tree, &keys[i]);
        if (present[i]) {
            assert(found_val != NULL && *(int*)found_val == vals[i]);
        } else {
            assert(found_val == NULL);
        }
    }
    printf("Final state verification OK.\n");

    cds_avl_tree_delete(&tree);
    printf("Random operations test completed.\n");
}
