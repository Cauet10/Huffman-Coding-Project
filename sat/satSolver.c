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
        header->interpretacaoParcial[indexVar] = 1;
        atual->left = novoNode;
    }
    else // se for 0;
    {
        header->interpretacaoParcial[indexVar] = -1;
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
    }
    return true;
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
    if (satisfas( numClausulas,  numLiterais, formula,header->interpretacaoParcial) )
    {
        return true;
    }
    if (contradiz(numClausulas, numLiterais, formula,header->interpretacaoParcial))
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

/**
 * @brief funcao simples que exibe uma string, usado principalmente para erros, 
 * principalmente : ENCERRA O PROGRAMA
 * 
 * @param mensagem string a ser printada
 */
void mensagemDeErro(char mensagem[])
{
    printf("%s\n",mensagem);
    exit(EXIT_FAILURE);
}
void lerInput(int *literal, int *clausula)
{
    char comando;
    char comentario[comentarioMaximo];
    char forma[10]; // vai ser cnf

    while (scanf(" %c", &comando) != EOF)
    {
        if (comando == 'c')
        {
           fgets(comentario, sizeof(comentario),stdin);
           comentario[strlen(comentario) - 1] = '\0';
           printf("=== %s ===\n",comentario +1);// +1 pra tirar o espaco do buffer começando a partir do 2 caracter (indice 1)

        }
        if (comando == 'p')
        {
            scanf("%s",forma);
            if (strcmp(forma,"cnf") != 0)
            {
                mensagemDeErro("Forma invalida! use 'cnf'");
            }
            scanf("%d %d", literal, clausula);
            if (*literal <= 0)
            {
                mensagemDeErro("\n numero de lierais invalido! use um valor valido ");
            }
            if (*clausula <= 0)
            {
                mensagemDeErro("\n Numero de clausulas invalido! use um numero de clausulas valido");
            }
            break;
        }
    }
    printf("\n");

    return;
}

void lerFormula(int numClausula, int numLiteral, int formula[numClausula][numLiteral])
{
    //inicializa a matriz com tudo zerado
    memset(formula,0, numClausula * numLiteral * sizeof(int));

    for (int i = 0; i < numClausula; i++)
    {
        int aux, tautologia = 0;
    
    while (1)
    {
        scanf("%d",&aux);

        if (aux == 0) // fim da clausula
        {
            break;
        }
        if (aux > 0)
        {
            if (formula)
            {
                if (formula[i][aux-1]  == -1)
                {
                    tautologia = 1;
                }
                
            }
            formula[i][aux-1] = 1;
        }
        if (aux < 0 )
        {
            if (formula[i][-aux-1] == 1)
            {
                tautologia = 1;
            }
            formula[i][-aux-1] = -1;
        }
    }

    if (tautologia)
    {
        memset(formula[i], 0, numLiteral * sizeof(int));
        formula[i][0] = 2;
    }
    }

    return;
}


void imprimirFormula(int numClausulas, int numLiterais, int formula[numClausulas][numLiterais])
{
    printf("=== Fórmula na Memória: ===\n");
    for (int i = 0; i < numClausulas; i++)
    {
        for (int j = 0; j < numLiterais; j++)
        {
            printf("%d ", formula[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    return;
}

void imprimirResultado(bool resultado, arvore *header)
{
    if (resultado)
    {
        printf("SATISFIABLE\n");
        for (int i = 0; i < header->qVariaveis; i++)
        {
            if (header->interpretacaoParcial[i] != 0)
            {
                printf("X%d: %d ", i + 1, header->interpretacaoParcial[i]);
            }
        }
        printf("\n");
    }
    else
    {
        printf("UNSATISFIABLE\n");
    }

    return;
}

int main()
{
    int numLiterais;
    int numClausulas;

    lerInput(&numLiterais, &numClausulas);

    int formula[numClausulas][numLiterais];
    lerFormula(numClausulas, numLiterais, formula);
    imprimirFormula(numClausulas, numLiterais, formula);

    arvore *header = initArvore(numLiterais);
    bool resultado = sat(numClausulas, numLiterais, formula, header, header->raiz, 0);
    imprimirResultado(resultado, header);

    return 0;
}