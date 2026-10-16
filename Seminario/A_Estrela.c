#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/*Esta parte do código é referente as structs necessárias para o calculo da posição, estado do nó, nó e heap*/

//Coordenada de uma célula: x = linha, y = coluna
typedef struct Position
{
    int x;
    int y;
}Position;

//Estado de um nó durante a busca
typedef enum node_state
{
    node_unseen = 0, //Nó não descoberto
    node_open,       //Nó descoberto, mas não expandido
    node_closed      //Nó expandido
}node_state;

//Nó do A*
typedef struct node
{
    Position position; //Coordenadas do nó
    double g; //Custo real do início até este nó
    double h; //Custo estimado (heurística) deste nó até o alvo
    double f; //f = g + h, prioridade do nó na fila
    struct node *parent; //Nó anterior no melhor caminho (inicialmente é nulo)
    node_state state; //Estado do nó: não visto, aberto ou fechado
}node;

//Entrada da fila de prioridade: valor de f e índice do nó no vetor
typedef struct heap_entry
{
    double f;
    int index;
}heap_entry;

//Fila de prioridade (min-heap binário)
typedef struct min_heap
{
    heap_entry *data; //Vetor dinâmico com as entradas
    int size; //Quantidade de entradas atualmente armazenadas
    int capacity; //Capacidade alocada do vetor
}min_heap;

/*Esta parte do código é referente as funções de heap (fila de prioridades) que serão usadas nas funções do A* */

/**
 * @brief inicializa uma fila de prioridade vazia
 * 
 * @param heap ponteiro para a fila a ser inicializada
 * @param capacity capacidade inicial (o vetor cresce automaticamente)
 */
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

/**
 * @brief libera a memória usada pela fila
 * 
 * @param heap fila a ser liberada
 */
static void free_heap(min_heap *heap)
{
    free(heap->data);
    heap->data = NULL;
    heap->size = heap->capacity = 0;
}

/**
 * @brief insere uma entrada na fila mantendo a propriedade de min-heap
 * 
 * @param heap fila onde inserir
 * @param f prioridade (custo total estimado) da entrada
 * @param index índice do nó correspondente no vetor de nós
 */
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

/**
 * @brief remove e devolve a entrada com menor valor de f
 * 
 * @param heap fila de onde remover
 * 
 * @return entrada de menor prioridade
 */
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

/**
 * @brief cria e devolve um nó do A* já com f = g + h calculado
 * 
 * @param position coordenadas (x, y) do nó
 * @param g custo do início até este nó
 * @param h custo estimado deste nó até o objetivo (heurística)
 * @param parent ponteiro para o nó anterior no caminho (se não houver, é nulo)
 */
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

/**
 * @brief estima a distância entre dois pontos usando a distância euclidiana
 * 
 * @param pos1 primeira posição (x1, y1)
 * @param pos2 segunda posição (x2, y2)
 * 
 * @return square root((x2 - x1)^2 + (y2 - y1)^2)
 */
static double heuristic(Position pos1, Position pos2)
{
    double dx = (double)(pos2.x - pos1.x);
    double dy = (double)(pos2.y - pos1.y);
    return sqrt(pow(dx,2) + pow(dy,2));
}

/**
 * @brief obtém os vizinhos válidos de uma célula, isto é, dentro dos limites da grade e que não sejam obstáculos
 * 
 * @param grid grade 2D linearizada (0 = livre, 1 = obstáculo)
 * @param rows número de linhas da grade
 * @param collums número de colunas da grade
 * @param position célula atual (x, y)
 * @param neighbors vetor de saída com capacidade para pelo menos 8 posições, recebe vizinhos válidos
 * 
 * @return quantidade de vizinhos válidos escritos em 'neighbors' (0 a 8)
 */
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

/**
 * @brief reconstrói o caminho seguindo os ponteiros 'parent' do nó objetivo até o nó inicial, e devolve o caminho na ordem início -> objetivo
 * 
 * @param goal_node nó objetivo (ponto de partida da reconstrução)
 * @param lenght ponteiro de saída que recebe o número de posições do caminho
 * 
 * @return vetor alocado com malloc contendo o caminho ou NULL em caso de falha de alocação
 */
static Position *reconstruct_path(const node *goal_node, int *length)
{
    int count = 0;
    for (const node *n = goal_node; n != NULL; n = n->parent)
    {
        count++;
    }

    Position *path = (Position*) malloc((size_t)count * sizeof(Position));
    if (path == NULL)
    {
        *length = 0;
        return NULL;
    }

    int i = count - 1;
    for (const node *n = goal_node; n != NULL; n = n->parent)
    {
        path[i--] = n->position;
    }

    *length = count;
    return path;
}

/**
 * @brief encontra o caminho de menor custo entre 'start' e 'goal' usando o algoritmo A*
 * 
 * @param grid grade 2D linearizada (0 = livre, 1 = obstáculo), com rows * cols elementos em ordem row-major
 * @param rows número de linhas da grade
 * @param collums número de colunas da grade
 * @param start posição inicial (x, y)
 * @param goal posição objetivo (x, y)
 * @param path_length ponteiro de saída com o número de posições do caminho (0 se não houver caminho)
 * 
 * @return vetor alocado com malloc com as posições do caminho, de 'start' até 'goal', 
 * retorna NULL se não existir caminho, se start/goal forem inválidos ou em caso de erro de memória
 */
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

/**
 * @brief imprime a grade e o caminho encontrado no terminal
 * 
 * legenda: '#' obstáculo, '.' livre, '*' caminho, 'S' início, 'G' objetivo
 * 
 * @param grid grade 2D linearizada (0 = livre, 1 = obstáculo)
 * @param rows número de linhas da grade
 * @param collums número de colunas da grade 
 * @param path vetor com as posições do caminho (pode ser NULL)
 * @param path_length quantidade de posições em 'path'
 */
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

    int n_obstacles;
    printf("Digite a quantidade de obstaculos:\n");
    scanf("%d", &n_obstacles);
    for (int i = 0; i < n_obstacles; i++)
    {
        int ox;
        printf("Obstaculo - linha:\n");
        scanf("%d", &ox);

        int oy;
        printf("Obstaculo - coluna:\n");
        scanf("%d", &oy);
        if (ox < 0 || ox >= rows || oy < 0 || oy >= collums)
        {
            fprintf(stderr, "Obstaculo (%d, %d) fora da grade, ignorado.\n", ox, oy);
            continue;
        }
        grid[ox * collums + oy] = 1;
    }
 
    Position start, goal;
    start.x;
    printf("Linha inicial:\n");
    scanf("%d", &start.x);

    start.y;
    printf("Coluna inicial:\n");
    scanf("%d", &start.y);

    goal.x;
    printf("Linha do objetivo:\n");
    scanf("%d", &goal.x);
    
    goal.y;
    printf("Coluna objetivo:\n");
    scanf("%d", &goal.y);
 
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