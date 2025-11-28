#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cds/hashtable.h"

// Max displacements before considering table full/rehash needed.
// log2(size) is common. For a size of 1024, log2(1024) = 10.
// Let's use a slightly more generous fixed value for simplicity here,
// or a small multiple of log(size) if size is available at this point.
// For now, a fixed value. This should ideally be configurable or size-dependent.
#define DEFAULT_MAX_DISPLACEMENTS 16

// Define the internal structure for a Cuckoo hash table entry
typedef struct cds_cuckoo_entry_s {
    char* key;
    void* value;
    // No status flag needed if NULL key implies empty slot
} cds_cuckoo_entry_t;

// Define the Cuckoo hash table structure
struct cds_hashtable_cuckoo_s {
    size_t size; // Size of each individual table
    size_t count; // Total number of items in both tables
    cds_cuckoo_entry_t* table1;
    cds_cuckoo_entry_t* table2;
    cds_hash_function_t hash_func1;
    cds_hash_function_t hash_func2;
    unsigned int max_displacements;
};

// Allocates and initializes the Cuckoo hash table
cds_hashtable_cuckoo_t* cds_hashtable_cuckoo_create(size_t initial_size_per_table, cds_hash_function_t h1, cds_hash_function_t h2) {
    if (initial_size_per_table == 0 || !h1 || !h2) {
        return NULL;
    }
    cds_hashtable_cuckoo_t* table = malloc(sizeof(cds_hashtable_cuckoo_t));
    if (!table) {
        return NULL;
    }
    table->size = initial_size_per_table;
    table->count = 0;
    table->hash_func1 = h1;
    table->hash_func2 = h2;
    table->max_displacements = DEFAULT_MAX_DISPLACEMENTS; // Or pass as param

    table->table1 = calloc(table->size, sizeof(cds_cuckoo_entry_t));
    if (!table->table1) {
        free(table);
        return NULL;
    }
    table->table2 = calloc(table->size, sizeof(cds_cuckoo_entry_t));
    if (!table->table2) {
        free(table->table1);
        free(table);
        return NULL;
    }
    // calloc ensures entries are zeroed, so key pointers are NULL initially.
    return table;
}

// Frees all memory associated with the Cuckoo hash table
void cds_hashtable_cuckoo_destroy(cds_hashtable_cuckoo_t* table) {
    if (!table) {
        return;
    }
    for (size_t i = 0; i < table->size; i++) {
        if (table->table1[i].key) {
            free(table->table1[i].key);
            // User is responsible for value memory
        }
        if (table->table2[i].key) {
            free(table->table2[i].key);
            // User is responsible for value memory
        }
    }
    free(table->table1);
    free(table->table2);
    free(table);
}

// Inserts a key-value pair using Cuckoo hashing
int cds_hashtable_cuckoo_insert(cds_hashtable_cuckoo_t* table, const char* key, void* value) {
    if (!table || !key) {
        return -1; // Invalid args
    }

    // Check if key already exists (optional, but good practice)
    // For Cuckoo, search is cheap. This avoids displacing for an update.
    void* existing_value = cds_hashtable_cuckoo_search(table, key);
    if (existing_value != NULL) {
        // Key exists. Update value.
        // Need to find which table it's in to update.
        size_t h1 = table->hash_func1(key, table->size);
        if (table->table1[h1].key && strcmp(table->table1[h1].key, key) == 0) {
            table->table1[h1].value = value;
            return 0; // Success (updated)
        }
        size_t h2 = table->hash_func2(key, table->size);
        if (table->table2[h2].key && strcmp(table->table2[h2].key, key) == 0) {
            table->table2[h2].value = value;
            return 0; // Success (updated)
        }
        // If search found it but these checks don't, something is wrong.
        // This path should ideally not be hit if search is correct.
        return -3; // Inconsistent state or search error
    }


    char* current_key = strdup(key);
    if (!current_key) return -1; // strdup failed
    void* current_value = value;

    cds_cuckoo_entry_t* target_table;
    cds_hash_function_t current_hash_func;
    size_t current_hash_idx;

    // Start with table1
    target_table = table->table1;
    current_hash_func = table->hash_func1;

    for (unsigned int i = 0; i < table->max_displacements; i++) {
        current_hash_idx = current_hash_func(current_key, table->size);

        if (target_table[current_hash_idx].key == NULL) { // Slot is empty
            target_table[current_hash_idx].key = current_key;
            target_table[current_hash_idx].value = current_value;
            table->count++;
            return 0; // Success
        } else { // Slot is occupied, kick out existing element
            char* displaced_key = target_table[current_hash_idx].key;
            void* displaced_value = target_table[current_hash_idx].value;

            target_table[current_hash_idx].key = current_key;
            target_table[current_hash_idx].value = current_value;

            current_key = displaced_key;
            current_value = displaced_value;

            // Switch to the other table for the next iteration
            if (target_table == table->table1) {
                target_table = table->table2;
                current_hash_func = table->hash_func2;
            } else {
                target_table = table->table1;
                current_hash_func = table->hash_func1;
            }
        }
    }

    // If loop finishes, max_displacements reached. Insertion failed.
    // The `current_key`, `current_value` that couldn't be placed needs to be freed.
    free(current_key);
    // The original key/value were successfully inserted somewhere, but the last displaced one is homeless.
    // This indicates table is full or needs rehash.
    return -2; // Max displacements reached
}

// Searches for a key in the Cuckoo hash table
void* cds_hashtable_cuckoo_search(cds_hashtable_cuckoo_t* table, const char* key) {
    if (!table || !key) {
        return NULL;
    }

    size_t h1 = table->hash_func1(key, table->size);
    if (table->table1[h1].key && strcmp(table->table1[h1].key, key) == 0) {
        return table->table1[h1].value;
    }

    size_t h2 = table->hash_func2(key, table->size);
    if (table->table2[h2].key && strcmp(table->table2[h2].key, key) == 0) {
        return table->table2[h2].value;
    }

    return NULL; // Not found
}

// Deletes a key-value pair from the Cuckoo hash table
int cds_hashtable_cuckoo_delete(cds_hashtable_cuckoo_t* table, const char* key) {
    if (!table || !key) {
        return -1; // Invalid args
    }

    size_t h1 = table->hash_func1(key, table->size);
    if (table->table1[h1].key && strcmp(table->table1[h1].key, key) == 0) {
        free(table->table1[h1].key);
        table->table1[h1].key = NULL;
        table->table1[h1].value = NULL; // Clear value
        table->count--;
        return 0; // Success
    }

    size_t h2 = table->hash_func2(key, table->size);
    if (table->table2[h2].key && strcmp(table->table2[h2].key, key) == 0) {
        free(table->table2[h2].key);
        table->table2[h2].key = NULL;
        table->table2[h2].value = NULL; // Clear value
        table->count--;
        return 0; // Success
    }

    return -1; // Key not found
}
