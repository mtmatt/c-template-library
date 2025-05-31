#include <calgo/algos.h>
#include <cds/stack.h>  // For CdsStack in DFS
#include <cds/queue.h>  // For CdsQueue in BFS
#include <stdlib.h>     // For malloc, free (for visited array in BFS)
#include <stdio.h>      // For perror

// Recursive helper for DFS
static void cds_dfs_recursive_helper(CdsGraph *graph, size_t u, bool *visited_nodes, cds_visit_callback callback, void *user_data) {
    visited_nodes[u] = true;
    if (callback) {
        callback(u, user_data);
    }

    CdsList *neighbors = cds_graph_get_neighbors(graph, u);
    if (neighbors == NULL) {
        return; // Should not happen if graph is valid and u is a valid vertex
    }

    CdsListNode *current_neighbor_node = neighbors->head;
    while (current_neighbor_node != NULL) {
        size_t v = *(size_t *)(current_neighbor_node->data); // Assuming data stores size_t vertex ids
        if (!visited_nodes[v]) {
            cds_dfs_recursive_helper(graph, v, visited_nodes, callback, user_data);
        }
        current_neighbor_node = current_neighbor_node->next;
    }
}

void cds_dfs(CdsGraph *graph, size_t start_vertex, bool *visited_nodes, cds_visit_callback callback, void *user_data) {
    if (graph == NULL || graph->adj == NULL || start_vertex >= graph->num_vertices || visited_nodes == NULL) {
        return; // Invalid input
    }

    // For a typical DFS that might be called multiple times on disconnected components,
    // the visited array is managed by the caller.
    // This function performs DFS from a single start_vertex.
    if (!visited_nodes[start_vertex]) {
       cds_dfs_recursive_helper(graph, start_vertex, visited_nodes, callback, user_data);
    }
}

void cds_bfs(CdsGraph *graph, size_t start_vertex, cds_visit_callback callback, void *user_data) {
    if (graph == NULL || graph->adj == NULL || start_vertex >= graph->num_vertices) {
        return; // Invalid input
    }

    bool *visited = calloc(graph->num_vertices, sizeof(bool));
    if (visited == NULL) {
        perror("Failed to allocate memory for visited array in BFS");
        return;
    }

    CdsQueue queue = cds_queue_new(sizeof(size_t));
    if (queue.data == NULL && graph->num_vertices > 0) { // Check if queue creation failed (data is CdsArray)
        perror("Failed to create queue in BFS");
        free(visited);
        return;
    }

    visited[start_vertex] = true;
    if (cds_queue_enqueue(&queue, &start_vertex) != 0) {
        perror("Failed to enqueue start_vertex in BFS");
        cds_queue_delete(&queue);
        free(visited);
        return;
    }

    while (!cds_queue_empty(&queue)) {
        size_t u_val;
        // It's safer to get a pointer to the data and then copy it.
        void* front_data_ptr = cds_queue_front(&queue);
        if (front_data_ptr == NULL) { // Should not happen if queue is not empty
             if (!cds_queue_empty(&queue)) { // only print if queue is not empty
                perror("Failed to get front element from queue in BFS");
             }
             cds_queue_delete(&queue);
             free(visited);
             return;
        }
        u_val = *(size_t*)front_data_ptr;

        if (cds_queue_dequeue(&queue) != 0) {
            perror("Failed to dequeue in BFS");
            cds_queue_delete(&queue);
            free(visited);
            return;
        }

        if (callback) {
            callback(u_val, user_data);
        }

        CdsList *neighbors = cds_graph_get_neighbors(graph, u_val);
        if (neighbors == NULL) continue; // Should ideally not happen for a valid vertex

        CdsListNode *current_neighbor_node = neighbors->head;
        while (current_neighbor_node != NULL) {
            size_t v = *(size_t *)(current_neighbor_node->data);
            if (!visited[v]) {
                visited[v] = true;
                if (cds_queue_enqueue(&queue, &v) != 0) {
                    perror("Failed to enqueue neighbor in BFS");
                    // Potentially partial traversal, clean up and exit
                    cds_queue_delete(&queue);
                    free(visited);
                    return;
                }
            }
            current_neighbor_node = current_neighbor_node->next;
        }
    }

    cds_queue_delete(&queue);
    free(visited);
}
