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

/**
 * @brief cria um novo no
 * 
 * @param indexDaVar  index da variavel na interpretacao parcial
 * @param valor booleano associado a variavel
 * @return node* novo no
 */
node* criarNode(int indexDaVar, bool valor)
{
    node *novoNode = calloc(1,sizeof(node));
    novoNode->indexDaVariavel = indexDaVar;
    novoNode->valor = valor;

    return novoNode;
}

/**
 * @brief addiciona um novo no a arvore
 * 
 * @param header header da arvore
 * @param atual no pai, que recebe o novo no como filho
 * @param indexVar index da variavel na interpretacao parcial
 * @param valor valor booleano associado a variavel
 * @return node* 
 */
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
/**
 * @brief a funcao testa se a formula, com a atual interpretacao parcial, eh verdadeira, 
 * analogo a um gabarito, sendo a interpretacao parcial nossa folha de respostas a ser comparada
 * 
 * @param numeroClausulas numero de clausulas
 * @param numVariaveis numero de variaveis
 * @param formula matriz onde a formula eh guardada
 * @param interpParcial lista (vetor) da interpretacao parcial
 * @return true  se a formula for verdadeira com a Int Parcial
 * @return false  se alguma clausula ainda nao tiver acerto: a formula pode estar inconclusiva ou ja falsa, lembrando que inconcliusiva != unsat
 */
bool satisfas(int numeroClausulas, int numVariaveis, int formula[numeroClausulas][numVariaveis], int interpParcial[maxInterpParcial])
{
    for (int i = 0; i < numeroClausulas; i++)
    {
        bool satisfeito = false;

        for (int j = 0; j < numVariaveis; j++)
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

/**
 * @brief testa se a formula com a atual Ip Eh falsa: admite que cada clausula esta falsificada e busca
 * salvar cada uma, procurando por:
 * tautologias (2)
 * literal da clausula cuja variavel ainda nao tem valor na ip (ip[j] == 0)
 * coincidencia entre formula e ip para quando o elemento ip[j] != 0
 * se alguma clausula nao puder ser salva, a formula eh contraditoria
 *
 * @param numClausulas numero de clausulas
 * @param numVariaveis numero de variaveis
 * @param formula matriz da formula
 * @param interpParcial lista(Vetor) da IP
 * @return true se a formula for realmente contraditoria
 *
 * @return false se nenhuma clausula estiver falsificada: a formula pode estar inconclusiva ou ja verdadeira, lembrando, novamente inconclusiva != unsat
 */
bool contradiz(int numClausulas,int numVariaveis, int formula[numClausulas][numVariaveis],int interpParcial[maxInterpParcial])
{
     for (int i = 0; i < numClausulas; i++)
    {
        bool contradito = true;

        for (int j = 0;j < numVariaveis; j++)
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
 * @brief funcao recursiva que constroi a arvore binaria (cada chamada eh um no) e testa a formula com a ip atual
 * 
 * 
 * @param numClausulas numero de clausulas 
 * @param numVariaveis numero de variaveis
 * @param formula matriz da formula
 * @param header header da arvore
 * @param atual no atual onde o novo no sera adicionado ou a esquerda (caso true) ou a direita (caso false)
 * @param varIndxex index da variavel na Ip
 * @return true se a formula for sat
 * @return false se nao existir solucao a partir da ip atual (na primeira chamada, isso quer dizer unsat)
 *
 * se satisfaz e contradiz forem inconclusivos, chuta true e depois false para a variavel atual;
 * se os dois chutes falharem, faz backtracking: zera a variavel e retorna false para o nivel de cima tentar outro valor
 */
bool sat(int numClausulas,int numVariaveis, int formula[numClausulas][numVariaveis], arvore *header, node *atual, int varIndxex)
{
    if (satisfas( numClausulas,  numVariaveis, formula,header->interpretacaoParcial) )
    {
        return true;
    }
    if (contradiz(numClausulas, numVariaveis, formula,header->interpretacaoParcial))
    {
        return false;
    }
    
    node *nodeTrue = adicionarNode(header,atual,varIndxex, true);
    if (sat(numClausulas,numVariaveis,formula,header,nodeTrue,varIndxex+1))
    {
        return true;
    }
    
    node *nodeFalse = adicionarNode(header,atual,varIndxex,false);
    if (sat(numClausulas,numVariaveis,formula,header,nodeFalse,varIndxex+1))
    {
        return true;
    }

    header->interpretacaoParcial[varIndxex] = 0; // backtracking 

    return false;
    
}

/**
 * @brief inicializa a header(arvore)
 * 
 * @param qVariaveis quantidade de variaveis na formula
 * @return arvore* arvore (header) gereada
 */
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
/**
 * @brief le o input e extrai o numero de variaveis e clausulas
 * 
 * @param variavel ponteiro pra guardar o numero de varaiveis
 * @param clausula ponteiro pra guardar o numero de clausualas
 */
void lerInput(int *variavel, int *clausula)
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
            scanf("%d %d", variavel, clausula);
            if (*variavel <= 0)
            {
                mensagemDeErro("\n numero de variaveis invalido! use um valor valido ");
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
/**
 * @brief le a formula e guarda na matriz
 * 
 * @param numClausula numero de clausulas
 * @param numVariavel numero de variaveis
 * @param formula matriz criada
 */
void lerFormula(int numClausula, int numVariavel, int formula[numClausula][numVariavel])
{
    //inicializa a matriz com tudo zerado
    memset(formula,0, numClausula * numVariavel * sizeof(int));

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

    if (tautologia)// no caso de tautologia, a clausula recebe a flag (2) pra ser tratada como sempre satisfeita em satisfaz() e contradiz()
    {
        memset(formula[i], 0, numVariavel * sizeof(int)); // limpa a clausula
        formula[i][0] = 2; // flag tautologia
    }
    }

    return;
}

/**
 * @brief printa a formula na memoria
 * 
 * @param numClausulas numero de clausulas
 * @param numVariaveis numero de variaveis(lit)
 * @param formula matreiz onde a formula ta guardada
 */
void imprimirFormula(int numClausulas, int numVariaveis, int formula[numClausulas][numVariaveis])
{
    printf("=== Formula na Memoria: ===\n");
    for (int i = 0; i < numClausulas; i++)
    {
        for (int j = 0; j < numVariaveis; j++)
        {
            printf("%d ", formula[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    return;
}

/**
 * @brief printa o resultado do sat solver
 * 
 * @param resultado resultado do sat, se TRUE: SAT se FALSE: UNSAT 
 * @param header 
 */
void imprimirResultado(bool resultado, arvore *header)
{
    if (resultado)
    {
        printf("SAT\n");
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
        printf("UNSAT\n");
    }

    return;
}

int main()
{
    int numVariaveis;
    int numClausulas;

    lerInput(&numVariaveis, &numClausulas);

    int formula[numClausulas][numVariaveis];
    lerFormula(numClausulas, numVariaveis, formula);
    imprimirFormula(numClausulas, numVariaveis, formula);

    arvore *header = initArvore(numVariaveis);
    bool resultado = sat(numClausulas, numVariaveis, formula, header, header->raiz, 0);
    imprimirResultado(resultado, header);

    return 0;
}


// o segredo da inteligencia eh a boina italiana (favor nao confundir com francesa)