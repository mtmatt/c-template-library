#ifndef CDS_GRAPH_H
#define CDS_GRAPH_H

#include <cds/list.h> // For CdsList in API and default impl.
#include <stddef.h>   // For size_t
#include <stdbool.h>  // For bool

// Define CDS_GRAPH_ADJ_USE_ARRAY to use CdsArray for adjacency lists.
// By default, CdsList is used.
// #define CDS_GRAPH_ADJ_USE_ARRAY

#ifdef CDS_GRAPH_ADJ_USE_ARRAY
#include <cds/array.h> // Needed for CdsArray if using array impl
#endif

// Represents an adjacency list graph
typedef struct cds_graph {
#ifdef CDS_GRAPH_ADJ_USE_ARRAY
  CdsArray *adj; // Array of CdsArrays, each storing size_t vertex IDs
#else
  CdsList *adj;  // Array of CdsLists, each storing size_t vertex IDs
#endif
  size_t num_vertices;
  bool directed;
} CdsGraph;

/*
 *********************************************************************************************************
 *
 *                                            CDS GRAPH NEW
 *
 * Description: Creates a new graph with a specified number of vertices.
 *
 * Arguments: num_vertices   The number of vertices in the graph.
 *            directed       True if the graph is directed, false otherwise.
 *
 * Returns: A newly created struct cds_graph instance. The adj field can be NULL if memory allocation
 *          fails.
 *
 * Notes: The caller is responsible for freeing the memory allocated for the graph using cds_graph_delete.
 *        Each adjacency list is initialized to be empty.
 *********************************************************************************************************
 */
CdsGraph cds_graph_new(size_t num_vertices, bool directed);

/*
 *********************************************************************************************************
 *
 *                                            CDS GRAPH DELETE
 *
 * Description: Frees the memory allocated for the graph, including all adjacency lists.
 *
 * Arguments: graph   A pointer to the struct cds_graph instance to be deleted.
 *
 * Returns: none
 *
 * Notes: The caller should ensure that the graph pointer is not NULL before calling this function.
 *********************************************************************************************************
 */
void cds_graph_delete(CdsGraph *graph);

/*
 *********************************************************************************************************
 *
 *                                            CDS GRAPH ADD EDGE
 *
 * Description: Adds an edge between two vertices in the graph.
 *              If the graph is undirected, an edge is added in both directions.
 *
 * Arguments: graph   A pointer to the struct cds_graph instance.
 *            u       The source vertex.
 *            v       The destination vertex.
 *
 * Returns: 0 on success, -1 on failure (e.g., if memory allocation fails or vertices are out of bounds).
 *
 * Notes: This function assumes that the vertices u and v are valid (i.e., within the range
 *        [0, num_vertices - 1]).
 *********************************************************************************************************
 */
int cds_graph_add_edge(CdsGraph *graph, size_t u, size_t v);

/*
 *********************************************************************************************************
 *
 *                                        CDS GRAPH GET NEIGHBORS
 *
 * Description: Retrieves the list of neighbors for a given vertex.
 *
 * Arguments: graph   A pointer to the struct cds_graph instance.
 *            v       The vertex whose neighbors are to be retrieved.
 *
 * Returns: A pointer to a CdsList of neighbors, or NULL if the vertex is out of bounds or on error.
 *
 * Notes: Behavior depends on the underlying adjacency list implementation:
 *        - Default (CdsList based): Returns a direct pointer to the internal adjacency list.
 *          The caller should not modify this list directly. If modifications are needed, a copy
 *          should be made. The list does not need to be freed by the caller.
 *        - Array based (if CDS_GRAPH_ADJ_USE_ARRAY is defined): Returns a *newly allocated* CdsList,
 *          populated from the internal CdsArray. The caller IS RESPONSIBLE for calling `cds_list_delete()`
 *          on this returned list.
 *********************************************************************************************************
 */
CdsList* cds_graph_get_neighbors(const CdsGraph *graph, size_t v);

#endif // CDS_GRAPH_H
