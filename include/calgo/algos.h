#ifndef CALGO_ALGOS_H
#define CALGO_ALGOS_H

#include <cds/graph.h> // For CdsGraph
#include <stdbool.h> // For bool
#include <stddef.h>  // For size_t

// Callback function type for processing visited vertices
typedef void (*cds_visit_callback)(size_t vertex_id, void *user_data);

/*
 *********************************************************************************************************
 *
 *                                                CDS DFS
 *
 * Description: Performs a Depth-First Search (DFS) on the given graph starting from a specified vertex.
 *
 * Arguments: graph          A pointer to the struct cds_graph instance.
 *            start_vertex   The vertex from which to start the DFS.
 *            visited_nodes  A boolean array to keep track of visited vertices. Its size must be at least
 *                           graph->num_vertices.
 *            callback       A function pointer to be called for each visited vertex.
 *            user_data      User-defined data to be passed to the callback function.
 *
 * Returns: none
 *
 * Notes: The caller is responsible for initializing the `visited_nodes` array to all false before
 *        calling this function for the first time in a traversal.
 *        The DFS will only visit nodes reachable from `start_vertex` that have not been visited yet.
 *********************************************************************************************************
 */
void cds_dfs(CdsGraph *graph, size_t start_vertex, bool *visited_nodes, cds_visit_callback callback, void *user_data);

/*
 *********************************************************************************************************
 *
 *                                                CDS BFS
 *
 * Description: Performs a Breadth-First Search (BFS) on the given graph starting from a specified vertex.
 *
 * Arguments: graph          A pointer to the struct cds_graph instance.
 *            start_vertex   The vertex from which to start the BFS.
 *            callback       A function pointer to be called for each visited vertex.
 *            user_data      User-defined data to be passed to the callback function.
 *
 * Returns: none
 *
 * Notes: The BFS will only visit nodes reachable from `start_vertex`.
 *        The function handles tracking of visited nodes internally.
 *********************************************************************************************************
 */
void cds_bfs(CdsGraph *graph, size_t start_vertex, cds_visit_callback callback, void *user_data);

#endif // CALGO_ALGOS_H
