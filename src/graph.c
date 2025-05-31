#include <cds/graph.h>
#include <cds/list.h>      // Always needed for cds_graph_get_neighbors API and array conversion
#ifdef CDS_GRAPH_ADJ_USE_ARRAY
#include <cds/array.h> // Specifically for array implementation
#endif
#include <stdlib.h>
#include <stdio.h>

CdsGraph cds_graph_new(size_t num_vertices, bool directed) {
  CdsGraph graph;
  graph.num_vertices = num_vertices;
  graph.directed = directed;

  // If num_vertices is 0, malloc(0) behavior is implementation-defined.
  // It might return NULL or a pointer that can't be dereferenced.
  // For consistency, handle 0 vertices by not allocating adj.
  if (num_vertices == 0) {
    graph.adj = NULL; // Or consider a special sentinel if needed, but NULL is fine for 0 vertices.
    return graph;
  }

#ifdef CDS_GRAPH_ADJ_USE_ARRAY
  graph.adj = malloc(num_vertices * sizeof(CdsArray));
#else
  graph.adj = malloc(num_vertices * sizeof(CdsList));
#endif

  if (graph.adj == NULL) {
    perror("Failed to allocate memory for adjacency lists/arrays container");
    graph.num_vertices = 0; // Indicate failure
    return graph;
  }

  for (size_t i = 0; i < num_vertices; ++i) {
#ifdef CDS_GRAPH_ADJ_USE_ARRAY
    graph.adj[i] = cds_array_new(sizeof(size_t)); // Each array stores vertex IDs
    // If element_size > 0 (which sizeof(size_t) is), cds_array_new attempts an initial allocation.
    // If that fails, data will be NULL.
    if (graph.adj[i].data == NULL && graph.adj[i].element_size > 0) {
        perror("cds_array_new failed for an adjacency CdsArray");
        // Cleanup previously allocated arrays
        for(size_t k=0; k<i; ++k) {
            cds_array_delete(&(graph.adj[k]));
        }
        free(graph.adj);
        graph.adj = NULL;
        graph.num_vertices = 0; // Indicate failure
        return graph;
    }
#else
    graph.adj[i] = cds_list_new(sizeof(size_t)); // Each list stores vertex IDs
    // CdsList_new returns by value; internal allocations (if any for the list struct itself) are not checked here.
    // Node allocations happen during push_back.
#endif
  }
  return graph;
}

void cds_graph_delete(CdsGraph *graph) {
  if (graph == NULL || graph->adj == NULL || graph->num_vertices == 0) {
    // If num_vertices is 0, graph->adj might be NULL (if graph_new handled 0 vertices that way)
    // or not, depending on malloc(0) behavior if it wasn't explicitly handled.
    // If adj is NULL, nothing to free here for adj elements.
    if (graph && graph->num_vertices == 0 && graph->adj == NULL) {
        // This is a valid state for a 0-vertex graph after graph_new.
        // No further cleanup of adj elements needed. Freeing graph->adj itself happens below if not NULL.
    } else if (graph == NULL || graph->adj == NULL) { // Handles cases where graph might be non-NULL but adj is.
        return;
    }
  }


  for (size_t i = 0; i < graph->num_vertices; ++i) {
    // It's possible adj[i] itself might not be valid if num_vertices was
    // changed without proper reallocation, but standard usage assumes valid elements.
#ifdef CDS_GRAPH_ADJ_USE_ARRAY
    cds_array_delete(&(graph->adj[i]));
#else
    cds_list_delete(&(graph->adj[i]));
#endif
  }

  // Free the container for adjacency lists/arrays itself
  // This was allocated by malloc(num_vertices * sizeof(CdsList/CdsArray))
  // This should only be freed if num_vertices > 0 at the time of allocation.
  // If graph.adj is NULL (e.g. for a 0-vertex graph or if initial malloc failed), free(NULL) is safe.
  free(graph->adj);
  graph->adj = NULL;
  graph->num_vertices = 0;
}

int cds_graph_add_edge(CdsGraph *graph, size_t u, size_t v) {
  if (graph == NULL || graph->adj == NULL) {
    fprintf(stderr, "cds_graph_add_edge: Graph not properly initialized.\n");
    return -1;
  }
  if (u >= graph->num_vertices || v >= graph->num_vertices) {
    fprintf(stderr, "cds_graph_add_edge: Vertex out of bounds (u=%zu, v=%zu, num_vertices=%zu).\n", u, v, graph->num_vertices);
    return -1;
  }

#ifdef CDS_GRAPH_ADJ_USE_ARRAY
  if (cds_array_push_back(&(graph->adj[u]), &v) != 0) {
    perror("Failed to add edge from u to v (array backend)");
    return -1;
  }
  if (!graph->directed) {
    if (cds_array_push_back(&(graph->adj[v]), &u) != 0) {
      perror("Failed to add edge from v to u for undirected graph (array backend)");
      // NOTE: Rollback of the first push_back is not implemented for simplicity.
      // This could leave the graph in an inconsistent state for this edge.
      return -1;
    }
  }
#else
  if (cds_list_push_back(&(graph->adj[u]), &v) != 0) {
    perror("Failed to add edge from u to v (list backend)");
    return -1;
  }
  if (!graph->directed) {
    if (cds_list_push_back(&(graph->adj[v]), &u) != 0) {
      perror("Failed to add edge from v to u for undirected graph (list backend)");
      // NOTE: Rollback of the first push_back is not implemented for simplicity.
      return -1;
    }
  }
#endif
  return 0;
}

CdsList* cds_graph_get_neighbors(const CdsGraph *graph, size_t v_idx) {
  if (graph == NULL || graph->adj == NULL || v_idx >= graph->num_vertices) {
    return NULL;
  }

#ifdef CDS_GRAPH_ADJ_USE_ARRAY
  // Create a new CdsList from the CdsArray. Caller must free this list.
  CdsList *list_to_return = (CdsList*)malloc(sizeof(CdsList));
  if (!list_to_return) {
    perror("Failed to allocate CdsList shell in get_neighbors (array_backend)");
    return NULL;
  }
  *list_to_return = cds_list_new(sizeof(size_t)); // Initialize it

  // Check if list_new effectively failed (e.g. if it had internal allocs that are not apparent)
  // For current CdsList, it's simple initialization, so this check is mostly for future-proofing
  // or if CdsList changes. A more direct failure from cds_list_new would be better.
  // For now, we assume cds_list_new itself doesn't fail to initialize the struct members.

  CdsArray *array_of_neighbors = &(graph->adj[v_idx]);
  for (size_t i = 0; i < cds_array_size(array_of_neighbors); ++i) {
    void *elem_ptr = cds_array_at(array_of_neighbors, i);
    if (elem_ptr == NULL) {
        // This case should ideally not be reached if array_of_neighbors is consistent.
        perror("NULL element pointer from cds_array_at in get_neighbors");
        cds_list_delete(list_to_return);
        free(list_to_return);
        return NULL;
    }
    if (cds_list_push_back(list_to_return, elem_ptr) != 0) {
      perror("Failed to push_back to CdsList in get_neighbors (array_backend)");
      cds_list_delete(list_to_return);
      free(list_to_return);
      return NULL;
    }
  }
  return list_to_return;
#else
  // Return a direct pointer to the internal list. Caller must not free this one.
  // Casting away const here is not ideal but often done in C for such getters if modification is complex.
  // The header documentation clearly states not to modify it.
  return (CdsList*)&(graph->adj[v_idx]);
#endif
}
