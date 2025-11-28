#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cds/hashtable.h"

// Entry status flags
typedef enum {
    EMPTY,
    OCCUPIED,
    DELETED
} cds_lp_entry_status_e;

// Define the internal structure for a hash table entry
typedef struct cds_lp_entry_s {
    char* key;
    void* value;
    cds_lp_entry_status_e status;
} cds_lp_entry_t;

// Define the hash table structure
struct cds_hashtable_lp_s {
    size_t size;
    size_t count; // Number of OCCUPIED entries, for load factor
    cds_lp_entry_t* entries;
};

// Hash function (same as chaining for simplicity, can be different)
static size_t cds_hashtable_lp_hash(const char* key, size_t table_size) {
    size_t hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++) {
        hash = (hash * 31 + key[i]) % table_size;
    }
    return hash;
}

// Allocates and initializes the hash table
cds_hashtable_lp_t* cds_hashtable_lp_create(size_t initial_size) {
    if (initial_size == 0) {
        return NULL;
    }
    cds_hashtable_lp_t* table = malloc(sizeof(cds_hashtable_lp_t));
    if (!table) {
        return NULL;
    }
    table->size = initial_size;
    table->count = 0;
    table->entries = malloc(initial_size * sizeof(cds_lp_entry_t));
    if (!table->entries) {
        free(table);
        return NULL;
    }
    for (size_t i = 0; i < initial_size; i++) {
        table->entries[i].key = NULL;
        table->entries[i].value = NULL;
        table->entries[i].status = EMPTY;
    }
    return table;
}

// Frees all memory associated with the hash table
void cds_hashtable_lp_destroy(cds_hashtable_lp_t* table) {
    if (!table) {
        return;
    }
    for (size_t i = 0; i < table->size; i++) {
        if (table->entries[i].status == OCCUPIED || (table->entries[i].key != NULL)) { // Key might exist even if DELETED
            free(table->entries[i].key);
        }
    }
    free(table->entries);
    free(table);
}

// Inserts a key-value pair using linear probing
// For simplicity, resizing is not implemented in this version.
// A production system should handle resizing when load factor is high.
int cds_hashtable_lp_insert(cds_hashtable_lp_t* table, const char* key, void* value) {
    if (!table || !key) {
        return -1; // Error: invalid arguments
    }

    // TODO: Implement resizing if table->count / table->size > LOAD_FACTOR_THRESHOLD

    size_t initial_index = cds_hashtable_lp_hash(key, table->size);
    size_t index = initial_index;
    size_t first_deleted_slot = (size_t)-1; // Sentinel for first deleted slot

    do {
        if (table->entries[index].status == EMPTY) {
            // Found an empty slot. If we passed a DELETED slot, use that one.
            size_t insert_pos = (first_deleted_slot != (size_t)-1) ? first_deleted_slot : index;

            table->entries[insert_pos].key = strdup(key);
            if (!table->entries[insert_pos].key) {
                return -1; // strdup failed
            }
            table->entries[insert_pos].value = value;
            table->entries[insert_pos].status = OCCUPIED;
            table->count++;
            return 0; // Success
        } else if (table->entries[index].status == OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                // Key already exists, update value
                table->entries[index].value = value;
                return 0; // Success
            }
        } else if (table->entries[index].status == DELETED) {
            // Found a deleted slot, note it down if it's the first one.
            if (first_deleted_slot == (size_t)-1) {
                first_deleted_slot = index;
            }
        }

        index = (index + 1) % table->size;
    } while (index != initial_index);

    // If we are here, it means table is full and no existing key was found.
    // If there was a deleted slot, we can use it.
    if (first_deleted_slot != (size_t)-1) {
        table->entries[first_deleted_slot].key = strdup(key);
        if (!table->entries[first_deleted_slot].key) {
            return -1; // strdup failed
        }
        table->entries[first_deleted_slot].value = value;
        table->entries[first_deleted_slot].status = OCCUPIED;
        table->count++;
        return 0; // Success
    }

    return -2; // Error: table is full or no suitable slot found (should trigger resize in a full impl)
}

// Searches for a key using linear probing
void* cds_hashtable_lp_search(cds_hashtable_lp_t* table, const char* key) {
    if (!table || !key) {
        return NULL;
    }
    size_t initial_index = cds_hashtable_lp_hash(key, table->size);
    size_t index = initial_index;

    do {
        if (table->entries[index].status == OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                return table->entries[index].value; // Found
            }
        } else if (table->entries[index].status == EMPTY) {
            return NULL; // Key not found (hit an empty slot, cannot be further)
        }
        // If DELETED, continue probing

        index = (index + 1) % table->size;
    } while (index != initial_index);

    return NULL; // Key not found after full scan
}

// Deletes a key-value pair by marking the slot as DELETED
int cds_hashtable_lp_delete(cds_hashtable_lp_t* table, const char* key) {
    if (!table || !key) {
        return -1; // Error: invalid arguments
    }
    size_t initial_index = cds_hashtable_lp_hash(key, table->size);
    size_t index = initial_index;

    do {
        if (table->entries[index].status == OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                free(table->entries[index].key); // Free the strdup'd key
                table->entries[index].key = NULL; // Avoid dangling pointer if this slot is reused
                table->entries[index].value = NULL; // Clear value
                table->entries[index].status = DELETED;
                table->count--;
                return 0; // Success
            }
        } else if (table->entries[index].status == EMPTY) {
            return -1; // Key not found (hit an empty slot)
        }
        // If DELETED, continue probing

        index = (index + 1) % table->size;
    } while (index != initial_index);

    return -1; // Key not found after full scan
}
