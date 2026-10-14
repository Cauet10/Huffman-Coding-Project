#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/*Esta parte do código é referente as structs necessárias para o calculo da posição, estado do nó, nó e heap*/

typedef struct Position
{
    int x;
    int y;
}Position;

typedef enum node_state
{
    node_unseen = 0,
    node_open,
    node_closed
}node_state;

typedef struct node
{
    Position position;
    double g;
    double h;
    double f;
    node *parent;
    node_state state;
}node;

typedef struct heap_entry
{
    double f;
    int index;
}heap_entry;

typedef struct min_heap
{
    heap_entry *data;
    int size;
    int capacity;
}min_heap;

/*Esta parte do código é referente as funções de heap (fila de prioridades) que serão usadas nas funções do A* */

static void heap_begin(min_heap *heap, int capacity)
{
    if (capacity < 16)
    {
        capacity = 16;
    }
    heap->data = (heap_entry*) malloc((size_t)capacity * sizeof(heap_entry));

    if (heap->data == NULL)
    {
        printf("Error: insuficient memory.\n");
        exit(EXIT_FAILURE);
    }
    heap->size = 0;
    heap->capacity = capacity;
}

static void free_heap(min_heap *heap)
{
    free(heap->data);
    heap->data = NULL;
    heap->size = heap->capacity = 0;
}

static void heap_push(min_heap *heap, double f, int index)
{
    if (heap->size == heap->capacity)
    {
        int new_cap = heap->capacity * 2;
        heap_entry *temp = (heap_entry*) realloc(heap->data, (size_t)new_cap * sizeof(heap_entry));

        if (temp == NULL)
        {
            printf("Error: insuficient memory.\n");
            exit(EXIT_FAILURE);
        }

        heap->data = temp;
        heap->capacity = new_cap;
    }

    int i = heap->size++;
    while (i > 0)
    {
        int parent = (i - 1) / 2;
        if (heap->data[parent].f <= f)
        {
            break;
        }
        heap->data[i] = heap->data[parent];
        i = parent;
    }
    heap->data[i].f = f;
    heap->data[i].index = index;
}

static heap_entry heap_pop(min_heap *heap)
{
    heap_entry top = heap->data[0];
    heap_entry last = heap->data[--heap->size];
    
    int i = 0;
    while (true)
    {
        int left = 2 * i + 1;
        int right = left + 1;

        if (left >= heap->size)
        {
            break;
        }
        int smallest = left;

        if (right < heap->size && heap->data[right].f < heap->data[left].f)
        {
            smallest = right;
        }

        if (last.f <= heap->data[smallest].f)
        {
            break;
        }
        heap->data[i] = heap->data[smallest];
        i = smallest;
    }

    if (heap->size > 0)
    {
        heap->data[i] = last;
    }
    return top;
}

/*Esta parte do código é referente as funções do A* */

static node create_node(Position position, double g, double h, node *parent)
{
    node node;
    node.position = position;
    node.g = g;
    node.h = h;
    node.f = g + h;
    node.parent = parent;
    node.state = node_unseen;
    return node;
}

