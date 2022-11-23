/*
 * initMapping.c
 *

 */

#include <stdio.h>
#include <stdlib.h>
#include "initMapping.h"
#include "initMovement.h"
#include "initUltrasound.h"
#include "printf.h"
struct directionlist* head = NULL;


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
    if(head->direction == 'E')
    {
        if(nextvertex > currentvertex)
        {
            if(currentvertex+1 == nextvertex)
            {
                //moveforward
                moveCar(1);
                //Uartprintf(EUSCI_A0_BASE, "moving east forward %i \r\n", nextvertex);
                stopCar();

            }
            else if(currentvertex+5 == nextvertex)
            {
                //turnright
                turnRight();
                moveCar(1);
                stopCar();
                head = head->next;
            }
        }
        else if(nextvertex < currentvertex)
        {
            if(currentvertex == nextvertex-5)
            {
                //turnleft
                turnLeft();
                moveCar(1);
                stopCar();
                head = head->prev;
            }
            else if(currentvertex == nextvertex-1)
            {
                //turnaround
                turnRight();
                turnRight();
                moveCar(1);
                stopCar();
                head = head->next;
                head = head->next;
            }
        }
    }
    if(head->direction == 'S')
    {
        if(nextvertex > currentvertex)
        {
            if(currentvertex+5 == nextvertex)
            {
                //moveforward
                moveCar(1);
                stopCar();

            }
            else if(currentvertex+1 == nextvertex)
            {
                //turnleft
                turnLeft();
                moveCar(1);
                stopCar();
                head = head->prev;
            }
        }
        else if(nextvertex < currentvertex)
        {
            if(currentvertex-5 == nextvertex)
            {
                //turnaround
                turnRight();
                turnRight();
                moveCar(1);
                stopCar();
                head = head->next;
                head = head->next;
            }
            else if(currentvertex-1 == nextvertex)
            {
                //turnright
                turnRight();
                moveCar(1);
                stopCar();
                head = head->next;
            }
        }
    }
    if(head->direction == 'W')
    {
        if(nextvertex > currentvertex)
        {
            if(currentvertex+5 == nextvertex)
            {
                //turnleft
                turnLeft();
                moveCar(1);
                stopCar();
                head = head->prev;
            }
            else if(currentvertex+1 == nextvertex)
            {
                //turnaround
                turnRight();
                turnRight();
                moveCar(1);
                stopCar();
                head = head->next;
                head = head->next;
            }
        }
        else if(nextvertex < currentvertex)
        {
            if(currentvertex-5 == nextvertex)
            {
                //turnright
                turnRight();
                moveCar(1);
                stopCar();
                head = head->next;
            }
            else if(currentvertex-1 == nextvertex)
            {
                //move forward
                moveCar(1);
                stopCar();
            }
        }
    }
    if(head->direction == 'N')
    {
        if(nextvertex > currentvertex)
        {
            if(currentvertex+5 == nextvertex)
            {
                //turnaround
                turnRight();
                turnRight();
                moveCar(1);
                stopCar();
                head = head->next;
                head = head->next;
            }
            else if(currentvertex+1 == nextvertex)
            {
                //turnright
                turnRight();
                moveCar(1);
                stopCar();
                head = head->next;
            }
        }
        else if(nextvertex < currentvertex)
        {
            if(currentvertex-5 == nextvertex)
            {
                //moveforward
                moveCar(1);
                stopCar();
            }
            else if(currentvertex-1 == nextvertex)
            {
                //turnleft
                turnLeft();
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
    //printf("current %d\n",vertex);
    Uartprintf(EUSCI_A0_BASE, "current %i \r\n",vertex);
    //connected vertex can be track to next block to travel(may have issues?)
    //printf("connected %d\n",connectedVertex);
    Uartprintf(EUSCI_A0_BASE, "connected %i \r\n",connectedVertex);
    checkdirectionmovement(graph,vertex,connectedVertex);


    if (graph->visited[connectedVertex] == 0) {
        //printf("robot move here %d\n",connectedVertex);
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

void initMap(void) {

    struct Graph* graph = createGraph(3);
    //addEdge(graph,0,1);
    createDirectionList();

    DFS(graph, 0);

    //moveCar(1);
    //printGraph(graph);
}
