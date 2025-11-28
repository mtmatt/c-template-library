#include "cds/graph.h"
#include "cds/queue.h"
#include "cds/stack.h"
#include "cds/rb_tree.h"
#include "cds/list.h"
#include <stdlib.h>
#include <string.h>

void cds_graph_bfs(const void *start_node,
                   size_t element_size,
                   CdsGraphGetNeighborsFunc get_neighbors,
                   CdsGraphVisitFunc visit,
                   int (*cmp)(const void *, const void *)) {
    if (!start_node || !get_neighbors || !visit || !cmp) {
        return;
    }

    struct cds_queue queue = cds_queue_new(element_size);
    struct cds_rb_tree visited = cds_rb_tree_new(element_size, element_size, cmp); // Key is node itself

    // Add start node to queue and visited set
    cds_queue_push(&queue, start_node);
    cds_rb_tree_insert(&visited, start_node, start_node);

    void *current_node = malloc(element_size);
    if (!current_node) {
        cds_queue_delete(&queue);
        cds_rb_tree_delete(&visited);
        return;
    }

    while (!cds_queue_empty(&queue)) {
        // Dequeue current node
        void *front = cds_queue_front(&queue);
        if (front) {
            memcpy(current_node, front, element_size);
            cds_queue_pop(&queue);

            // Visit current node
            visit(current_node);

            // Get neighbors
            struct cds_list neighbors = cds_list_new(element_size);
            get_neighbors(current_node, &neighbors);

            struct cds_list_node *node = neighbors.head;
            while (node) {
                if (cds_rb_tree_find(&visited, node->data) == NULL) {
                    cds_rb_tree_insert(&visited, node->data, node->data);
                    cds_queue_push(&queue, node->data);
                }
                node = node->next;
            }
            cds_list_delete(&neighbors);
        } else {
            // Should not happen if check empty
            cds_queue_pop(&queue);
        }
    }

    free(current_node);
    cds_queue_delete(&queue);
    cds_rb_tree_delete(&visited);
}

void cds_graph_dfs(const void *start_node,
                   size_t element_size,
                   CdsGraphGetNeighborsFunc get_neighbors,
                   CdsGraphVisitFunc visit,
                   int (*cmp)(const void *, const void *)) {
    if (!start_node || !get_neighbors || !visit || !cmp) {
        return;
    }

    struct cds_stack stack = cds_stack_new(element_size);
    struct cds_rb_tree visited = cds_rb_tree_new(element_size, element_size, cmp);

    // Push start node to stack and mark as visited?
    // Standard DFS (iterative) usually pushes start node.
    // Visiting happens when popping.

    cds_stack_push(&stack, start_node);

    // Note: If we mark as visited upon pushing, it's more like BFS.
    // For DFS, we usually mark as visited upon pushing to stack to avoid infinite loops with cycles immediately,
    // OR mark as visited when popping.
    // If we mark when popping, we might push the same node multiple times.
    // A common way for iterative DFS:
    // Push start.
    // While stack not empty:
    //   Pop u.
    //   If u not visited:
    //     Visit u.
    //     Mark u visited.
    //     For each neighbor v:
    //       Push v.

    // Let's implement this version as it handles cycles correctly and is standard.

    void *current_node = malloc(element_size);
    if (!current_node) {
        cds_stack_delete(&stack);
        cds_rb_tree_delete(&visited);
        return;
    }

    while (!cds_stack_empty(&stack)) {
        void *top = cds_stack_top(&stack);
        if (top) {
            memcpy(current_node, top, element_size);
            cds_stack_pop(&stack);

            if (cds_rb_tree_find(&visited, current_node) == NULL) {
                visit(current_node);
                cds_rb_tree_insert(&visited, current_node, current_node);

                struct cds_list neighbors = cds_list_new(element_size);
                get_neighbors(current_node, &neighbors);

                // For DFS, to visit neighbors in order (if order matters), we should push them in reverse order
                // so that the first neighbor is popped first.
                // But the list is doubly linked, so we can iterate from tail to head.

                struct cds_list_node *node = neighbors.tail;
                while (node) {
                    // Optimization: if neighbor is already visited, don't push?
                    // It saves stack space.
                    if (cds_rb_tree_find(&visited, node->data) == NULL) {
                        cds_stack_push(&stack, node->data);
                    }
                    node = node->prev;
                }
                cds_list_delete(&neighbors);
            }
        } else {
             cds_stack_pop(&stack);
        }
    }

    free(current_node);
    cds_stack_delete(&stack);
    cds_rb_tree_delete(&visited);
}
