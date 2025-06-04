#ifndef CDS_HASHTABLE_H
#define CDS_HASHTABLE_H

// Common structures and function declarations for hash tables will go here.

// Generic hash function pointer type for Cuckoo Hashing
typedef size_t (*cds_hash_function_t)(const char* key, size_t table_size);

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration for chaining hash table
typedef struct cds_hashtable_chaining_s cds_hashtable_chaining_t;

cds_hashtable_chaining_t* cds_hashtable_chaining_create(size_t initial_size);
void cds_hashtable_chaining_destroy(cds_hashtable_chaining_t* table);
int cds_hashtable_chaining_insert(cds_hashtable_chaining_t* table, const char* key, void* value); // Return 0 on success, -1 on failure
void* cds_hashtable_chaining_search(cds_hashtable_chaining_t* table, const char* key);
int cds_hashtable_chaining_delete(cds_hashtable_chaining_t* table, const char* key); // Return 0 on success, -1 if not found

// Forward declaration for linear probing hash table
typedef struct cds_hashtable_lp_s cds_hashtable_lp_t;

cds_hashtable_lp_t* cds_hashtable_lp_create(size_t initial_size);
void cds_hashtable_lp_destroy(cds_hashtable_lp_t* table);
int cds_hashtable_lp_insert(cds_hashtable_lp_t* table, const char* key, void* value); // Return 0 on success, -1 on error, -2 if table full
void* cds_hashtable_lp_search(cds_hashtable_lp_t* table, const char* key);
int cds_hashtable_lp_delete(cds_hashtable_lp_t* table, const char* key); // Return 0 on success, -1 if not found

// Forward declaration for quadratic probing hash table
typedef struct cds_hashtable_qp_s cds_hashtable_qp_t;

cds_hashtable_qp_t* cds_hashtable_qp_create(size_t initial_size);
void cds_hashtable_qp_destroy(cds_hashtable_qp_t* table);
int cds_hashtable_qp_insert(cds_hashtable_qp_t* table, const char* key, void* value); // Return 0 on success, -1 on error, -2 if table full or insert failed
void* cds_hashtable_qp_search(cds_hashtable_qp_t* table, const char* key);
int cds_hashtable_qp_delete(cds_hashtable_qp_t* table, const char* key); // Return 0 on success, -1 if not found

// Forward declaration for double hashing hash table
typedef struct cds_hashtable_dh_s cds_hashtable_dh_t;

cds_hashtable_dh_t* cds_hashtable_dh_create(size_t initial_size);
void cds_hashtable_dh_destroy(cds_hashtable_dh_t* table);
int cds_hashtable_dh_insert(cds_hashtable_dh_t* table, const char* key, void* value); // Return 0 on success, -1 on error, -2 if table full or insert failed
void* cds_hashtable_dh_search(cds_hashtable_dh_t* table, const char* key);
int cds_hashtable_dh_delete(cds_hashtable_dh_t* table, const char* key); // Return 0 on success, -1 if not found

// Forward declaration for Cuckoo hash table
typedef struct cds_hashtable_cuckoo_s cds_hashtable_cuckoo_t;

cds_hashtable_cuckoo_t* cds_hashtable_cuckoo_create(size_t initial_size_per_table, cds_hash_function_t h1, cds_hash_function_t h2);
void cds_hashtable_cuckoo_destroy(cds_hashtable_cuckoo_t* table);
int cds_hashtable_cuckoo_insert(cds_hashtable_cuckoo_t* table, const char* key, void* value); // Return 0 on success, -1 on error, -2 if max displacements reached
void* cds_hashtable_cuckoo_search(cds_hashtable_cuckoo_t* table, const char* key);
int cds_hashtable_cuckoo_delete(cds_hashtable_cuckoo_t* table, const char* key); // Return 0 on success, -1 if not found

// Forward declaration for Hopscotch hash table
typedef struct cds_hashtable_hopscotch_s cds_hashtable_hopscotch_t;

cds_hashtable_hopscotch_t* cds_hashtable_hopscotch_create(size_t initial_size, unsigned int H_neighborhood_size); // H_neighborhood_size is often fixed (e.g. 32/64)
void cds_hashtable_hopscotch_destroy(cds_hashtable_hopscotch_t* table);
int cds_hashtable_hopscotch_insert(cds_hashtable_hopscotch_t* table, const char* key, void* value); // Ret 0 success, -1 err, -2 full, -3 needs swap/rehash
void* cds_hashtable_hopscotch_search(cds_hashtable_hopscotch_t* table, const char* key);
int cds_hashtable_hopscotch_delete(cds_hashtable_hopscotch_t* table, const char* key); // Ret 0 success, -1 not found

#ifdef __cplusplus
}
#endif

#endif // CDS_HASHTABLE_H
