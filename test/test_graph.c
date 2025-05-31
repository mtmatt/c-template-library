#include "test_graph.h"
#include <cds/graph.h>
#include <calgo/algos.h>
#include <cds/list.h> // For checking neighbors
#include <stdio.h>
#include <stdlib.h> // For calloc, free

// NOTE ON CDS_GRAPH_ADJ_USE_ARRAY:
// If the graph library is compiled with CDS_GRAPH_ADJ_USE_ARRAY defined,
// cds_graph_get_neighbors() returns a NEWLY ALLOCATED CdsList that
// MUST be freed by the caller using cds_list_delete().
// These tests currently DO NOT free the list returned by cds_graph_get_neighbors(),
// which is correct for the default CdsList-based graph implementation.
// To run these tests with an array-based graph, you would need to:
//   1. Compile graph.c with -DCDS_GRAPH_ADJ_USE_ARRAY.
//   2. Modify calls to cds_graph_get_neighbors in these tests to store the result
//      in a CdsList* and call cds_list_delete() on it (and free the pointer itself)
//      after use, e.g.:
//      CdsList* neighbors = cds_graph_get_neighbors(&g, 0);
//      // ... checks ...
//      #ifdef CDS_GRAPH_ADJ_USE_ARRAY
//      if (neighbors) { // Important check, as it could be NULL
//          cds_list_delete(neighbors);
//          free(neighbors); // Free the CdsList struct itself
//      }
//      #endif
// This conditional freeing is not implemented below to keep tests simpler for the default case.

#include <string.h> // For memset

// Helper function to check if a value exists in a CdsList of size_t
bool list_contains_size_t(CdsList *list, size_t value) {
    if (!list) return false;
    CdsListNode *current = list->head;
    while (current) {
        if (*(size_t *)current->data == value) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Test data for traversals
typedef struct {
    size_t visited_order[10]; // Max 10 vertices for test traversals
    size_t count;
} TraversalData;

void visit_callback(size_t vertex_id, void *user_data) {
    TraversalData *data = (TraversalData *)user_data;
    if (data->count < 10) {
        data->visited_order[data->count++] = vertex_id;
    }
}

bool test_graph_creation() {
    printf("Running test_graph_creation...\n");
    CdsGraph g_undirected = cds_graph_new(5, false);
    if (g_undirected.adj == NULL || g_undirected.num_vertices != 5 || g_undirected.directed != false) {
        fprintf(stderr, "test_graph_creation: Undirected graph creation failed.\n");
        cds_graph_delete(&g_undirected);
        return false;
    }
    for(size_t i=0; i<5; ++i) {
        if (g_undirected.adj[i].head != NULL || g_undirected.adj[i].size != 0 || g_undirected.adj[i].element_size != sizeof(size_t)) {
             fprintf(stderr, "test_graph_creation: Undirected graph adj list %zu not initialized correctly.\n", i);
             cds_graph_delete(&g_undirected);
             return false;
        }
    }
    cds_graph_delete(&g_undirected);

    CdsGraph g_directed = cds_graph_new(3, true);
    if (g_directed.adj == NULL || g_directed.num_vertices != 3 || g_directed.directed != true) {
        fprintf(stderr, "test_graph_creation: Directed graph creation failed.\n");
        cds_graph_delete(&g_directed);
        return false;
    }
    cds_graph_delete(&g_directed);

    CdsGraph g_zero = cds_graph_new(0, false);
     if (g_zero.adj != NULL || g_zero.num_vertices != 0) { // adj might be NULL or not, but num_vertices must be 0
        // if num_vertices is 0, adj could be NULL (if malloc(0*...) returns NULL) or a valid pointer to 0 allocated space.
        // The important part for a 0-vertex graph is that num_vertices is 0.
        // However, our cds_graph_new sets adj to NULL if num_vertices is 0 after allocation.
        if (g_zero.adj != NULL) { // Based on current implementation, it should be NULL for 0 vertices
            fprintf(stderr, "test_graph_creation: Zero vertex graph creation failed (adj not NULL).\n");
            cds_graph_delete(&g_zero);
            return false;
        }
    }
    cds_graph_delete(&g_zero);


    printf("test_graph_creation: Passed.\n");
    return true;
}

bool test_graph_add_edge_undirected() {
    printf("Running test_graph_add_edge_undirected...\n");
    CdsGraph g = cds_graph_new(3, false);
    if (cds_graph_add_edge(&g, 0, 1) != 0) {
        fprintf(stderr, "test_graph_add_edge_undirected: Failed to add edge 0-1.\n");
        cds_graph_delete(&g);
        return false;
    }
    if (cds_graph_add_edge(&g, 0, 2) != 0) {
        fprintf(stderr, "test_graph_add_edge_undirected: Failed to add edge 0-2.\n");
        cds_graph_delete(&g);
        return false;
    }

    // Check neighbors of 0
    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    CdsList *neighbors_0 = cds_graph_get_neighbors(&g, 0);
    if (neighbors_0 == NULL || !list_contains_size_t(neighbors_0, 1) || !list_contains_size_t(neighbors_0, 2) || neighbors_0->size != 2) {
        fprintf(stderr, "test_graph_add_edge_undirected: Vertex 0 neighbors incorrect.\n");
        cds_graph_delete(&g);
        return false;
    }

    // Check neighbors of 1 (should contain 0)
    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    CdsList *neighbors_1 = cds_graph_get_neighbors(&g, 1);
    if (neighbors_1 == NULL || !list_contains_size_t(neighbors_1, 0) || neighbors_1->size != 1) {
        fprintf(stderr, "test_graph_add_edge_undirected: Vertex 1 neighbors incorrect.\n");
        cds_graph_delete(&g);
        return false;
    }

    // Check neighbors of 2 (should contain 0)
    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    CdsList *neighbors_2 = cds_graph_get_neighbors(&g, 2);
    if (neighbors_2 == NULL || !list_contains_size_t(neighbors_2, 0) || neighbors_2->size != 1) {
        fprintf(stderr, "test_graph_add_edge_undirected: Vertex 2 neighbors incorrect.\n");
        cds_graph_delete(&g);
        return false;
    }

    // Test adding edge to non-existent vertex
    if (cds_graph_add_edge(&g, 0, 3) == 0) { // Should fail
        fprintf(stderr, "test_graph_add_edge_undirected: Added edge to non-existent vertex 3.\n");
        cds_graph_delete(&g);
        return false;
    }

    cds_graph_delete(&g);
    printf("test_graph_add_edge_undirected: Passed.\n");
    return true;
}

bool test_graph_add_edge_directed() {
    printf("Running test_graph_add_edge_directed...\n");
    CdsGraph g = cds_graph_new(3, true);
    cds_graph_add_edge(&g, 0, 1);
    cds_graph_add_edge(&g, 0, 2);
    cds_graph_add_edge(&g, 1, 2);

    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    CdsList *neighbors_0 = cds_graph_get_neighbors(&g, 0);
    if (neighbors_0 == NULL || !list_contains_size_t(neighbors_0, 1) || !list_contains_size_t(neighbors_0, 2) || neighbors_0->size != 2) {
        fprintf(stderr, "test_graph_add_edge_directed: Vertex 0 neighbors incorrect.\n");
        cds_graph_delete(&g);
        return false;
    }

    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    CdsList *neighbors_1 = cds_graph_get_neighbors(&g, 1);
    if (neighbors_1 == NULL || !list_contains_size_t(neighbors_1, 2) || list_contains_size_t(neighbors_1, 0) || neighbors_1->size != 1) {
        fprintf(stderr, "test_graph_add_edge_directed: Vertex 1 neighbors incorrect.\n");
        cds_graph_delete(&g);
        return false;
    }

    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    CdsList *neighbors_2 = cds_graph_get_neighbors(&g, 2);
    if (neighbors_2 == NULL || neighbors_2->size != 0) { // Vertex 2 has no outgoing edges
        fprintf(stderr, "test_graph_add_edge_directed: Vertex 2 neighbors incorrect (should be empty).\n");
        cds_graph_delete(&g);
        return false;
    }
    cds_graph_delete(&g);
    printf("test_graph_add_edge_directed: Passed.\n");
    return true;
}


bool test_graph_get_neighbors() {
    printf("Running test_graph_get_neighbors...\n");
    CdsGraph g = cds_graph_new(2, false);
    cds_graph_add_edge(&g, 0, 1);

    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    CdsList *neighbors_0 = cds_graph_get_neighbors(&g, 0);
    if (neighbors_0 == NULL || !list_contains_size_t(neighbors_0, 1) || neighbors_0->size != 1) {
        fprintf(stderr, "test_graph_get_neighbors: Error getting neighbors for vertex 0.\n");
        cds_graph_delete(&g);
        return false;
    }

    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    CdsList *neighbors_1 = cds_graph_get_neighbors(&g, 1);
    if (neighbors_1 == NULL || !list_contains_size_t(neighbors_1, 0) || neighbors_1->size != 1) {
        fprintf(stderr, "test_graph_get_neighbors: Error getting neighbors for vertex 1.\n");
        cds_graph_delete(&g);
        return false;
    }

    // If CDS_GRAPH_ADJ_USE_ARRAY is defined, the list returned by
    // cds_graph_get_neighbors must be freed. See NOTE at top of file.
    // (Though, for an invalid vertex, it should return NULL, so no freeing needed)
    CdsList *neighbors_invalid = cds_graph_get_neighbors(&g, 2); // Non-existent vertex
    if (neighbors_invalid != NULL) {
        fprintf(stderr, "test_graph_get_neighbors: Got neighbors for non-existent vertex.\n");
        cds_graph_delete(&g);
        return false;
    }

    cds_graph_delete(&g);
    printf("test_graph_get_neighbors: Passed.\n");
    return true;
}

bool test_dfs_traversal() {
    printf("Running test_dfs_traversal...\n");
    CdsGraph g = cds_graph_new(4, false); // Undirected graph
    // 0 -- 1
    // |    |
    // 2 -- 3
    cds_graph_add_edge(&g, 0, 1);
    cds_graph_add_edge(&g, 0, 2);
    cds_graph_add_edge(&g, 1, 3);
    cds_graph_add_edge(&g, 2, 3);


    bool visited[4];
    TraversalData data;

    // Expected: 0, 1, 3, 2 (one possible DFS order from 0) or 0, 2, 3, 1 etc.
    // We will check if all nodes are visited and the first node is correct.
    // Specific order depends on internal list order and recursion.
    // For this test, we'll check if all connected nodes are visited.

    memset(visited, 0, sizeof(visited));
    data.count = 0;
    cds_dfs(&g, 0, visited, visit_callback, &data);

    if (data.count != 4) {
        fprintf(stderr, "test_dfs_traversal: DFS from 0 did not visit all 4 nodes. Visited: %zu\n", data.count);
        cds_graph_delete(&g);
        return false;
    }
    for(size_t i=0; i<4; ++i) {
        if (!visited[i]) {
            fprintf(stderr, "test_dfs_traversal: Node %zu not marked as visited.\n", i);
            cds_graph_delete(&g);
            return false;
        }
    }
    if (data.visited_order[0] != 0) {
         fprintf(stderr, "test_dfs_traversal: DFS from 0 did not start with 0. Started with %zu\n", data.visited_order[0]);
        cds_graph_delete(&g);
        return false;
    }

    // Test DFS on a directed graph
    CdsGraph dg = cds_graph_new(3, true);
    // 0 -> 1 -> 2
    cds_graph_add_edge(&dg, 0, 1);
    cds_graph_add_edge(&dg, 1, 2);

    bool visited_dg[3];
    TraversalData data_dg;
    size_t expected_dg_order[] = {0, 1, 2};

    memset(visited_dg, 0, sizeof(visited_dg));
    data_dg.count = 0;
    cds_dfs(&dg, 0, visited_dg, visit_callback, &data_dg);

    if (data_dg.count != 3) {
        fprintf(stderr, "test_dfs_traversal: Directed DFS from 0 visited %zu nodes, expected 3.\n", data_dg.count);
        cds_graph_delete(&g);
        cds_graph_delete(&dg);
        return false;
    }
    for (size_t i = 0; i < 3; ++i) {
        if (data_dg.visited_order[i] != expected_dg_order[i]) {
            fprintf(stderr, "test_dfs_traversal: Directed DFS order mismatch. Expected %zu, got %zu at index %zu.\n", expected_dg_order[i], data_dg.visited_order[i], i);
            cds_graph_delete(&g);
            cds_graph_delete(&dg);
            return false;
        }
    }

    cds_graph_delete(&g);
    cds_graph_delete(&dg);
    printf("test_dfs_traversal: Passed.\n");
    return true;
}

bool test_bfs_traversal() {
    printf("Running test_bfs_traversal...\n");
    CdsGraph g = cds_graph_new(4, false); // Undirected graph
    // 0 -- 1
    // | \ / | (0-3 is also an edge)
    // 2 -- 3
    cds_graph_add_edge(&g, 0, 1);
    cds_graph_add_edge(&g, 0, 2);
    cds_graph_add_edge(&g, 0, 3); // Adding this to make BFS order more predictable from 0
    cds_graph_add_edge(&g, 1, 3);
    cds_graph_add_edge(&g, 2, 3);

    TraversalData data;
    data.count = 0;

    // Expected BFS from 0: 0, then (1,2,3 in some order), then deeper nodes if any.
    // With 0 connected to 1,2,3 directly:
    // Order: 0, then 1, 2, 3 (order among 1,2,3 depends on list insertion order)
    size_t expected_bfs_order_0[] = {0, 1, 2, 3}; // Assuming 1,2,3 pushed in order to 0's list.

    cds_bfs(&g, 0, visit_callback, &data);

    if (data.count != 4) {
        fprintf(stderr, "test_bfs_traversal: BFS from 0 visited %zu nodes, expected 4.\n", data.count);
        cds_graph_delete(&g);
        return false;
    }
    if (data.visited_order[0] != 0) {
        fprintf(stderr, "test_bfs_traversal: BFS from 0 did not start with 0.\n");
        cds_graph_delete(&g);
        return false;
    }

    // Check if 1, 2, 3 are present in data.visited_order[1], [2], [3]
    bool found1 = false, found2 = false, found3 = false;
    for(size_t i=1; i<4; ++i) {
        if(data.visited_order[i] == 1) found1=true;
        if(data.visited_order[i] == 2) found2=true;
        if(data.visited_order[i] == 3) found3=true;
    }
    if(!found1 || !found2 || !found3){
        fprintf(stderr, "test_bfs_traversal: BFS from 0 did not visit 1, 2, and 3 in the first level. Visited: %zu, %zu, %zu, %zu\n",
            data.visited_order[0], data.visited_order[1], data.visited_order[2], data.visited_order[3]);
        cds_graph_delete(&g);
        return false;
    }


    // Test BFS on a directed graph
    CdsGraph dg = cds_graph_new(4, true);
    // 0 -> 1 -> 3
    // | -> 2 -^
    cds_graph_add_edge(&dg, 0, 1);
    cds_graph_add_edge(&dg, 0, 2);
    cds_graph_add_edge(&dg, 1, 3);
    cds_graph_add_edge(&dg, 2, 3);

    TraversalData data_dg;
    data_dg.count = 0;
    // Expected from 0: 0, then (1,2), then 3
    // Order: 0, 1, 2, 3 (if 1 pushed before 2 for 0's list)

    cds_bfs(&dg, 0, visit_callback, &data_dg);
    if(data_dg.count != 4){
        fprintf(stderr, "test_bfs_traversal: Directed BFS from 0 visited %zu nodes, expected 4.\n", data_dg.count);
        cds_graph_delete(&g);
        cds_graph_delete(&dg);
        return false;
    }
    if(data_dg.visited_order[0] != 0){
        fprintf(stderr, "test_bfs_traversal: Directed BFS from 0 did not start with 0.\n");
        cds_graph_delete(&g);
        cds_graph_delete(&dg);
        return false;
    }
    // Check 1 and 2 are in pos 1 or 2
    if (!((data_dg.visited_order[1] == 1 && data_dg.visited_order[2] == 2) ||
          (data_dg.visited_order[1] == 2 && data_dg.visited_order[2] == 1))) {
        fprintf(stderr, "test_bfs_traversal: Directed BFS from 0, level 1 nodes incorrect. Got %zu, %zu.\n", data_dg.visited_order[1], data_dg.visited_order[2]);
        cds_graph_delete(&g);
        cds_graph_delete(&dg);
        return false;
    }
    if(data_dg.visited_order[3] != 3){
         fprintf(stderr, "test_bfs_traversal: Directed BFS from 0, level 2 node incorrect. Expected 3, got %zu.\n", data_dg.visited_order[3]);
        cds_graph_delete(&g);
        cds_graph_delete(&dg);
        return false;
    }

    cds_graph_delete(&g);
    cds_graph_delete(&dg);
    printf("test_bfs_traversal: Passed.\n");
    return true;
}

bool test_dfs_disconnected() {
    printf("Running test_dfs_disconnected...\n");
    CdsGraph g = cds_graph_new(5, false); // 0-1, 2-3, 4 is isolated
    cds_graph_add_edge(&g, 0, 1);
    cds_graph_add_edge(&g, 2, 3);

    bool visited[5];
    TraversalData data;

    // DFS from 0
    memset(visited, 0, sizeof(visited));
    data.count = 0;
    cds_dfs(&g, 0, visited, visit_callback, &data);
    if (data.count != 2 || !visited[0] || !visited[1] || visited[2] || visited[3] || visited[4]) {
        fprintf(stderr, "test_dfs_disconnected: DFS from 0 on disconnected graph failed. Count: %zu, Visited: %d %d %d %d %d\n", data.count, visited[0], visited[1], visited[2], visited[3], visited[4]);
        cds_graph_delete(&g);
        return false;
    }

    // DFS from 4 (isolated)
    memset(visited, 0, sizeof(visited));
    data.count = 0;
    cds_dfs(&g, 4, visited, visit_callback, &data);
    if (data.count != 1 || !visited[4] || visited[0] || visited[1] || visited[2] || visited[3]) {
        fprintf(stderr, "test_dfs_disconnected: DFS from 4 (isolated) on disconnected graph failed. Count: %zu, Visited: %d %d %d %d %d\n", data.count, visited[0], visited[1], visited[2], visited[3], visited[4]);
        cds_graph_delete(&g);
        return false;
    }

    // To traverse all components, caller would iterate and call cds_dfs if not visited.
    // Example:
    // memset(visited, 0, sizeof(visited));
    // int total_visited_count = 0;
    // for(size_t i=0; i<5; ++i) {
    //    if(!visited[i]) {
    //        TraversalData component_data = { .count = 0 };
    //        cds_dfs(&g, i, visited, visit_callback, &component_data);
    //        total_visited_count += component_data.count;
    //    }
    // }
    // if (total_visited_count != 5) { ... } // This would be a more complete graph traversal test

    cds_graph_delete(&g);
    printf("test_dfs_disconnected: Passed.\n");
    return true;
}

bool test_bfs_disconnected() {
    printf("Running test_bfs_disconnected...\n");
    CdsGraph g = cds_graph_new(5, false); // 0-1, 2-3, 4 is isolated
    cds_graph_add_edge(&g, 0, 1);
    cds_graph_add_edge(&g, 2, 3);

    TraversalData data;

    // BFS from 0
    data.count = 0;
    cds_bfs(&g, 0, visit_callback, &data);
    if (data.count != 2) { // Should only visit 0 and 1
        fprintf(stderr, "test_bfs_disconnected: BFS from 0 on disconnected graph visited %zu nodes, expected 2.\n", data.count);
        cds_graph_delete(&g);
        return false;
    }
    // Check if 0 and 1 are the ones visited (order might vary for the second element)
    bool found0 = false, found1 = false;
    for(size_t i=0; i < data.count; ++i) {
        if(data.visited_order[i] == 0) found0 = true;
        if(data.visited_order[i] == 1) found1 = true;
    }
    if(!found0 || !found1){
        fprintf(stderr, "test_bfs_disconnected: BFS from 0 did not visit 0 and 1. Visited: ");
        for(size_t i=0; i < data.count; ++i) fprintf(stderr, "%zu ", data.visited_order[i]);
        fprintf(stderr, "\n");
        cds_graph_delete(&g);
        return false;
    }


    // BFS from 4 (isolated)
    data.count = 0;
    cds_bfs(&g, 4, visit_callback, &data);
    if (data.count != 1 || data.visited_order[0] != 4) {
        fprintf(stderr, "test_bfs_disconnected: BFS from 4 (isolated) on disconnected graph failed. Count: %zu, Visited[0]: %zu\n", data.count, data.visited_order[0]);
        cds_graph_delete(&g);
        return false;
    }

    cds_graph_delete(&g);
    printf("test_bfs_disconnected: Passed.\n");
    return true;
}


bool test_dfs_empty_graph() {
    printf("Running test_dfs_empty_graph...\n");
    CdsGraph g = cds_graph_new(0, false);
    bool visited[] = {}; // Empty, or could be NULL if start_vertex is also checked against num_vertices
    TraversalData data = { .count = 0 };

    // Call DFS on an empty graph (num_vertices = 0).
    // start_vertex >= num_vertices will be true (e.g. 0 >= 0 for start_vertex 0)
    // The function should handle this gracefully (return early).
    cds_dfs(&g, 0, visited, visit_callback, &data);
    if (data.count != 0) {
        fprintf(stderr, "test_dfs_empty_graph: DFS on empty graph visited %zu nodes, expected 0.\n", data.count);
        cds_graph_delete(&g);
        return false;
    }

    CdsGraph g2 = cds_graph_new(1, false); // Single node, no edges
    bool visited2[1];
    memset(visited2, 0, sizeof(visited2));
    data.count = 0;
    cds_dfs(&g2, 0, visited2, visit_callback, &data);
    if (data.count != 1 || data.visited_order[0] != 0 || !visited2[0]) {
         fprintf(stderr, "test_dfs_empty_graph: DFS on single node graph failed. Count: %zu, Visited[0]: %zu, visited_flag: %d\n", data.count, data.visited_order[0], visited2[0]);
        cds_graph_delete(&g);
        cds_graph_delete(&g2);
        return false;
    }

    cds_graph_delete(&g);
    cds_graph_delete(&g2);
    printf("test_dfs_empty_graph: Passed.\n");
    return true;
}

bool test_bfs_empty_graph() {
    printf("Running test_bfs_empty_graph...\n");
    CdsGraph g = cds_graph_new(0, false);
    TraversalData data = { .count = 0 };

    // Call BFS on an empty graph.
    // start_vertex >= num_vertices will be true.
    // The function should handle this gracefully.
    cds_bfs(&g, 0, visit_callback, &data);
    if (data.count != 0) {
        fprintf(stderr, "test_bfs_empty_graph: BFS on empty graph visited %zu nodes, expected 0.\n", data.count);
        cds_graph_delete(&g);
        return false;
    }

    CdsGraph g2 = cds_graph_new(1, false); // Single node, no edges
    data.count = 0;
    cds_bfs(&g2, 0, visit_callback, &data);
    if (data.count != 1 || data.visited_order[0] != 0) {
         fprintf(stderr, "test_bfs_empty_graph: BFS on single node graph failed. Count: %zu, Visited[0]: %zu\n", data.count, data.visited_order[0]);
        cds_graph_delete(&g);
        cds_graph_delete(&g2);
        return false;
    }

    cds_graph_delete(&g);
    cds_graph_delete(&g2);
    printf("test_bfs_empty_graph: Passed.\n");
    return true;
}
