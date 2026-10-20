#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define draw 1000 //number of draws to be done and registered
#define max_value 1000000 //drawn numbers go from 1 to 1000000
#define trials 10 //number of different random structures averaged for each n

//LIST PART
typedef struct node
{
    int item;
    struct node* next;
} node;

//adds item at the beginning of the list
void list_add(int item, node** head)
{
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
    srand(time(NULL));

    FILE* csv = fopen("dados.csv", "w");
    fprintf(csv, "n,comp_lista,comp_arvore\n");

    for(int n = 1; n <= 1000; n++)
    {
        long long total_list = 0, total_tree = 0; //number of comparisons accumulated throughout the iterations with 'trials'.

        for(int t = 0; t < trials; t++) //repeats 10 times each iteration of n;
        {
            int* values = (int*) malloc(n * sizeof(int));
            node* list = NULL;
            binary_tree* tree = NULL;

            int count = 0; //keeps track of quantitty of items in values.
            while(count < n)
            {
                int value = rand() % max_value + 1; //draws random value.                   
                long long unused = 0;
                if(tree_search(value, tree, &unused) == NULL) //searches through the tree to check if the item is already there. if so, doesn't put it in the list.
                {
                    values[count++] = value;
                    list_add(value, &list);
                    tree_add(value, &tree);
                }
            }

            //searches every stored item and accumulates the comparisons
            for(int i = 0; i < n; i++)
            {
                list_search(values[i], list, &total_list);
                tree_search(values[i], tree, &total_tree);
            }

            free_list(list);
            free_tree(tree);
            free(values);
        }

        double avg_list = (double) total_list / ((double) trials * n);
        double avg_tree = (double) total_tree / ((double) trials * n);
        fprintf(csv, "%d,%.4f,%.4f\n", n, avg_list, avg_tree);

        printf("n = %d done\n", n);
    }

    fclose(csv);
    return 0;
}
