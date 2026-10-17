#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define draw 1000 //number of draws to be done and registered
#define max_value 1000000 //drawn numbers go from 1 to 1000000

//LIST PART
typedef struct node
{
    int item;
    struct node* next;
} node;

//adds item at the beginning of the list
void list_add(int item, node** head)
{
    node* current = *head;
    while(current != NULL)
    {
        if(current->item == item) return;
        current = current->next;
    }
    node* new_node = (node*) malloc(sizeof(node));
    new_node->item = item;
    new_node->next = *head;
    *head = new_node;
}

//navigates the list and searches for an item. comparisons are counted for plotting.
node* list_search(int item, node* head, long long* comps)
{
    node* current = head;
    while(current != NULL)
    {
        (*comps)++;
        if(current->item == item) return current;
        current = current->next;
    }
    return NULL;
}

void free_list(node* head)
{
    while(head != NULL)
    {
        node* temp = head;
        head = head->next;
        free(temp);
    }
}

//BINARY TREE PART
typedef struct binary_tree
{
    int item;
    struct binary_tree* left;
    struct binary_tree* right;
} binary_tree;

binary_tree* create_binary_tree(int item, binary_tree* left, binary_tree* right)
{
    binary_tree* new_tree = (binary_tree*) malloc(sizeof(binary_tree));
    new_tree->item = item;
    new_tree->left = left;
    new_tree->right = right;
    return new_tree;
}

//goes through all the tree to find the right place to put an item, keeping the tree ordered.
void tree_add(int item, binary_tree** tree)
{
    if(*tree == NULL)
    {
        *tree = create_binary_tree(item, NULL, NULL);
        return;
    }

    binary_tree* current = *tree;
    binary_tree* parent = NULL;
    while(current != NULL)
    {
        parent = current;
        if(item == current->item) return;
        current = (item < current->item) ? current->left : current->right;
    }

    if(item < parent->item)
        parent->left = create_binary_tree(item, NULL, NULL);
    else
        parent->right = create_binary_tree(item, NULL, NULL);
}

//navigates the tree to find the item. comparisons are counted for plotting.
binary_tree* tree_search(int item, binary_tree* tree, long long* comps)
{
    binary_tree* current = tree;
    while(current != NULL)
    {
        (*comps)++;
        if(item == current->item) return current;
        current = (item < current->item) ? current->left : current->right;
    }
    return NULL;
}

void free_tree(binary_tree* tree)
{
    if(tree == NULL) return;
    free_tree(tree->left);
    free_tree(tree->right);
    free(tree);
}

//MAIN
int main()
{
    srand(time(NULL)); //initializes rand() with the current time, so each execution draws different numbers

    //creates file and writes columns names
    FILE* csv = fopen("dados.csv", "w");
    fprintf(csv, "n,sorteio,numero,comp_lista,comp_arvore\n");

    for(int k = 1; k <= 1000; k++)
    {
        int n = k; //number of items in each struct for this loop
        int* values = (int*) malloc(n * sizeof(int)); //will store all the items for the list and tree.
        node* list = NULL;
        binary_tree* tree = NULL;

        //new random numbers are put in the list and tree until both have exactly 'n' items.
        int count = 0;
        while(count < n)
        {
            int value = rand() % max_value + 1; //random drawn number
            long long unused = 0;
            if(tree_search(value, tree, &unused) == NULL) //skips numbers that are already stored
            {
                values[count++] = value;
                list_add(value, &list);
                tree_add(value, &tree);
            }
        }

        //random numbers are drawn for a search on the list and on the tree exactly 'draw' times.
        for(int s = 1; s <= draw; s++)
        {
            int number = values[rand() % n]; 
            long long comps_list = 0, comps_tree = 0;
            list_search(number, list, &comps_list);
            tree_search(number, tree, &comps_tree);
            fprintf(csv, "%d,%d,%d,%lld,%lld\n", n, s, number, comps_list, comps_tree);
        }

        printf("n = %d done\n", n);

        free_list(list);
        free_tree(tree);
        free(values);
    }

    fclose(csv);
    return 0;
}
