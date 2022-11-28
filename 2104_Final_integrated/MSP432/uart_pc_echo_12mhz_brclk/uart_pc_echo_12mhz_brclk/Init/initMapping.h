/*
 * initMapping.h
 *
 */

#ifndef INIT_INITMAPPING_H_
#define INIT_INITMAPPING_H_

#include <stdio.h>
#include "driverlib.h"

#define SIZE 40

extern int done;
extern unsigned char * string[20];
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

struct queue {
  int items[40];
  int front;
  int rear;
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

struct queue* createQueue();
void enqueue(struct queue* q, int);
int dequeue(struct queue* q);
void display(struct queue* q);
int isEmpty(struct queue* q);
void printQueue(struct queue* q);
int bfs(struct Graph* graph, int startVertex, int endVertex, int pred[], int dist[]);
void printShortestPath(struct Graph* graph, int startVertex, int endVertex);


#endif /* INIT_INITMAPPING_H_ */
