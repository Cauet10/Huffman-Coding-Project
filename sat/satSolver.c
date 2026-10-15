#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define comentarioMaximo 1000
#define maxInterpParcial 100

typedef struct node 
{
    int indexDaVariavel; //index da variavel em cnf para a ilista de interpretacao parcial
    bool valor; // valor atribuiodo em 0 ou 1
    struct node *left;
    struct node *right;
}node;

typedef struct arvore
{
    //header da arvre
    int qVariaveis;
    node *raiz;
    int interpretacaoParcial[maxInterpParcial];
}arvore;

node* criarNode(int indexDaVar, bool valor)
{
    node *novoNode = calloc(1,sizeof(node));
    novoNode->indexDaVariavel = indexDaVar;
    novoNode->valor = valor;

    return novoNode;
}

node *adicionarNode(arvore *header, node *atual, int indexVar, bool valor)
{
    node *novoNode = criarNode(indexVar, valor);
    if(valor == true)
    {
        atual->left = novoNode;
    }
    else // se for 0;
    {
        atual->right = novoNode;
        
    }
    return novoNode;
}

bool satisfas(int numeroClausulas, int numLiterais, int formula[numeroClausulas][numLiterais], int interpParcial[maxInterpParcial])
{
    for (int i = 0; i < numeroClausulas; i++)
    {
        bool satisfeito = false;
    }
    
}
