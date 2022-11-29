/*
 * initMapping.c
 *

 */

#include <stdio.h>
#include <stdlib.h>
#include "initMapping.h"
#include "initMovement.h"

// Function prototypes
void push();       // Push element to the top of the stack
void pop();         // Remove and return the top most element of the stack
int peek();        // Return the top most element of the stack
int isEmpty();    // Check if the stack is in Underflow state or not
int isFull();

// DFS algo
void DFS(struct Graph* graph, int vertex) {

    //add edge first
//    printf("press e to exit\n");
//    char iSelection = 'a';
//    while( iSelection != 'e' ) {
//        int currentvertex1;
//        int currentvertex2;
//        //put where it currently is
//        printf("Enter an vertex 1: \n");
////        scanf("%d", &currentvertex1);
//        //put where the next node is at
//        printf("Enter an vertex 2: \n");
////        scanf("%d", &currentvertex2);
//        addEdge(graph, currentvertex1, currentvertex2);
//        printf("choose to exit press e any key to continue\n");
////        scanf(" %c", &iSelection);
//    }

  struct node* adjList = graph->adjLists[vertex];
  struct node* temp = adjList;


  // indicate which vertex(point has been visited)
  graph->visited[vertex] = 1;
  printf("Visited %d \n", vertex);



  while (temp != NULL) {
    int connectedVertex = temp->vertex;
    //current vertex is to track which block on the map that it has to travel
    //current vertex to show where the robot is at
    printf("current %d\n",vertex);
    //connected vertex can be track to next block to travel(may have issues?)
    printf("connected %d\n",connectedVertex);
    if (graph->visited[connectedVertex] == 0) {
        printf("robot move here %d\n",connectedVertex);
        //get currentlocation with Vertex
        //check if current location == connected vertex
        //if(so do nothing)
        //else(call motor move method here)
        DFS(graph, connectedVertex);

    }
    temp = temp->next;
    //graph->adjlist[vertex]
    //printf("%d\n",connectedVertex);
    /*
    here connectedvertex will go to a visted adj location until it goes to a
    non visted location
    call motor move method here
    */
  }

}

// Create a node
struct node* createNode(int v) {
  struct node* newNode = malloc(sizeof(struct node));
  newNode->vertex = v;
  newNode->next = NULL;
  return newNode;
}

// Create graph
struct Graph* createGraph(int vertices) {
  struct Graph* graph = malloc(sizeof(struct Graph));
  graph->numVertices = vertices;

  graph->adjLists = malloc(vertices * sizeof(struct node*));

  graph->visited = malloc(vertices * sizeof(int));

  int i;
  for (i = 0; i < vertices; i++) {
    graph->adjLists[i] = NULL;
    graph->visited[i] = 0;
  }
  return graph;
}

// Add edge
void addEdge(struct Graph* graph, int src, int dest) {
  // Add edge from src to dest
  struct node* newNode = createNode(dest);
  newNode->next = graph->adjLists[src];
  graph->adjLists[src] = newNode;

  // Add edge from dest to src
  newNode = createNode(src);
  newNode->next = graph->adjLists[dest];
  graph->adjLists[dest] = newNode;
}

// Print the graph
void printGraph(struct Graph* graph) {
  int v;
  for (v = 0; v < graph->numVertices; v++) {
    struct node* temp = graph->adjLists[v];
    printf("\n Adjacency list of vertex %d\n ", v);
    while (temp) {
      printf("%d -> ", temp->vertex);
      temp = temp->next;
    }
    printf("\n");
  }
}

void createEmptyStack(struct stack *s){
    s->top = -1;
    s->size = 0;
}

int isEmpty(struct stack *s){
    if (s->top == -1){
        return 1;
    } else {
        return 0;
    }
}

int isFull(struct stack *s){
    if (s->top == 16 - 1){
        return 1;
    } else{
        return 0;
    }
}

void push(struct stack *s, int newItem){
    if (!isFull(s)){
        s->top++;
        s->items[s->top] = newItem;
        s->size++;
        printf("Item pushed= %d \n", s->items[s->top]);
    }
}

void printStack(struct stack *s){
    int i;
    for (i = 0; i < s->size; i++){
        printf("%d ", s->items[i]);
    }
    printf("\n");
}

int dijkstra(struct Graph* graph, int src, int dest){
    // create stack of 1 path
    struct stack *path = malloc(sizeof(struct stack));
    createEmptyStack(path);

    int minDistance = 16;// set to max distance
    int dist;
    int visitCount = 0; // count of all visited nodes

    // reset all visit points in graph
    int i;
    for (i = 0; i < graph->numVertices; i++) {
        graph->visited[i] = 0;
    }

    int shortestPath[16]; // result to be returned

    // get source node
    struct node* srcNode = graph->adjLists[src];

    struct node* currNode = srcNode;
    struct node* temp;

    // mark source node as visited
    graph->visited[src] = 1;

    while (1) {

        // push src into path
        if (path->size == 0){
            push(path, src);
            dist = 1;
        }

        // save previous node
        temp = currNode;

        // get next vertex
        currNode = currNode->next;

        // if we reach the end of the src node's adjacency list or if the current node has already been visited
        if (!currNode || graph->visited[currNode->vertex] == 1){
            // if previous node has not been visited, backtrack to it
            if (graph->visited[temp->vertex] == 0){
                // mark current node as visited and add into path
                graph->visited[temp->vertex] = 1;

                // if current node is a vertex
                if (temp->vertex < 16){
                    push(path, temp->vertex);
                    dist++;
                    visitCount++;
                }

                currNode = graph->adjLists[temp->vertex];
            }
        }

        // if we hit dest
        if (currNode->vertex == dest){
            // push dest into path
            push(path, currNode->vertex);
            dist++;

            if (path->size < minDistance){
                int j;
                for (j = 0; j < path->size; j++){
                    shortestPath[j] = path->items[j];
                }

                minDistance = dist;
            }

            if (visitCount == 8){
                int j;
                printf("final shortest path: \n");
                for (j = 0; j < path->size; j++){
                    printf("%d \n", shortestPath[j]);
                }
                return 1;
            }


            // reset path and start from src node again
            createEmptyStack(path);

            currNode = srcNode;
            printf("path reset\n");

        }
    }
}

void initMap(void) {
    struct stack* shortestPath;
    struct Graph* graph = createGraph(16);
    addEdge(graph, 0,1);
    addEdge(graph,1,2);
    addEdge(graph,2,3);
    addEdge(graph,3,7);
    addEdge(graph,7,11);
    addEdge(graph,10,11);
    addEdge(graph,10,6);
    addEdge(graph,10,9);
    addEdge(graph,5,6);
    addEdge(graph,9,5);
    addEdge(graph,9,8);
    addEdge(graph,8,12);
    addEdge(graph,12,13);
    addEdge(graph,13,14);
    addEdge(graph,14,15);
    addEdge(graph,8,4);
    addEdge(graph,4,0);


    DFS(graph, 0);
//    moveCar(10);
    printGraph(graph);
    dijkstra(graph, 0, 10);
}
