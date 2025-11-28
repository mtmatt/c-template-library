#include "test_graph.h"
#include "cds/graph.h"
#include "cds/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Simple graph representation for testing
// Nodes are integers 0 to N-1
#define MAX_NODES 10
int adjacency_matrix[MAX_NODES][MAX_NODES];
int visited_order[MAX_NODES];
int visited_count = 0;

void clear_graph() {
    for (int i = 0; i < MAX_NODES; ++i) {
        for (int j = 0; j < MAX_NODES; ++j) {
            adjacency_matrix[i][j] = 0;
        }
    }
}

void add_edge(int u, int v) {
    if (u < MAX_NODES && v < MAX_NODES) {
        adjacency_matrix[u][v] = 1;
        // Directed graph for simplicity, or undirected? Let's assume directed for flexibility.
    }
}

void get_neighbors_int(const void *node, struct cds_list *neighbors) {
    int u = *(const int*)node;
    for (int v = 0; v < MAX_NODES; ++v) {
        if (adjacency_matrix[u][v]) {
            cds_list_push_back(neighbors, &v);
        }
    }
}

void visit_int(const void *node) {
    int u = *(const int*)node;
    if (visited_count < MAX_NODES) {
        visited_order[visited_count++] = u;
    }
    // printf("Visited: %d\n", u);
}

int cmp_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return ia - ib;
}

void test_bfs() {
    printf("Testing BFS...\n");
    clear_graph();
    visited_count = 0;

    /*
       0 -> 1
       0 -> 2
       1 -> 3
       2 -> 4

       BFS from 0: 0, 1, 2, 3, 4 (order of 1 and 2 depends on implementation/insertion order)
    */
    add_edge(0, 1);
    add_edge(0, 2);
    add_edge(1, 3);
    add_edge(2, 4);

    int start = 0;
    cds_graph_bfs(&start, sizeof(int), get_neighbors_int, visit_int, cmp_int);

    assert(visited_count == 5);
    assert(visited_order[0] == 0);
    // 1 and 2 are neighbors of 0.
    assert(visited_order[1] == 1 || visited_order[1] == 2);
    assert(visited_order[2] == 1 || visited_order[2] == 2);
    assert(visited_order[1] != visited_order[2]);

    // 3 and 4 are next level
    assert(visited_order[3] == 3 || visited_order[3] == 4);
    assert(visited_order[4] == 3 || visited_order[4] == 4);

    printf("BFS Passed\n");
}

void test_dfs() {
    printf("Testing DFS...\n");
    clear_graph();
    visited_count = 0;

    /*
       0 -> 1
       0 -> 2
       1 -> 3
       2 -> 4

       DFS from 0:
       0
       stack: [1, 2] (if 1 pushed first then 2, top is 2) -> visit 2
       stack: [1, 4] -> visit 4
       stack: [1] -> visit 1
       stack: [3] -> visit 3
       Order: 0, 2, 4, 1, 3 (if neighbors iterated 1, 2 and pushed)

       Wait, my implementation pushes neighbors in reverse order (tail to head).
       get_neighbors iterates 0 to MAX_NODES. So neighbors are pushed 1, then 2 (if 1<2).
       But wait, get_neighbors pushes back to list. List is [1, 2].
       DFS implementation iterates tail to head: 2, then 1.
       Pushes 2, then 1.
       Stack top is 1.
       So visit 1.
       Neighbors of 1 is [3].
       Iterate tail to head: 3.
       Push 3.
       Stack top is 3.
       Visit 3.
       Stack top is 2.
       Visit 2.
       Neighbors of 2 is [4].
       Push 4.
       Stack top is 4.
       Visit 4.

       So order: 0, 1, 3, 2, 4.
    */
    add_edge(0, 1);
    add_edge(0, 2);
    add_edge(1, 3);
    add_edge(2, 4);

    int start = 0;
    cds_graph_dfs(&start, sizeof(int), get_neighbors_int, visit_int, cmp_int);

    assert(visited_count == 5);
    assert(visited_order[0] == 0);

    // Based on logic above
    if (visited_order[1] == 1) {
        assert(visited_order[2] == 3);
        assert(visited_order[3] == 2);
        assert(visited_order[4] == 4);
    } else {
        // If logic reversed
        assert(visited_order[1] == 2);
        assert(visited_order[2] == 4);
        assert(visited_order[3] == 1);
        assert(visited_order[4] == 3);
    }

    printf("DFS Passed\n");
}

void test_cycle() {
    printf("Testing Cycle handling...\n");
    clear_graph();
    visited_count = 0;

    // 0 -> 1 -> 0
    add_edge(0, 1);
    add_edge(1, 0);

    int start = 0;
    cds_graph_dfs(&start, sizeof(int), get_neighbors_int, visit_int, cmp_int);

    assert(visited_count == 2);
    assert(visited_order[0] == 0);
    assert(visited_order[1] == 1);

    printf("Cycle Test Passed\n");
}

void test_graph(void) {
    test_bfs();
    test_dfs();
    test_cycle();
}
