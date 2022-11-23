/*
 * initMapping.h
 *
 */

#ifndef INIT_INITMAPPING_H_
#define INIT_INITMAPPING_H_

#include <stdio.h>
#include "driverlib.h"

struct directionlist {
    char direction;
    struct directionlist* next;
    struct directionlist* prev;
};

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
void checkdirectionsensor(struct Graph* graph,int currentvertex);
void checkdirectionmovement(struct Graph* graph,int currentvertex,int nextvertex);
void createDirectionList(void);
void printList(struct directionlist* node);
void push(struct directionlist** head_ref, char new_data);
void append(struct directionlist** head_ref, int new_data);
void linkfrontback(struct directionlist* node);
void initMap(void);
void addEdge(struct Graph* graph, int src, int dest);
struct node* createNode(int v);
struct Graph* createGraph(int vertices);
void printGraph(struct Graph* graph);

#endif /* INIT_INITMAPPING_H_ */
