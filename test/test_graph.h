#ifndef TEST_GRAPH_H
#define TEST_GRAPH_H

#include <stdbool.h>

bool test_graph_creation();
bool test_graph_add_edge_undirected();
bool test_graph_add_edge_directed();
bool test_graph_get_neighbors();
bool test_dfs_traversal();
bool test_bfs_traversal();
bool test_dfs_disconnected();
bool test_bfs_disconnected();
bool test_dfs_empty_graph();
bool test_bfs_empty_graph();

#endif // TEST_GRAPH_H
