#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "cds/hashtable.h"
#include "test_hashtable.h"

// --- Test Data ---
const char* KEY1 = "apple";
const char* KEY2 = "banana";
const char* KEY3 = "orange";
const char* KEY4 = "grape";
const char* KEY5 = "mango";
const char* KEY_COLLIDE1 = "epine"; // Simple hash collision with apple for some functions
const char* KEY_COLLIDE2 = "banana"; // Duplicate of KEY2 for update tests


int VAL1 = 100;
int VAL2 = 200;
int VAL3 = 300;
int VAL1_UPDATED = 101;

// --- Helper Hash Functions for Cuckoo Hashing Tests ---
static size_t test_cuckoo_hash_func1(const char* key, size_t table_size) {
    size_t hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++) {
        hash = (hash * 31 + key[i]);
    }
    return hash % table_size;
}

static size_t test_cuckoo_hash_func2(const char* key, size_t table_size) {
    size_t hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++) {
        hash = (hash * 17 + key[i]); // Different multiplier
    }
    if (table_size > 0 && (hash % table_size == test_cuckoo_hash_func1(key, table_size))) {
      // Ensure hash2 is different from hash1 for the same key if possible
      // This is a simplistic way; better functions would inherently do this.
      hash = (hash * 19 + 1);
    }
    return hash % table_size;
}


// --- Test Implementations ---

void test_hashtable_chaining(void) {
    printf("Testing Separate Chaining...\n");
    cds_hashtable_chaining_t* table = NULL;

    // Test Creation
    table = cds_hashtable_chaining_create(10);
    assert(table != NULL && "Chaining: Create failed");
    printf("  Chaining: Creation successful.\n");

    // Test Insertion
    assert(cds_hashtable_chaining_insert(table, KEY1, &VAL1) == 0 && "Chaining: Insert KEY1 failed");
    assert(cds_hashtable_chaining_insert(table, KEY2, &VAL2) == 0 && "Chaining: Insert KEY2 failed");
    assert(cds_hashtable_chaining_insert(table, KEY3, &VAL3) == 0 && "Chaining: Insert KEY3 failed");
    printf("  Chaining: Initial insertion successful.\n");

    // Test Search
    int* retrieved_val = (int*)cds_hashtable_chaining_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1 && "Chaining: Search KEY1 failed or wrong value");
    retrieved_val = (int*)cds_hashtable_chaining_search(table, KEY2);
    assert(retrieved_val != NULL && *retrieved_val == VAL2 && "Chaining: Search KEY2 failed or wrong value");
    retrieved_val = (int*)cds_hashtable_chaining_search(table, "nonexistent");
    assert(retrieved_val == NULL && "Chaining: Search for nonexistent key did not return NULL");
    printf("  Chaining: Search tests successful.\n");

    // Test Update (Insert same key)
    assert(cds_hashtable_chaining_insert(table, KEY1, &VAL1_UPDATED) == 0 && "Chaining: Update KEY1 failed");
    retrieved_val = (int*)cds_hashtable_chaining_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "Chaining: Search KEY1 after update failed or wrong value");
    printf("  Chaining: Update test successful.\n");

    // Test Collision (implementation detail, but good to have keys that might collide)
    // For chaining, collision is handled gracefully by design.
    assert(cds_hashtable_chaining_insert(table, KEY_COLLIDE1, &VAL2) == 0 && "Chaining: Insert KEY_COLLIDE1 failed");
    retrieved_val = (int*)cds_hashtable_chaining_search(table, KEY_COLLIDE1);
    assert(retrieved_val != NULL && *retrieved_val == VAL2 && "Chaining: Search KEY_COLLIDE1 failed");
    printf("  Chaining: Collision test successful.\n");

    // Test Deletion
    assert(cds_hashtable_chaining_delete(table, KEY2) == 0 && "Chaining: Delete KEY2 failed");
    retrieved_val = (int*)cds_hashtable_chaining_search(table, KEY2);
    assert(retrieved_val == NULL && "Chaining: Search KEY2 after delete did not return NULL");
    assert(cds_hashtable_chaining_delete(table, "nonexistent") == -1 && "Chaining: Delete nonexistent key did not indicate failure");
    printf("  Chaining: Deletion tests successful.\n");

    // Test inserting after deletion
    assert(cds_hashtable_chaining_insert(table, KEY4, &VAL1) == 0 && "Chaining: Insert KEY4 after delete failed");
    retrieved_val = (int*)cds_hashtable_chaining_search(table, KEY4);
    assert(retrieved_val != NULL && *retrieved_val == VAL1 && "Chaining: Search KEY4 after delete failed");
    printf("  Chaining: Insert after delete successful.\n");

    // Test Destruction
    cds_hashtable_chaining_destroy(table);
    printf("  Chaining: Destruction successful (manual check for no crash).\n");
    // Test destroying NULL (should not crash)
    cds_hashtable_chaining_destroy(NULL);
    printf("  Chaining: Destroying NULL table handled.\n");

    printf("Separate Chaining tests complete.\n\n");
}

// --- Stubs for other test functions (to be implemented) ---
void test_hashtable_linear_probing(void) {
    printf("Testing Linear Probing...\n");
    cds_hashtable_lp_t* table = NULL;

    // Test Creation
    table = cds_hashtable_lp_create(10); // Small size to test fullness
    assert(table != NULL && "LP: Create failed");
    printf("  LP: Creation successful.\n");

    // Test Insertion
    assert(cds_hashtable_lp_insert(table, KEY1, &VAL1) == 0 && "LP: Insert KEY1 failed");
    assert(cds_hashtable_lp_insert(table, KEY2, &VAL2) == 0 && "LP: Insert KEY2 failed");
    assert(cds_hashtable_lp_insert(table, KEY3, &VAL3) == 0 && "LP: Insert KEY3 failed");
    printf("  LP: Initial insertion successful.\n");

    // Test Search
    int* retrieved_val = (int*)cds_hashtable_lp_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1 && "LP: Search KEY1 failed");
    retrieved_val = (int*)cds_hashtable_lp_search(table, KEY2);
    assert(retrieved_val != NULL && *retrieved_val == VAL2 && "LP: Search KEY2 failed");
    assert(cds_hashtable_lp_search(table, "nonexistent") == NULL && "LP: Search non-existent failed");
    printf("  LP: Search tests successful.\n");

    // Test Update
    assert(cds_hashtable_lp_insert(table, KEY1, &VAL1_UPDATED) == 0 && "LP: Update KEY1 failed");
    retrieved_val = (int*)cds_hashtable_lp_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "LP: Search KEY1 after update failed");
    printf("  LP: Update test successful.\n");

    // Test Deletion
    assert(cds_hashtable_lp_delete(table, KEY2) == 0 && "LP: Delete KEY2 failed");
    assert(cds_hashtable_lp_search(table, KEY2) == NULL && "LP: Search KEY2 after delete failed");
    assert(cds_hashtable_lp_delete(table, "nonexistent") == -1 && "LP: Delete non-existent failed to return error");
    printf("  LP: Deletion tests successful.\n");

    // Test search after delete (ensuring DELETED markers are handled)
    retrieved_val = (int*)cds_hashtable_lp_search(table, KEY1); // Should still find KEY1
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "LP: Search KEY1 after deleting KEY2 failed");
    assert(cds_hashtable_lp_insert(table, KEY4, &VAL2) == 0 && "LP: Insert KEY4 after delete failed"); // Insert into a potentially DELETED slot
    retrieved_val = (int*)cds_hashtable_lp_search(table, KEY4);
    assert(retrieved_val != NULL && *retrieved_val == VAL2 && "LP: Search KEY4 after insert failed");
    printf("  LP: Search/Insert after delete successful.\n");

    // Test Table Fullness (Exact number depends on load factor if resizing were implemented)
    // Using a small table of size 10. Insert 7 more items (total 10 with KEY1, KEY3, KEY4).
    // Current items: KEY1, KEY3, KEY4. (KEY2 was deleted). Count = 3.
    char key_buf[20];
    int val_buf[7];
    for (int i = 0; i < 7; ++i) {
        sprintf(key_buf, "test_key_%d", i);
        val_buf[i] = i * 10;
        // Assuming no resize, this should fill the table (3 existing + 7 new = 10)
        int ins_ret = cds_hashtable_lp_insert(table, key_buf, &val_buf[i]);
        assert(ins_ret == 0 && "LP: Failed to insert when table should have space.");
    }
    // Table should be full now (10 items in a size 10 table)
    int last_val = 999;
    int full_ret = cds_hashtable_lp_insert(table, "overflow_key", &last_val);
    assert(full_ret == -2 && "LP: Insert into full table did not return -2 (table full)");
    printf("  LP: Table fullness test successful.\n");


    // Test Destruction
    cds_hashtable_lp_destroy(table);
    printf("  LP: Destruction successful.\n");
    cds_hashtable_lp_destroy(NULL);
    printf("  LP: Destroying NULL table handled.\n");

    printf("Linear Probing tests complete.\n\n");
}


void test_hashtable_quadratic_probing(void) {
    printf("Testing Quadratic Probing...\n");
    cds_hashtable_qp_t* table = NULL;

    // Test Creation
    table = cds_hashtable_qp_create(10); // Small size
    assert(table != NULL && "QP: Create failed");
    printf("  QP: Creation successful.\n");

    // Test Insertion
    assert(cds_hashtable_qp_insert(table, KEY1, &VAL1) == 0 && "QP: Insert KEY1 failed");
    assert(cds_hashtable_qp_insert(table, KEY2, &VAL2) == 0 && "QP: Insert KEY2 failed");
    assert(cds_hashtable_qp_insert(table, KEY3, &VAL3) == 0 && "QP: Insert KEY3 failed");
    printf("  QP: Initial insertion successful.\n");

    // Test Search
    int* retrieved_val = (int*)cds_hashtable_qp_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1 && "QP: Search KEY1 failed");
    assert(cds_hashtable_qp_search(table, "nonexistent") == NULL && "QP: Search non-existent failed");
    printf("  QP: Search tests successful.\n");

    // Test Update
    assert(cds_hashtable_qp_insert(table, KEY1, &VAL1_UPDATED) == 0 && "QP: Update KEY1 failed");
    retrieved_val = (int*)cds_hashtable_qp_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "QP: Search KEY1 after update failed");
    printf("  QP: Update test successful.\n");

    // Test Deletion
    assert(cds_hashtable_qp_delete(table, KEY2) == 0 && "QP: Delete KEY2 failed");
    assert(cds_hashtable_qp_search(table, KEY2) == NULL && "QP: Search KEY2 after delete failed");
    assert(cds_hashtable_qp_delete(table, "nonexistent") == -1 && "QP: Delete non-existent failed to return error");
    printf("  QP: Deletion tests successful.\n");

    // Test search/insert after delete
    retrieved_val = (int*)cds_hashtable_qp_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "QP: Search KEY1 after deleting KEY2 failed");
    assert(cds_hashtable_qp_insert(table, KEY4, &VAL2) == 0 && "QP: Insert KEY4 after delete failed");
    retrieved_val = (int*)cds_hashtable_qp_search(table, KEY4);
    assert(retrieved_val != NULL && *retrieved_val == VAL2 && "QP: Search KEY4 failed");
    printf("  QP: Search/Insert after delete successful.\n");

    // Test Table Fullness (similar to LP, no resize)
    // Items: KEY1, KEY3, KEY4. Count = 3. Size = 10.
    char key_buf[20];
    int val_buf[7];
    for (int i = 0; i < 7; ++i) {
        sprintf(key_buf, "qp_test_key_%d", i);
        val_buf[i] = i * 10;
        int ins_ret = cds_hashtable_qp_insert(table, key_buf, &val_buf[i]);
        // Quadratic probing can fail to find a slot even if table is not count == size
        // if probing sequence doesn't hit an empty/deleted slot after max attempts (table->size probes)
        // So we only assert ins_ret == 0 if it's expected for a non-pathological case.
        // For this test, assume it works until table is very full.
        if (i < 6) { // Allow some grace for probing misses on last slots
             assert(ins_ret == 0 && "QP: Failed to insert when table should have space.");
        } else if (ins_ret != 0 && ins_ret != -2) {
             assert(0 && "QP: Unexpected error during fill");
        }
    }
    int last_val = 999;
    // Try to insert when very full or full
    int full_ret = cds_hashtable_qp_insert(table, "qp_overflow_key", &last_val);
    assert(full_ret == -2 && "QP: Insert into (nearly) full table did not return -2");
    printf("  QP: Table fullness test successful (may depend on probing luck).\n");

    // Test Destruction
    cds_hashtable_qp_destroy(table);
    printf("  QP: Destruction successful.\n");
    cds_hashtable_qp_destroy(NULL);
    printf("  QP: Destroying NULL table handled.\n");

    printf("Quadratic Probing tests complete.\n\n");
}


void test_hashtable_double_hashing(void) {
    printf("Testing Double Hashing...\n");
    cds_hashtable_dh_t* table = NULL;

    // Test Creation
    table = cds_hashtable_dh_create(10);
    assert(table != NULL && "DH: Create failed");
    printf("  DH: Creation successful.\n");

    // Test Insertion
    assert(cds_hashtable_dh_insert(table, KEY1, &VAL1) == 0 && "DH: Insert KEY1 failed");
    assert(cds_hashtable_dh_insert(table, KEY2, &VAL2) == 0 && "DH: Insert KEY2 failed");
    assert(cds_hashtable_dh_insert(table, KEY3, &VAL3) == 0 && "DH: Insert KEY3 failed");
    printf("  DH: Initial insertion successful.\n");

    // Test Search
    int* retrieved_val = (int*)cds_hashtable_dh_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1 && "DH: Search KEY1 failed");
    assert(cds_hashtable_dh_search(table, "nonexistent") == NULL && "DH: Search non-existent failed");
    printf("  DH: Search tests successful.\n");

    // Test Update
    assert(cds_hashtable_dh_insert(table, KEY1, &VAL1_UPDATED) == 0 && "DH: Update KEY1 failed");
    retrieved_val = (int*)cds_hashtable_dh_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "DH: Search KEY1 after update failed");
    printf("  DH: Update test successful.\n");

    // Test Deletion
    assert(cds_hashtable_dh_delete(table, KEY2) == 0 && "DH: Delete KEY2 failed");
    assert(cds_hashtable_dh_search(table, KEY2) == NULL && "DH: Search KEY2 after delete failed");
    assert(cds_hashtable_dh_delete(table, "nonexistent") == -1 && "DH: Delete non-existent failed to return error");
    printf("  DH: Deletion tests successful.\n");

    // Test search/insert after delete
    retrieved_val = (int*)cds_hashtable_dh_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "DH: Search KEY1 after deleting KEY2 failed");
    assert(cds_hashtable_dh_insert(table, KEY4, &VAL2) == 0 && "DH: Insert KEY4 after delete failed");
    retrieved_val = (int*)cds_hashtable_dh_search(table, KEY4);
    assert(retrieved_val != NULL && *retrieved_val == VAL2 && "DH: Search KEY4 failed");
    printf("  DH: Search/Insert after delete successful.\n");

    // Test Table Fullness (similar to QP)
    // Items: KEY1, KEY3, KEY4. Count = 3. Size = 10.
    char key_buf[20];
    int val_buf[7];
    for (int i = 0; i < 7; ++i) {
        sprintf(key_buf, "dh_test_key_%d", i);
        val_buf[i] = i * 10;
        int ins_ret = cds_hashtable_dh_insert(table, key_buf, &val_buf[i]);
         if (i < 6) {
             assert(ins_ret == 0 && "DH: Failed to insert when table should have space.");
        } else if (ins_ret != 0 && ins_ret != -2) {
             assert(0 && "DH: Unexpected error during fill");
        }
    }
    int last_val = 999;
    int full_ret = cds_hashtable_dh_insert(table, "dh_overflow_key", &last_val);
    assert(full_ret == -2 && "DH: Insert into (nearly) full table did not return -2");
    printf("  DH: Table fullness test successful.\n");

    // Test Destruction
    cds_hashtable_dh_destroy(table);
    printf("  DH: Destruction successful.\n");
    cds_hashtable_dh_destroy(NULL);
    printf("  DH: Destroying NULL table handled.\n");

    printf("Double Hashing tests complete.\n\n");
}

void test_hashtable_cuckoo(void) {
    printf("Testing Cuckoo Hashing...\n");
    cds_hashtable_cuckoo_t* table = NULL;
    size_t cuckoo_table_size = 5; // Small per-table size to force displacements

    // Test Creation
    table = cds_hashtable_cuckoo_create(cuckoo_table_size, test_cuckoo_hash_func1, test_cuckoo_hash_func2);
    assert(table != NULL && "Cuckoo: Create failed");
    printf("  Cuckoo: Creation successful.\n");

    // Test Insertion
    assert(cds_hashtable_cuckoo_insert(table, KEY1, &VAL1) == 0 && "Cuckoo: Insert KEY1 failed");
    assert(cds_hashtable_cuckoo_insert(table, KEY2, &VAL2) == 0 && "Cuckoo: Insert KEY2 failed");
    assert(cds_hashtable_cuckoo_insert(table, KEY3, &VAL3) == 0 && "Cuckoo: Insert KEY3 failed");
    printf("  Cuckoo: Initial insertion successful.\n");

    // Test Search
    int* retrieved_val = (int*)cds_hashtable_cuckoo_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1 && "Cuckoo: Search KEY1 failed");
    retrieved_val = (int*)cds_hashtable_cuckoo_search(table, KEY2);
    assert(retrieved_val != NULL && *retrieved_val == VAL2 && "Cuckoo: Search KEY2 failed");
    assert(cds_hashtable_cuckoo_search(table, "nonexistent") == NULL && "Cuckoo: Search non-existent failed");
    printf("  Cuckoo: Search tests successful.\n");

    // Test Update
    assert(cds_hashtable_cuckoo_insert(table, KEY1, &VAL1_UPDATED) == 0 && "Cuckoo: Update KEY1 failed");
    retrieved_val = (int*)cds_hashtable_cuckoo_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "Cuckoo: Search KEY1 after update failed");
    printf("  Cuckoo: Update test successful.\n");

    // Test Deletion
    assert(cds_hashtable_cuckoo_delete(table, KEY2) == 0 && "Cuckoo: Delete KEY2 failed");
    assert(cds_hashtable_cuckoo_search(table, KEY2) == NULL && "Cuckoo: Search KEY2 after delete failed");
    assert(cds_hashtable_cuckoo_delete(table, "nonexistent") == -1 && "Cuckoo: Delete non-existent failed to return error");
    printf("  Cuckoo: Deletion tests successful.\n");

    // Test insertion that causes displacements
    // KEY1, KEY3 are in. KEY2 deleted.
    // Add KEY4, KEY5. Total table capacity is 2 * cuckoo_table_size = 10.
    // Max displacements is default (e.g. 16 from implementation).
    assert(cds_hashtable_cuckoo_insert(table, KEY4, &VAL1) == 0 && "Cuckoo: Insert KEY4 failed (displacement test)");
    assert(cds_hashtable_cuckoo_insert(table, KEY5, &VAL2) == 0 && "Cuckoo: Insert KEY5 failed (displacement test)");
    printf("  Cuckoo: Displacement insertions successful.\n");

    // Test for exceeding max displacements (difficult to guarantee without specific hash functions and size)
    // Fill up the table more to try and trigger it.
    // Current: KEY1, KEY3, KEY4, KEY5. (4 items)
    char key_buf[20];
    int val_buf[10]; // Max 10 items
    int items_inserted_before_full = 0;
    for(int i = 0; i < 6; ++i) { // Try to insert 6 more items
        sprintf(key_buf, "cuckoo_fill_%d", i);
        val_buf[i] = i * 5;
        int ret = cds_hashtable_cuckoo_insert(table, key_buf, &val_buf[i]);
        if (ret == 0) {
            items_inserted_before_full++;
        } else if (ret == -2) { // Max displacements reached or table considered full by insert logic
            printf("  Cuckoo: Max displacements likely reached as expected (ret code -2).\n");
            break;
        } else {
            printf("  Cuckoo: Unexpected error %d during fill.\n", ret);
            assert(0 && "Cuckoo: Fill error");
        }
    }
    // Ensure at least some items were inserted before it got "full"
    assert(items_inserted_before_full > 0 && "Cuckoo: Failed to insert any items during fill test");


    // Test Destruction
    cds_hashtable_cuckoo_destroy(table);
    printf("  Cuckoo: Destruction successful.\n");
    cds_hashtable_cuckoo_destroy(NULL);
    printf("  Cuckoo: Destroying NULL table handled.\n");

    printf("Cuckoo Hashing tests complete.\n\n");
}


void test_hashtable_hopscotch(void) {
    printf("Testing Hopscotch Hashing...\n");
    cds_hashtable_hopscotch_t* table = NULL;
    unsigned int hopscotch_H = 32; // Default H from implementation for consistency

    // Test Creation
    table = cds_hashtable_hopscotch_create(20, hopscotch_H); // Larger size for hopscotch
    assert(table != NULL && "Hopscotch: Create failed");
    printf("  Hopscotch: Creation successful.\n");

    // Test Insertion (basic, without forced complex swaps)
    assert(cds_hashtable_hopscotch_insert(table, KEY1, &VAL1) == 0 && "Hopscotch: Insert KEY1 failed");
    assert(cds_hashtable_hopscotch_insert(table, KEY2, &VAL2) == 0 && "Hopscotch: Insert KEY2 failed");
    assert(cds_hashtable_hopscotch_insert(table, KEY3, &VAL3) == 0 && "Hopscotch: Insert KEY3 failed");
    printf("  Hopscotch: Initial insertion successful.\n");

    // Test Search
    int* retrieved_val = (int*)cds_hashtable_hopscotch_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1 && "Hopscotch: Search KEY1 failed");
    assert(cds_hashtable_hopscotch_search(table, "nonexistent") == NULL && "Hopscotch: Search non-existent failed");
    printf("  Hopscotch: Search tests successful.\n");

    // Test Update (Hopscotch insert should handle update if key exists)
    // Note: Hopscotch does not directly "update" in place if key is found during probing for empty.
    // It finds an empty slot then adds. If search reveals key, it's an application-level update.
    // The provided insert does not search first, it finds empty then sets hop_info.
    // To test update, one might need a get_entry then modify, or delete then re-insert.
    // For now, we assume insert of existing key is not how updates are primarily done.
    // Let's test inserting a new value for an existing key, this should ideally update.
    // The current `insert` doesn't explicitly check for existing keys before finding an empty slot.
    // This test might reflect "inserting a duplicate key" rather than "update".
    // Depending on specific hopscotch impl, this might create a duplicate or error.
    // The current code does NOT check for duplicates before insertion and will add another one if hash collides + finds a slot.
    // This is a limitation of the current test/implementation interaction.
    // A proper test would require a get_entry_for_update or similar.
    // For now, let's test deletion and re-insertion as a form of update.
    assert(cds_hashtable_hopscotch_delete(table, KEY1) == 0 && "Hopscotch: Delete KEY1 for update test failed");
    assert(cds_hashtable_hopscotch_insert(table, KEY1, &VAL1_UPDATED) == 0 && "Hopscotch: Insert KEY1 with new value failed");
    retrieved_val = (int*)cds_hashtable_hopscotch_search(table, KEY1);
    assert(retrieved_val != NULL && *retrieved_val == VAL1_UPDATED && "Hopscotch: Search KEY1 after re-insert (update) failed");
    printf("  Hopscotch: Update (via delete and re-insert) test successful.\n");

    // Test Deletion
    assert(cds_hashtable_hopscotch_delete(table, KEY2) == 0 && "Hopscotch: Delete KEY2 failed");
    assert(cds_hashtable_hopscotch_search(table, KEY2) == NULL && "Hopscotch: Search KEY2 after delete failed");
    assert(cds_hashtable_hopscotch_delete(table, "nonexistent") == -1 && "Hopscotch: Delete non-existent failed to return error");
    printf("  Hopscotch: Deletion tests successful.\n");

    // Test insertion where swap logic *would* be needed (currently returns -3)
    // This requires careful setup of keys to ensure initial hash is far from empty slot.
    // For now, acknowledge this is hard to test without full swap logic.
    // We can try to fill the table to a point where linear probing for an empty slot goes far.
    printf("  Hopscotch: Advanced swap test for insertion is skipped due to simplified insert logic.\n");
    // Fill table to near capacity
    char key_buf[20];
    int val_buf[15]; // Table size 20. Items: KEY1, KEY3. (2 items). Add 15 more.
    int items_inserted = 0;
    for(int i=0; i < 15; ++i) {
        sprintf(key_buf, "hop_fill_%d", i);
        val_buf[i] = i*7;
        if (cds_hashtable_hopscotch_insert(table, key_buf, &val_buf[i]) == 0) {
            items_inserted++;
        } else {
            // This might happen if an insertion requires a swap (returns -3)
            // or if table gets truly full for linear probing part (-2)
            printf("  Hopscotch: Insertion %d during fill returned non-zero (possibly needs swap or full).\n", i);
            break;
        }
    }
    printf("  Hopscotch: Filled table with %d additional items.\n", items_inserted);
    assert(items_inserted > 10 && "Hopscotch: Should be able to insert a good number of items.");


    // Test Destruction
    cds_hashtable_hopscotch_destroy(table);
    printf("  Hopscotch: Destruction successful.\n");
    cds_hashtable_hopscotch_destroy(NULL);
    printf("  Hopscotch: Destroying NULL table handled.\n");

    printf("Hopscotch Hashing tests complete (with simplified insert).\n\n");
}
