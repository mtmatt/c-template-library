#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cds/hashtable.h"

// Entry status flags
typedef enum {
    DH_EMPTY,
    DH_OCCUPIED,
    DH_DELETED
} cds_dh_entry_status_e;

// Define the internal structure for a hash table entry
typedef struct cds_dh_entry_s {
    char* key;
    void* value;
    cds_dh_entry_status_e status;
} cds_dh_entry_t;

// Define the hash table structure
struct cds_hashtable_dh_s {
    size_t size;
    size_t count; // Number of OCCUPIED entries
    cds_dh_entry_t* entries;
};

// Primary hash function
static size_t cds_hashtable_dh_hash1(const char* key, size_t table_size) {
    size_t hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++) {
        hash = (hash * 31 + key[i]) % table_size; // Example: DJB2a variant
    }
    return hash;
}

// Secondary hash function
// Must not return 0. A common practice: q - (key_val % q) where q is prime < table_size.
// For simplicity, let's use a different multiplicative hash and ensure non-zero.
static size_t cds_hashtable_dh_hash2(const char* key, size_t table_size) {
    size_t hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++) {
        hash = (hash * 37 + key[i]); // Using a different prime (37)
    }
    // Ensure hash is not 0 and is less than table_size (for step size)
    // The result of this hash is used as a step, so it should ideally be coprime to table_size.
    // A simple way to ensure non-zero for step: (hash % (table_size - 1)) + 1
    // This makes the step between 1 and table_size - 1.
    if (table_size <= 1) return 1; // Avoid modulo by zero or negative if table_size is tiny
    size_t step = (hash % (table_size -1)) + 1;
    return step;

    // A common prime-based secondary hash:
    // const size_t prime_smaller_than_size = 97; // Example if size is ~100. Choose appropriately.
    // if (table_size <= prime_smaller_than_size) return 1; // Fallback for small tables
    // size_t h_val = 0;
    // for (size_t i = 0; key[i] != '\0'; i++) { h_val = (h_val * 19 + key[i]); } // Yet another hash
    // size_t step = prime_smaller_than_size - (h_val % prime_smaller_than_size);
    // return step == 0 ? 1 : step; // Ensure non-zero
}


// Allocates and initializes the hash table
cds_hashtable_dh_t* cds_hashtable_dh_create(size_t initial_size) {
    if (initial_size == 0) {
        return NULL;
    }
    cds_hashtable_dh_t* table = malloc(sizeof(cds_hashtable_dh_t));
    if (!table) {
        return NULL;
    }
    table->size = initial_size;
    table->count = 0;
    table->entries = malloc(initial_size * sizeof(cds_dh_entry_t));
    if (!table->entries) {
        free(table);
        return NULL;
    }
    for (size_t i = 0; i < initial_size; i++) {
        table->entries[i].key = NULL;
        table->entries[i].value = NULL;
        table->entries[i].status = DH_EMPTY;
    }
    return table;
}

// Frees all memory associated with the hash table
void cds_hashtable_dh_destroy(cds_hashtable_dh_t* table) {
    if (!table) {
        return;
    }
    for (size_t i = 0; i < table->size; i++) {
        if (table->entries[i].status == DH_OCCUPIED || table->entries[i].key != NULL) {
            free(table->entries[i].key);
        }
    }
    free(table->entries);
    free(table);
}

// Inserts a key-value pair using double hashing
// Probing sequence: (h1(key) + i * h2(key)) % size
// Resizing is not implemented.
int cds_hashtable_dh_insert(cds_hashtable_dh_t* table, const char* key, void* value) {
    if (!table || !key || table->size == 0) {
        return -1; // Error: invalid arguments
    }

    // TODO: Implement resizing if table->count / table->size > LOAD_FACTOR_THRESHOLD
    // No perfect check for "full" with double hashing other than trying to insert.
    // Limit probes to table->size to prevent infinite loops if hash2 is not well chosen or table is full.

    size_t h1 = cds_hashtable_dh_hash1(key, table->size);
    size_t h2 = cds_hashtable_dh_hash2(key, table->size);
    size_t first_deleted_slot = (size_t)-1;

    for (size_t i = 0; i < table->size; i++) { // Limit probes
        size_t index = (h1 + i * h2) % table->size;

        if (table->entries[index].status == DH_EMPTY) {
            size_t insert_pos = (first_deleted_slot != (size_t)-1) ? first_deleted_slot : index;
            table->entries[insert_pos].key = strdup(key);
            if (!table->entries[insert_pos].key) {
                return -1; // strdup failed
            }
            table->entries[insert_pos].value = value;
            table->entries[insert_pos].status = DH_OCCUPIED;
            table->count++;
            return 0; // Success
        } else if (table->entries[index].status == DH_OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                // Key already exists, update value
                table->entries[index].value = value;
                return 0; // Success
            }
        } else if (table->entries[index].status == DH_DELETED) {
            if (first_deleted_slot == (size_t)-1) {
                first_deleted_slot = index;
            }
        }
    }

    // If loop finishes, we couldn't find an EMPTY slot.
    // If we found a DELETED slot during probing, use it.
    if (first_deleted_slot != (size_t)-1) {
        table->entries[first_deleted_slot].key = strdup(key);
        if (!table->entries[first_deleted_slot].key) {
            return -1; // strdup failed
        }
        table->entries[first_deleted_slot].value = value;
        table->entries[first_deleted_slot].status = DH_OCCUPIED;
        table->count++;
        return 0; // Success
    }

    return -2; // Error: table is full or insertion failed after max probes
}

// Searches for a key using double hashing
void* cds_hashtable_dh_search(cds_hashtable_dh_t* table, const char* key) {
    if (!table || !key || table->size == 0) {
        return NULL;
    }
    size_t h1 = cds_hashtable_dh_hash1(key, table->size);
    size_t h2 = cds_hashtable_dh_hash2(key, table->size);

    for (size_t i = 0; i < table->size; i++) { // Limit probes
        size_t index = (h1 + i * h2) % table->size;

        if (table->entries[index].status == DH_OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                return table->entries[index].value; // Found
            }
        } else if (table->entries[index].status == DH_EMPTY) {
            return NULL; // Key not found (hit an empty slot)
        }
        // If DH_DELETED, continue probing
    }
    return NULL; // Key not found after probing limit
}

// Deletes a key-value pair by marking the slot as DELETED
int cds_hashtable_dh_delete(cds_hashtable_dh_t* table, const char* key) {
    if (!table || !key || table->size == 0) {
        return -1; // Error: invalid arguments
    }
    size_t h1 = cds_hashtable_dh_hash1(key, table->size);
    size_t h2 = cds_hashtable_dh_hash2(key, table->size);

    for (size_t i = 0; i < table->size; i++) { // Limit probes
        size_t index = (h1 + i * h2) % table->size;

        if (table->entries[index].status == DH_OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                free(table->entries[index].key);
                table->entries[index].key = NULL;
                table->entries[index].value = NULL;
                table->entries[index].status = DH_DELETED;
                table->count--;
                return 0; // Success
            }
        } else if (table->entries[index].status == DH_EMPTY) {
            return -1; // Key not found
        }
        // If DH_DELETED, continue probing
    }
    return -1; // Key not found after probing limit
}
