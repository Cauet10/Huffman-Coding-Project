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
    struct node *parent;
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

static double heuristic(Position pos1, Position pos2)
{
    double dx = (double)(pos2.x - pos1.x);
    double dy = (double)(pos2.y - pos1.y);
    return sqrt(pow(dx,2) + pow(dy,2));
}

//Funçao para obter os vizinhos válidos de uma célula, ou seja, os que não são obstáculos
static int valid_neighbors(const int *grid, int rows, int collums, Position position, Position neighbors[8])
{
    //Grid 2d dos vizinhos
    static const int moves[8][2] = {
    
        {1,0}, {-1,0}, // ortogonais x + 1 e x - 1
        {0,1}, {0,-1}, // ortogonais y + 1 e y - 1
        {1,1}, {-1,-1}, // diagonais
        {1,-1}, {-1,1} // diagonais
    };

    int count = 0; //contagem de vizinhos validos
    for (int i = 0; i < 8; i++)
    {
        //movimentações
        int new_x = position.x + moves[i][0];
        int new_y = position.y + moves[i][1];

        //Se estiver dentro da grid e nao for obstaculo, o vizinho eh valido
        if (new_x >= 0 && new_x < rows && new_y >= 0 && new_y < collums && grid[new_x * collums + new_y] == 0)
        {
            neighbors[count].x = new_x;
            neighbors[count].y = new_y;
            count++;
        }
    }
    return count;
}

static Position *reconstruct_path(const node *goal_node, int *lenght)
{
    int count = 0;
    for (const node *n = goal_node; n != NULL; n = n->parent)
    {
        count++;
    }

    Position *path = (Position*) malloc((size_t)count * sizeof(Position));
    if (path == NULL)
    {
        *lenght = 0;
        return NULL;
    }

    int i = count - 1;
    for (const node *n = goal_node; n != NULL; n = n->parent)
    {
        path[i--] = n->position;
    }

    *lenght = count;
    return path;
}

static Position *find_path(const int *grid, int rows, int collums, Position start, Position goal, int *path_length)
{
    *path_length = 0;

    if (rows <= 0 || collums <= 0)
    {
        return NULL;
    }

    if (start.x < 0 || start.x >= rows || start.y < 0 || start.y >= collums)
    {
        return NULL;
    }

    if (goal.x < 0 || goal.x >= rows || goal.y < 0 || goal.y >= collums)
    {
        return NULL;
    }

    if (grid[start.x * collums + start.y] != 0)
    {
        return NULL;
    }

    if (grid[goal.x * collums + goal.y] != 0)
    {
        return NULL;
    }

    int total = rows * collums;
    node *nodes = (node*) malloc((size_t)total * sizeof(node));
    if (nodes == NULL)
    {
        return NULL;
    }

    for (int x = 0; x < rows; x++) 
    {
        for (int y = 0; y < collums; y++) 
        {
            Position p = {x, y};
            nodes[x * collums + y] = create_node(p, INFINITY, 0.0, NULL);
        }
    }

    int start_index = start.x * collums + start.y;
    nodes[start_index] = create_node(start, 0.0, heuristic(start, goal), NULL);
    nodes[start_index].state = node_open;

    min_heap open_list;
    heap_begin(&open_list, 64);
    heap_push(&open_list, nodes[start_index].f, start_index);
    Position *result = NULL;

    while (open_list.size > 0)
    {
        heap_entry entry = heap_pop(&open_list);
        node *current = &nodes[entry.index];

        if (current->state == node_closed)
        {
            continue;
        } 

        if (current->position.x == goal.x && current->position.y == goal.y) 
        {
            result = reconstruct_path(current, path_length);
            break;
        }

        current->state = node_closed;
        Position neighbors[8];
        int n_count = valid_neighbors(grid, rows, collums, current->position, neighbors);
        for (int i = 0; i < n_count; i++) 
        {
            Position n_pos = neighbors[i];
            node *neighbor = &nodes[n_pos.x * collums + n_pos.y];
            if (neighbor->state == node_closed)
            {
                continue;
            } 
 
            double tentative_g = current->g + heuristic(current->position, n_pos);
            if (neighbor->state == node_unseen || tentative_g < neighbor->g) 
            {
                neighbor->g = tentative_g;
                neighbor->h = heuristic(n_pos, goal);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = current;
                neighbor->state = node_open;
                heap_push(&open_list, neighbor->f, n_pos.x * collums + n_pos.y);
            }
        }
    }

    free_heap(&open_list);
    free(nodes);
    return result;
}

static void visualize_path(const int *grid, int rows, int collums, const Position *path, int path_length)
{
    char *canvas = (char*) malloc((size_t)rows * collums);
    if (canvas == NULL)
    {
        return;
    }

    for (int i = 0; i < rows * collums; i++)
    {
        if (grid[i] != 0)
        {
            canvas[i] = '#';
        }
        else
        {
            canvas[i] = '.';
        }
    }

    if (path != NULL && path_length > 0) 
    {
        for (int i = 0; i < path_length; i++)
        {
            canvas[path[i].x * collums + path[i].y] = '*';
        }
        canvas[path[0].x * collums + path[0].y] = 'S';
        canvas[path[path_length - 1].x * collums + path[path_length - 1].y] = 'G';
    }
 
    printf("A* Pathfinding Result\n");
    for (int x = 0; x < rows; x++) 
    {
        for (int y = 0; y < collums; y++)
        {
            printf("%c ", canvas[x * collums + y]);
        }
        printf("\n");
    }
    free(canvas);
}

int main(void)
{
    int rows, collums;
    printf("Digite a quantidade de linhas:\n");
    scanf("%d", &rows);
    printf("Digite a quantidade de colunas:\n");
    scanf("%d", &collums);

    if (rows <= 0 || collums <= 0)
    {
        printf("Tamanho de grade invalido\n");
        return 1;
    }
 
    int *grid = (int*) calloc((size_t)rows * collums, sizeof(int));
    if (grid == NULL)
    {
        printf("Memoria insuficiente\n");
        return 1;
    }

    int n_obstacles = read_int("Digite a quantidade de obstaculos:\n");
    for (int i = 0; i < n_obstacles; i++)
    {
        int ox = read_int("Obstaculo - linha:\n");
        int oy = read_int("Obstaculo - coluna:\n");
        if (ox < 0 || ox >= rows || oy < 0 || oy >= collums)
        {
            fprintf(stderr, "Obstaculo (%d, %d) fora da grade, ignorado.\n", ox, oy);
            continue;
        }
        grid[ox * collums + oy] = 1;
    }
 
    Position start, goal;
    start.x = read_int("Linha inicial:\n");
    start.y = read_int("Coluna inicial:\n");
    goal.x = read_int("Linha do objetivo:\n");
    goal.y = read_int("Coluna objetivo:\n");
 
    int length = 0;
    Position *path = find_path(grid, rows, collums, start, goal, &length);
 
    if (path == NULL) 
    {
        printf("Nenhum caminho encontrado\n");
        free(grid);
        return 1;
    }
 
    printf("Caminho encontrado com %d posicoes:\n", length);
    for (int i = 0; i < length; i++)
    {
        printf("(%d, %d)%s", path[i].x, path[i].y, i + 1 < length ? " -> " : "\n");
    }
    printf("\n");
 
    visualize_path(grid, rows, collums, path, length);
 
    free(path);
    return 0;
}