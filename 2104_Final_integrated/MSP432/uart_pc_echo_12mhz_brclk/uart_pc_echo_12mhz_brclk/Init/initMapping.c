/*
 * initMapping.c
 *

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "initMapping.h"
#include "initMovement.h"
#include "initUltrasound.h"
#include "printf.h"
#include "initComms.h"
#define SIZE 40

struct directionlist* head = NULL;

#define TIMER_PERIOD 46875
volatile int time = 0;
volatile int started = 0;
int done =0;
char buffer[20];

unsigned char * string[20];


// BFS algorithm
int bfs(struct Graph* graph, int startVertex, int endVertex, int pred[], int dist[]) {
    //queue to maintain the order of vertices traverse
    struct queue* q = createQueue();

  int visited[20];
  int i =0;

  //initially set all vertices to be unvisited, and dist to be MAX
  for (i=0; i<20; i++) {
    visited[i] = 0;
    dist[i] = 999;
    pred[i] = -1;
  }

  // set the inital start vertices to be visited
  visited[startVertex] = 1;

  //distance from itself is 0.
  dist[startVertex] =0;
  enqueue(q, startVertex);

//   graph->visited[startVertex] = 1;
//   dist[startVertex] = 0;
//   enqueue(q, startVertex);

  //check if queue is empty, if yes means bfs all travelled
  while (!isEmpty(q)) {
    //printQueue(q);
      //this is to get the current vertice it is at.
    int currentVertex = dequeue(q);
    printf("Visited %d\n", currentVertex);


    //retrieve all adjacent vertices
    struct node* temp = graph->adjLists[currentVertex];

    while (temp) {
      int adjVertex = temp->vertex;

      //check if the adj vertice is visited or not
      if (visited[temp->vertex] == 0) {
        visited[temp->vertex] = 1;
        graph->visited[adjVertex] = 1;

        //updated the distance array with the value of the current vertice
        dist[temp->vertex] = dist[currentVertex] +1;

        //this is used to store the order of the path
        pred[temp->vertex] = currentVertex;
        enqueue(q, temp->vertex);

        //check if current vertex is the dest
        if (currentVertex == endVertex) {
            return 1;
        }
      }
      temp = temp->next;
    }
  }
  return 0;
}

void printShortestPath(struct Graph* graph, int startVertex, int endVertex) {
    //pred is used to store the prev paths, dist is used to store the value of the "weight"
    int pred[20], dist[20];
    if (bfs(graph, startVertex, endVertex, pred, dist)) {
        printf("Start & Dest not connected");
        return;
    }

    //pathQ is used to store the shortest path
    struct queue* pathQ = createQueue();
    int crawl = endVertex;
    enqueue(pathQ, crawl);

    while (pred[crawl] != -1) {
        enqueue(pathQ, pred[crawl]);
        crawl = pred[crawl];
    }


    printQueue(pathQ);

}

// Create a queue
struct queue* createQueue() {
  struct queue* q = malloc(sizeof(struct queue));
  q->front = -1;
  q->rear = -1;
  return q;
}

// Check if the queue is empty
int isEmpty(struct queue* q) {
  if (q->rear == -1)
    return 1;
  else
    return 0;
}

// Adding elements into queue
void enqueue(struct queue* q, int value) {
  if (q->rear == SIZE - 1) {
    printf("\nQueue is Full!!");
  }
  else {
    if (q->front == -1) {
      q->front = 0;
    }
    q->rear++;
    q->items[q->rear] = value;
  }
}

// Removing elements from queue
int dequeue(struct queue* q) {
  int item;
  if (isEmpty(q)) {
    printf("Queue is empty");
    item = -1;
  } else {
    item = q->items[q->front];
    q->front++;
    if (q->front > q->rear) {
      printf("Resetting queue ");
      q->front = q->rear = -1;
    }
  }
  return item;
}

// Print the queue
void printQueue(struct queue* q) {
  int i = q->front;

  if (isEmpty(q)) {
    printf("Queue is empty");
  } else {
    printf("\nQueue contains \n");
    strncat(string,"*", 1);
    for (i = q->front; i < q->rear + 1; i++) {
        Uartprintf(EUSCI_A0_BASE,"%i \r\n" , q->items[i]);
        //snprintf(buffer, 20, "pre_%d_suff", q->items[i]);
        sprintf(buffer, "<-%d", q->items[i]);
        strncat(string,buffer, 4);
    }
  }
  strncat(string,"*", 1);


  Uartprintf(EUSCI_A0_BASE,"str: %s \r\n" , string);
}


void push(struct directionlist** head_ref, char new_data)
{
    /* 1. allocate node */
    struct directionlist* new_node
        = (struct directionlist*)malloc(sizeof(struct directionlist));

    /* 2. put in the data  */
    new_node->direction = new_data;

    /* 3. Make next of new node as head and previous as NULL
     */
    new_node->next = (*head_ref);
    new_node->prev = NULL;

    /* 4. change prev of head node to new node */
    if ((*head_ref) != NULL)
        (*head_ref)->prev = new_node;

    /* 5. move the head to point to the new node */
    (*head_ref) = new_node;
}

void append(struct directionlist** head_ref, int new_data)
{
    /* 1. allocate node */
    struct directionlist* new_node
        = (struct directionlist*)malloc(sizeof(struct directionlist));

    struct directionlist* last = *head_ref; /* used in step 5*/

    /* 2. put in the data  */
    new_node->direction = new_data;

    /* 3. This new node is going to be the last node, so
          make next of it as NULL*/
    new_node->next = NULL;

    /* 4. If the Linked List is empty, then make the new
          node as head */
    if (*head_ref == NULL) {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }

    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;

    /* 6. Change the next of last node */
    last->next = new_node;

    /* 7. Make last node as previous of new node */
    new_node->prev = last;

    return;
}
// link the direction list [E,S,W,N] EAST and NORTH
void linkfrontback(struct directionlist* node)
{
    struct directionlist* last;
    struct directionlist* head;

    head = node;
    while (node != NULL) {
        last = node;
        node = node->next;
    }
    if(last == NULL)
    {
        last = last->prev;
    }
    else
    {
        last->next = head;
        head->prev = last;
    }
    last = last->next;
}

void createDirectionList(void) {
    append(&head, 'N');
    push(&head, 'W');
    push(&head, 'S');
    push(&head, 'E');
    linkfrontback(head);
    //printList(head);
}
//Print direction list
void printList(struct directionlist* node)
{
    struct directionlist* last;
    printf("\nTraversal in forward direction \n");
    while (node != NULL) {
        printf(" %c ", node->direction);
        last = node;
        node = node->next;
    }
    while(last != NULL)
    {
        printf(" %c ", last->direction);
        last = last->prev;
    }

}
//use sensor to check where the walls are
void checkdirectionsensor(struct Graph* graph,int currentvertex)
{

          //check left sensor for wall and AddEdge in map if there is no wall
          if(checkWallLeft() == 0)
          {

              //check direction to coordinate if wall added
              if(head->direction == 'E')
              {
                  addEdge(graph,currentvertex,currentvertex-5);

                  Uartprintf(EUSCI_A0_BASE, "Left East %i %i \r\n",currentvertex,currentvertex-5);

              }
              if(head->direction == 'S')
              {
                  addEdge(graph,currentvertex,currentvertex+1);
                  Uartprintf(EUSCI_A0_BASE, "Left South %i %i \r\n",currentvertex,currentvertex+1);
              }
              if(head->direction == 'W')
              {
                  addEdge(graph,currentvertex,currentvertex+5);
                  Uartprintf(EUSCI_A0_BASE, "Left West %i %i \r\n",currentvertex,currentvertex+5);
              }
              if(head->direction == 'N')
              {
                  addEdge(graph,currentvertex,currentvertex-1);
                  Uartprintf(EUSCI_A0_BASE, "Left North %i %i \r\n",currentvertex,currentvertex-1);
              }
          }
          //check right sensor for wall and AddEdge in map if there is no wall
          if(checkWallRight() == 0)
          {
              //check direction to coordinate if wall added
              if(head->direction == 'E')
              {
                  addEdge(graph,currentvertex,currentvertex+5);
                  Uartprintf(EUSCI_A0_BASE, "Right East %i %i \r\n",currentvertex,currentvertex+5);

              }
              if(head->direction == 'S')
              {
                  addEdge(graph,currentvertex,currentvertex-1);
                  Uartprintf(EUSCI_A0_BASE, "Right South %i %i \r\n",currentvertex,currentvertex-1);
              }
              if(head->direction == 'W')
              {
                  addEdge(graph,currentvertex,currentvertex-5);
                  Uartprintf(EUSCI_A0_BASE, "Right West %i %i \r\n",currentvertex,currentvertex-5);
              }
              if(head->direction == 'N')
              {
                  addEdge(graph,currentvertex,currentvertex+1);
                  Uartprintf(EUSCI_A0_BASE, "Right North %i %i \r\n",currentvertex,currentvertex+1);
              }
          }
          //check front sensor for wall and AddEdge in map if there is no wall

          if(checkWallFront() == 0)
          {
              //check direction to coordinate if wall added
              if(head->direction == 'E')
              {
                  addEdge(graph,currentvertex,currentvertex+1);
                  Uartprintf(EUSCI_A0_BASE, "Front East %i %i \r\n",currentvertex,currentvertex+1);

              }
              if(head->direction == 'S')
              {
                  addEdge(graph,currentvertex,currentvertex+5);
                  Uartprintf(EUSCI_A0_BASE, "Front South %i %i \r\n",currentvertex,currentvertex+5);
              }
              if(head->direction == 'W')
              {
                  addEdge(graph,currentvertex,currentvertex-5);
                  Uartprintf(EUSCI_A0_BASE, "Front West %i %i \r\n",currentvertex,currentvertex-5);
              }
              if(head->direction == 'N')
              {
                  addEdge(graph,currentvertex,currentvertex+1);
                  Uartprintf(EUSCI_A0_BASE, "Front North %i %i \r\n",currentvertex,currentvertex+1);
              }
          }
}


void checkdirectionmovement(struct Graph* graph,int currentvertex,int nextvertex)
{
    //check direction East
    if(head->direction == 'E')
    {
        //this check is for the car moving straight or turn right
        if(nextvertex > currentvertex)
        {
            if(currentvertex+1 == nextvertex)
            {
                Uartprintf(EUSCI_A0_BASE, "east \r\n");
                //moveforward
                moveCar(1);
                stopCar();


            }
            if(currentvertex+5 == nextvertex)
            {
                //turnright
                Uartprintf(EUSCI_A0_BASE, "east 5 \r\n");
                turnRight(2);
                moveCar(1);
                stopCar();
                //change direction
                head = head->next;
            }
        }
        //this check is for the car moving turn around or turn left
        else if(nextvertex < currentvertex)
        {
            if(currentvertex == nextvertex-5)
            {
                //turnleft
                turnLeft(2);
                moveCar(1);
                stopCar();
                // change direction
                head = head->prev;
            }
            else if(currentvertex == nextvertex-1)
            {
                //turnaround
                turnRight(2);
                turnRight(2);
                moveCar(1);
                stopCar();
                //change direction
                head = head->next;
                head = head->next;
            }
        }
    }
    //check direction is south
    if(head->direction == 'S')
    {
        // this check move forward and turn left
        if(nextvertex > currentvertex)
        {
            if(currentvertex+5 == nextvertex)
            {
                //moveforward
                moveCar(1);
                stopCar();

            }
            if(currentvertex+1 == nextvertex)
            {
                //turnleft
                Uartprintf(EUSCI_A0_BASE, "south 1 \r\n");
                turnLeft(2);
                moveCar(1);
                stopCar();
                head = head->prev;
            }
        }
        //this check direction turnaround  and turn right
        else if(nextvertex < currentvertex)
        {
            if(currentvertex-5 == nextvertex)
            {
                //turnaround
                turnRight(2);
                turnRight(2);
                moveCar(1);
                stopCar();
                head = head->next;
                head = head->next;
            }
            else if(currentvertex-1 == nextvertex)
            {
                //turnright
                turnRight(2);
                moveCar(1);
                stopCar();
                head = head->next;
            }
        }
    }
    // check direction west
    if(head->direction == 'W')
    {
        //this check if it turnleft and turn around
        if(nextvertex > currentvertex)
        {
            if(currentvertex+5 == nextvertex)
            {
                //turnleft
                turnLeft(2);
                moveCar(1);
                stopCar();
                head = head->prev;
            }
            if(currentvertex+1 == nextvertex)
            {
                //turnaround
                turnRight(2);
                turnRight(2);
                moveCar(1);
                stopCar();
                head = head->next;
                head = head->next;
            }
        }
        // this check it turns right and move forward
        else if(nextvertex < currentvertex)
        {
            if(currentvertex-5 == nextvertex)
            {
                //turnright
                turnRight(2);
                moveCar(1);
                stopCar();
                head = head->next;
            }
            if(currentvertex-1 == nextvertex)
            {
                //move forward
                moveCar(1);
                stopCar();
            }
        }
    }
    // this check direction north
    if(head->direction == 'N')
    {
        // this check turn around and turn right
        if(nextvertex > currentvertex)
        {
            if(currentvertex+5 == nextvertex)
            {
                //turnaround
                turnRight(2);
                turnRight(2);
                moveCar(1);
                stopCar();
                head = head->next;
                head = head->next;
            }
            if(currentvertex+1 == nextvertex)
            {
                //turnright
                turnRight(2);
                moveCar(1);
                stopCar();
                head = head->next;
            }
        }
        //this check move forward and turn left
        else if(nextvertex < currentvertex)
        {
            if(currentvertex-5 == nextvertex)
            {
                //moveforward
                moveCar(1);
                stopCar();
            }
            if(currentvertex-1 == nextvertex)
            {
                //turnleft
                turnLeft(2);
                moveCar(1);
                stopCar();
                head = head->prev;
            }
        }
    }



}
// DFS algo
void DFS(struct Graph* graph, int vertex) {

  // check direction to add the edge to the map
  checkdirectionsensor(graph,vertex);
  Uartprintf(EUSCI_A0_BASE, "dfs1\r\n");

  struct node* adjList = graph->adjLists[vertex];
  struct node* temp = adjList;


  // indicate which vertex(point has been visited)
  graph->visited[vertex] = 1;
  Uartprintf(EUSCI_A0_BASE, "Visited %i \r\n", vertex);



  while (temp != NULL) {
    int connectedVertex = temp->vertex;
    //current vertex is to track which block on the map that it has to travel
    //current vertex to show where the robot is at
    Uartprintf(EUSCI_A0_BASE, "current %i \r\n",vertex);
    //connected vertex can be track to next block to travel
    Uartprintf(EUSCI_A0_BASE, "connected %i \r\n",connectedVertex);
    checkdirectionmovement(graph,vertex,connectedVertex);


    if (graph->visited[connectedVertex] == 0) {
        //recall the funtion with the new current location
        DFS(graph, connectedVertex);

    }
    temp = temp->next;
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

void initMap(void) {

    struct Graph* graph = createGraph(20);

//    addEdge(graph, 1, 6);
//    addEdge(graph, 6,11);
//    addEdge(graph, 11, 12);
//    addEdge(graph, 12, 13);
//    addEdge(graph, 2, 3);
//    addEdge(graph, 3, 4);
//    addEdge(graph, 4, 9);
//    addEdge(graph, 9, 14);
//    addEdge(graph, 14, 19);
//    addEdge(graph, 0, 5);
//    addEdge(graph, 5, 10);
//    addEdge(graph, 10, 15);
//    addEdge(graph, 15, 16);
//    addEdge(graph, 16, 17);
//    addEdge(graph, 17, 18);
//    addEdge(graph, 18, 19);
//    addEdge(graph, 2, 7);
//    addEdge(graph, 7, 12);
//    addEdge(graph, 12, 13);
//    addEdge(graph, 1, 6);
//    addEdge(graph, 6, 7);



    createDirectionList();

    DFS(graph, 0);
    printGraph(graph);

    Uartprintf(EUSCI_A0_BASE, "print shortest path: \r\n");

    printShortestPath(graph,0,13);
}
