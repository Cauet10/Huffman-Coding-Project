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

        for (int j = 0; j < numLiterais; j++)
        {
            if (formula[i][j] == 2)
            {
                satisfeito = true;
                break;
            }
            if(formula[i][j] == interpParcial[j] && interpParcial[j] != 0)
            {
                satisfeito = true;
                break;
            }

        }
        if (satisfeito == false)
        {
            return false;
        }
        return true;
    }

}

bool contradiz(int numClausulas,int numLiterais, int formula[numClausulas][numLiterais],int interpParcial[maxInterpParcial])
{
     for (int i = 0; i < numClausulas; i++)
    {
        bool contradito = true;

        for (int j = 0;j < numLiterais; j++)
        {
            if (formula[i][j] == 2)
            {
                contradito = false;
                break;
            }

            if(formula[i][j] != 0 && interpParcial[j] == 0)
            {
                contradito = false;
                break;
            }
            if(formula[i][j] == interpParcial[j] && interpParcial[j] != 0)
            {
                contradito = false;
                break;
            }
        }
        if (contradito == true)
        {
            return true;
        }
        
    }   
    return false;
}

/**
 * @brief 
 * 
 * @param numClausulas 
 * @param numLiterais 
 * @param formula 
 * @param interpParcial 
 * @param header 
 * @param atual 
 * @param varIndxex 
 * @return true 
 * @return false 
 */
bool sat(int numClausulas,int numLiterais, int formula[numClausulas][numLiterais], arvore *header, node *atual, int varIndxex)
{
    if (satisfas( numClausulas,  numLiterais, formula[numClausulas][numLiterais],header->interpretacaoParcial) )
    {
        return true;
    }
    if (contradiz(numClausulas, numLiterais, formula[numClausulas][numLiterais],header->interpretacaoParcial))
    {
        return false;
    }
    
    node *nodeTrue = adicionarNode(header,atual,varIndxex, true);
    if (sat(numClausulas,numLiterais,formula,header,nodeTrue,varIndxex+1))
    {
        return true;
    }
    
    node *nodeFalse = adicionarNode(header,atual,varIndxex,false);
    if (sat(numClausulas,numLiterais,formula,header,nodeFalse,varIndxex+1))
    {
        return true;
    }

    header->interpretacaoParcial[varIndxex] = 0; // backtrack?

    return false;
    
}


arvore *initArvore(int qVariaveis)
{
    arvore *arvoreNova = calloc(1,sizeof(arvore));
    arvoreNova->raiz = criarNode(-1,false); // nao representa nada

    arvoreNova->qVariaveis = qVariaveis;
    
    return arvoreNova;
    }


void lerInput(int *literal, int *clausula)
{
    char comando;
    char comentario[comentarioMaximo];
    char forma[10]; // vai ser cnf

    while (scanf(" %c", &comando) != EOF)
    {
        if (cmd == "sair")
        {
            /* code */
        }
        
    }
    
    
}