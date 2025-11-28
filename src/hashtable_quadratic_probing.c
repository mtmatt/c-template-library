#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cds/hashtable.h"

// Entry status flags (can be shared if defined in a common internal header, or redefined)
typedef enum {
    QP_EMPTY,
    QP_OCCUPIED,
    QP_DELETED
} cds_qp_entry_status_e;

// Define the internal structure for a hash table entry
typedef struct cds_qp_entry_s {
    char* key;
    void* value;
    cds_qp_entry_status_e status;
} cds_qp_entry_t;

// Define the hash table structure
struct cds_hashtable_qp_s {
    size_t size;
    size_t count; // Number of OCCUPIED entries
    cds_qp_entry_t* entries;
    // For h(k) + i*i, no c1, c2 needed explicitly in struct if we stick to i*i
};

// Primary hash function
static size_t cds_hashtable_qp_hash(const char* key, size_t table_size) {
    size_t hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++) {
        hash = (hash * 31 + key[i]) % table_size;
    }
    return hash;
}

// Allocates and initializes the hash table
cds_hashtable_qp_t* cds_hashtable_qp_create(size_t initial_size) {
    if (initial_size == 0) {
        return NULL;
    }
    cds_hashtable_qp_t* table = malloc(sizeof(cds_hashtable_qp_t));
    if (!table) {
        return NULL;
    }
    table->size = initial_size;
    table->count = 0;
    table->entries = malloc(initial_size * sizeof(cds_qp_entry_t));
    if (!table->entries) {
        free(table);
        return NULL;
    }
    for (size_t i = 0; i < initial_size; i++) {
        table->entries[i].key = NULL;
        table->entries[i].value = NULL;
        table->entries[i].status = QP_EMPTY;
    }
    return table;
}

// Frees all memory associated with the hash table
void cds_hashtable_qp_destroy(cds_hashtable_qp_t* table) {
    if (!table) {
        return;
    }
    for (size_t i = 0; i < table->size; i++) {
        if (table->entries[i].status == QP_OCCUPIED || table->entries[i].key != NULL) {
            free(table->entries[i].key);
        }
    }
    free(table->entries);
    free(table);
}

// Inserts a key-value pair using quadratic probing
// Probing sequence: (h(key) + i*i) % size
// Resizing is not implemented in this version.
int cds_hashtable_qp_insert(cds_hashtable_qp_t* table, const char* key, void* value) {
    if (!table || !key) {
        return -1; // Error: invalid arguments
    }

    // TODO: Implement resizing if table->count / table->size > LOAD_FACTOR_THRESHOLD
    if (table->count >= table->size) { // Basic check for full table before probing
        // This check is not perfect for quadratic probing as an insertion might fail even if count < size
        // A more robust check would be to limit the number of probes.
        // However, if all slots are QP_OCCUPIED, this is accurate.
    }


    size_t initial_index = cds_hashtable_qp_hash(key, table->size);
    size_t first_deleted_slot = (size_t)-1;

    for (size_t i = 0; i < table->size; i++) { // Limit probes to table size
        size_t index = (initial_index + i * i) % table->size;

        if (table->entries[index].status == QP_EMPTY) {
            size_t insert_pos = (first_deleted_slot != (size_t)-1) ? first_deleted_slot : index;

            // If we are using a deleted slot that is not the current empty slot,
            // we need to check if the key already exists at the current empty slot's original probe sequence position.
            // This is complex. For simplicity, if we find an EMPTY slot, we use it.
            // If we passed a DELETED slot, we prefer it.
            // This might not be the absolute optimal for probe chains but simplifies logic.

            if (first_deleted_slot != (size_t)-1 && index != first_deleted_slot) {
                 // We found an empty slot, but we also passed a deleted slot.
                 // Check if key exists at current 'index' before deciding to use 'first_deleted_slot'
                 // This only makes sense if the current 'index' was the one we would use without 'first_deleted_slot'
                 // Let's simplify: if first_deleted_slot is set, we target that. If not, current index.
            }


            size_t actual_insert_index = (first_deleted_slot != (size_t)-1) ? first_deleted_slot : index;

            // Special case: if we found an empty slot (index) but we are about to use an earlier deleted slot (first_deleted_slot)
            // we must ensure the key doesn't actually exist at 'index' if 'index' was probed before 'first_deleted_slot'
            // This logic gets complicated. A common strategy is to insert at first available (EMPTY or DELETED).

            table->entries[actual_insert_index].key = strdup(key);
            if (!table->entries[actual_insert_index].key) {
                return -1; // strdup failed
            }
            table->entries[actual_insert_index].value = value;
            table->entries[actual_insert_index].status = QP_OCCUPIED;
            table->count++;
            return 0; // Success
        } else if (table->entries[index].status == QP_OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                // Key already exists, update value
                table->entries[index].value = value;
                return 0; // Success
            }
        } else if (table->entries[index].status == QP_DELETED) {
            if (first_deleted_slot == (size_t)-1) {
                first_deleted_slot = index;
            }
        }
    }

    // If loop finishes, we couldn't find an EMPTY slot.
    // If we found a DELETED slot, we can use it.
    if (first_deleted_slot != (size_t)-1) {
        table->entries[first_deleted_slot].key = strdup(key);
        if (!table->entries[first_deleted_slot].key) {
            return -1; // strdup failed
        }
        table->entries[first_deleted_slot].value = value;
        table->entries[first_deleted_slot].status = QP_OCCUPIED;
        table->count++;
        return 0; // Success
    }

    return -2; // Error: table is full or insertion failed after max probes
}

// Searches for a key using quadratic probing
void* cds_hashtable_qp_search(cds_hashtable_qp_t* table, const char* key) {
    if (!table || !key) {
        return NULL;
    }
    size_t initial_index = cds_hashtable_qp_hash(key, table->size);

    for (size_t i = 0; i < table->size; i++) { // Limit probes
        size_t index = (initial_index + i * i) % table->size;

        if (table->entries[index].status == QP_OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                return table->entries[index].value; // Found
            }
        } else if (table->entries[index].status == QP_EMPTY) {
            return NULL; // Key not found (hit an empty slot)
        }
        // If QP_DELETED, continue probing
    }
    return NULL; // Key not found after probing limit
}

// Deletes a key-value pair by marking the slot as DELETED
int cds_hashtable_qp_delete(cds_hashtable_qp_t* table, const char* key) {
    if (!table || !key) {
        return -1; // Error: invalid arguments
    }
    size_t initial_index = cds_hashtable_qp_hash(key, table->size);

    for (size_t i = 0; i < table->size; i++) { // Limit probes
        size_t index = (initial_index + i * i) % table->size;

        if (table->entries[index].status == QP_OCCUPIED) {
            if (strcmp(table->entries[index].key, key) == 0) {
                free(table->entries[index].key);
                table->entries[index].key = NULL;
                table->entries[index].value = NULL;
                table->entries[index].status = QP_DELETED;
                table->count--;
                return 0; // Success
            }
        } else if (table->entries[index].status == QP_EMPTY) {
            return -1; // Key not found
        }
        // If QP_DELETED, continue probing
    }
    return -1; // Key not found after probing limit
}
