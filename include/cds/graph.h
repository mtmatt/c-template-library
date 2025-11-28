#ifndef CDS_GRAPH_H
#define CDS_GRAPH_H

#include "cds/list.h"
#include <stddef.h>

/*
 * Callback to get neighbors of a node.
 * Implementations should push neighbors into the provided 'neighbors' list.
 * The 'neighbors' list is initialized with the correct element_size before calling this.
 */
typedef void (*CdsGraphGetNeighborsFunc)(const void *node, struct cds_list *neighbors);

/*
 * Callback to visit a node.
 */
typedef void (*CdsGraphVisitFunc)(const void *node);

/*
 *********************************************************************************************************
 *
 *                                            CDS GRAPH BFS
 *
 * Description: Performs Breadth-First Search starting from a given node.
 *
 * Arguments: start_node     Pointer to the starting node.
 *            element_size   Size of the node data (in bytes).
 *            get_neighbors  Function to get neighbors of a node.
 *            visit          Function to visit/process a node.
 *            cmp            Comparison function for nodes (used to track visited nodes).
 *                           Returns <0, 0, >0.
 *
 * Returns: none
 *
 * Notes: Uses CdsQueue for BFS queue and CdsRbTree for visited set.
 *********************************************************************************************************
 */
void cds_graph_bfs(const void *start_node,
                   size_t element_size,
                   CdsGraphGetNeighborsFunc get_neighbors,
                   CdsGraphVisitFunc visit,
                   int (*cmp)(const void *, const void *));

/*
 *********************************************************************************************************
 *
 *                                            CDS GRAPH DFS
 *
 * Description: Performs Depth-First Search starting from a given node.
 *
 * Arguments: start_node     Pointer to the starting node.
 *            element_size   Size of the node data (in bytes).
 *            get_neighbors  Function to get neighbors of a node.
 *            visit          Function to visit/process a node.
 *            cmp            Comparison function for nodes (used to track visited nodes).
 *                           Returns <0, 0, >0.
 *
 * Returns: none
 *
 * Notes: Uses CdsStack for DFS stack and CdsRbTree for visited set.
 *********************************************************************************************************
 */
void cds_graph_dfs(const void *start_node,
                   size_t element_size,
                   CdsGraphGetNeighborsFunc get_neighbors,
                   CdsGraphVisitFunc visit,
                   int (*cmp)(const void *, const void *));

#endif
