/*
 * initMapping.h
 *
 */

#ifndef INIT_INITMAPPING_H_
#define INIT_INITMAPPING_H_

#include <stdio.h>
#include "driverlib.h"

struct node {
  int vertex;
  struct node* next;

};

struct Graph {
  int numVertices;
  int* visited;

  // We need int** to store a two dimensional array.
  // Similary, we need struct node** to store an array of Linked lists
  struct node** adjLists;
};


struct stack{
    int items[16];
    int size;
    int top;
};

void initMap(void);
void addEdge(struct Graph* graph, int src, int dest);
struct node* createNode(int v);
struct Graph* createGraph(int vertices);
void printGraph(struct Graph* graph);


#endif /* INIT_INITMAPPING_H_ */
