#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cds/hashtable.h"

// Define the internal structures for a hash node
typedef struct cds_hash_node_s {
    char* key;
    void* value;
    struct cds_hash_node_s* next;
} cds_hash_node_t;

// Define the hash table structure
struct cds_hashtable_chaining_s {
    size_t size;
    cds_hash_node_t** buckets;
};

// A simple hash function
static size_t cds_hashtable_chaining_hash(const char* key, size_t table_size) {
    size_t hash = 0;
    for (size_t i = 0; key[i] != '\0'; i++) {
        hash = (hash * 31 + key[i]) % table_size; // More robust hash function
    }
    return hash;
}

// Allocates and initializes the hash table
cds_hashtable_chaining_t* cds_hashtable_chaining_create(size_t initial_size) {
    if (initial_size == 0) {
        return NULL;
    }
    cds_hashtable_chaining_t* table = malloc(sizeof(cds_hashtable_chaining_t));
    if (!table) {
        return NULL;
    }
    table->size = initial_size;
    table->buckets = calloc(initial_size, sizeof(cds_hash_node_t*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    return table;
}

// Frees all memory associated with the hash table
void cds_hashtable_chaining_destroy(cds_hashtable_chaining_t* table) {
    if (!table) {
        return;
    }
    for (size_t i = 0; i < table->size; i++) {
        cds_hash_node_t* current = table->buckets[i];
        while (current) {
            cds_hash_node_t* next = current->next;
            free(current->key); // Assume key was strdup'd
            // User is responsible for freeing the value if it's dynamically allocated
            free(current);
            current = next;
        }
    }
    free(table->buckets);
    free(table);
}

// Inserts a key-value pair
int cds_hashtable_chaining_insert(cds_hashtable_chaining_t* table, const char* key, void* value) {
    if (!table || !key) {
        return -1; // Error: invalid arguments
    }
    size_t index = cds_hashtable_chaining_hash(key, table->size);
    cds_hash_node_t* current = table->buckets[index];
    cds_hash_node_t* prev = NULL;

    // Check if key already exists
    while (current) {
        if (strcmp(current->key, key) == 0) {
            // Key exists, update value (caller is responsible for old value's memory if needed)
            current->value = value;
            return 0; // Success
        }
        prev = current;
        current = current->next;
    }

    // Key does not exist, create new node
    cds_hash_node_t* new_node = malloc(sizeof(cds_hash_node_t));
    if (!new_node) {
        return -1; // Error: memory allocation failed
    }
    new_node->key = strdup(key);
    if (!new_node->key) {
        free(new_node);
        return -1; // Error: memory allocation failed for key
    }
    new_node->value = value;
    new_node->next = NULL;

    if (prev) {
        prev->next = new_node;
    } else {
        table->buckets[index] = new_node;
    }
    return 0; // Success
}

// Searches for a key and returns its value
void* cds_hashtable_chaining_search(cds_hashtable_chaining_t* table, const char* key) {
    if (!table || !key) {
        return NULL;
    }
    size_t index = cds_hashtable_chaining_hash(key, table->size);
    cds_hash_node_t* current = table->buckets[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL; // Not found
}

// Deletes a key-value pair
int cds_hashtable_chaining_delete(cds_hashtable_chaining_t* table, const char* key) {
    if (!table || !key) {
        return -1; // Error: invalid arguments
    }
    size_t index = cds_hashtable_chaining_hash(key, table->size);
    cds_hash_node_t* current = table->buckets[index];
    cds_hash_node_t* prev = NULL;

    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                table->buckets[index] = current->next;
            }
            free(current->key);
            // User is responsible for freeing the value if it's dynamically allocated
            free(current);
            return 0; // Success
        }
        prev = current;
        current = current->next;
    }
    return -1; // Not found
}
