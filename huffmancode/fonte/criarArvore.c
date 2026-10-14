#include "../include/criarArvore.h"
#include<string.h>
#include<stdlib.h>


/**
 * @brief cria a tabela de forma recursiva
 * 
 * @param table tabela a ser preenchida
 * @param NoDaArvore no atual da arvore
 * @param codigo codigo sendo construido no momento (codigo eh o caminho ate o no)
 */
void criarHuffmanTable(char *table[tamanhoHash], huffmanNode *NoDaArvore, char *codigo)
{
    //arvore vazia
    if (NoDaArvore == NULL)
    {
        return;
    }
    //no na verade eh folha (nao tem filhos)
    if (NoDaArvore->left == NULL && NoDaArvore->right == NULL)
    {
        unsigned char index = *(unsigned char*)NoDaArvore->valor;
        table[index] = strdup(codigo);
        return;
    }

    //em caso de no interno, prepara o codigo de cada filho e desse pros dois

    //reservar espaço
    char esquerdaCodigo[strlen(codigo)+ 2]; // +2 por conta do espaço para o novo char e o \0
    char direitaCodigo[strlen(codigo)+ 2]; 
    
    //copiar o tamanho attual pros filhos
    strcpy(esquerdaCodigo,codigo);
    strcpy(direitaCodigo,codigo);

     //direcao

    strcat(esquerdaCodigo,"0");
    strcat(direitaCodigo,"1");

    //descer pela arvore

    criarHuffmanTable(table,NoDaArvore->left,esquerdaCodigo);
    criarHuffmanTable(table,NoDaArvore->right,direitaCodigo);
    
}

/**
 * @brief cria um no interno de fusao que une dois nos por (menor) frequencia
 * 
 * @param primeiro primeiro no
 * @param segundo segundo no
 * @return huffmanNode*  retorna o no que une os dois nos
 */
huffmanNode* criarNoDeFusao(huffmanNode* primeiro, huffmanNode* segundo)
{
    huffmanNode* noDeFusao = (huffmanNode*)malloc(sizeof(huffmanNode));
    noDeFusao->valor = NULL;
    noDeFusao->frequencia = primeiro->frequencia + segundo->frequencia;
    noDeFusao->next = NULL;
    noDeFusao->left = primeiro;
    noDeFusao->right = segundo;
    
    return noDeFusao;
}

/**
 * @brief inserere um novo node no seu lugar apropriado
 * 
 * @param head head da lista encadeada
 * @param newNode novo no a ser adicionado
 * @return novo no, se for o primeiro ou ja estiver no local adequado
 * ou retorna a head caso passe pelo "ordenamento"no while3
 */
huffmanNode* inserirHuffmanNode(huffmanNode* head, huffmanNode* newNode)
{
    if (head == NULL || newNode->frequencia < head->frequencia)
    {
        newNode->next = head;
        return newNode;
    }
    
    huffmanNode *atual = head;
    //segunda parte do && faz com que ele fique no lugar certo pela frequencia na lista
    while (atual->next != NULL && atual->next->frequencia <= newNode->frequencia)
    {
        atual = atual->next;
    }
    //depois de caminhar no while, ele faz esse swap basico pra organizar 
    newNode->next = atual->next;
    atual->next = newNode;
    return head;
    
}

/**
 * @brief faz a arvore do huffman, a essencia dele ta no while
 * essencia do huffman, enquanto tiver ao menos 2 nos funde os de menor frequencia, quando so sobrar um
 *  eh a raiz (head)
 * @param head raiz
 * @return head, raiz eh o que sobra apos as fusoes
 */
huffmanNode* criarArvoreDeHuffman(huffmanNode* head)
{
    
    // se o arquivo so tem um tipo de byte, a arvore seria uma folha sozinha e o codigo dela ficaria vazio
    // entao eu crio uma folha ficticia (byte diferente, frequencia 0) pra forcar uma fusao e o byte real ganhar 1 bit
    if (head != NULL && head->next == NULL)
    {
    unsigned char ficticio = 0;
    
    if (*(unsigned char*)head->valor == 0)
    {
        ficticio = 1;
    }
    head = inserirHuffmanNode(head, criarNode(&ficticio, 0));
}
    while (head !=NULL && head->next != NULL)
    {
        huffmanNode *primeiro = head;
        huffmanNode *segundo = head->next;
        huffmanNode *noFundido = criarNoDeFusao(primeiro,segundo);
        head = segundo->next;
        head = inserirHuffmanNode(head,noFundido);
        primeiro->next = segundo->next = NULL;
    }
    return head;
}

/**
 * @brief inicializa o hash de frequencia
 * 
 * @return hash, o endereco da hash inicialiozada e vazia
 */
hashDeFrequencia* initHash()
{
    //com calloc no lugar de malloc, alocamos ja zerando as 256 frequencias e keys
    hashDeFrequencia *hash = calloc(1, sizeof(hashDeFrequencia));

    if (hash == NULL)
    {
        mensagemDeErro("erro na alocacao de memoria pro hash");
    }

    return hash;
}

/**
 * @brief desaloca o hash de frequencia da memoria
 * 
 * @param h hash a ser ANIQUILADO!!
 */
void aniquilarHash(hashDeFrequencia * h)
{
    free(h);
}

/**
 * @brief cria um novo huffmanNode
 * 
 * apesar de poder fazer com calloc, fica mais facil de visualizar as atribuicoes dessa forma
 * @param byte ponteiro do byte de um arquivi
 * @param frequencia frequencia desse byte no arquivo
 * @return huffmanNode* com todos os ponteiros aninhados apontando pra null
 */
huffmanNode *criarNode(void *byte,int frequencia)
{
    huffmanNode *novoNo = (huffmanNode*)malloc(sizeof(huffmanNode));
    if (novoNo == NULL)
    {
        mensagemDeErro("erro na alocacao de memoria de HuffmanNode");
    }
    novoNo->valor = malloc(1);

    if (novoNo->valor == NULL)
    {
        mensagemDeErro("erro na alocacao de memoria de HuffmanNode");
    }
    //copia informacao do byte pro no
    memcpy(novoNo->valor, byte,1);
    novoNo->frequencia = frequencia;
    novoNo->next = novoNo->right = novoNo->left = NULL;
    return novoNo;
    
}

void adicionar(hashDeFrequencia* hash, void* byteKey)
{
    unsigned char key = *(unsigned char*)byteKey;
    if (hash->frequencia[key] == 0)
    {
        hash->keys[hash->tamanhoAtual++] = key;
    }
    hash->frequencia[key]++;
    
}
/**
 * @brief troca de lugar dois elementos do arrayde keys
 *
 * @param array vetor de keys
 * @param idxA posicao do primeiro elemento
 * @param idxB posicao do segundo elemento
 */
void trocarKeys(unsigned char *array, int idxA, int idxB)
{
    unsigned char temp = array[idxA];
    array[idxA] = array[idxB];
    array[idxB] = temp;
}

/**
 * @brief compara dois bytes pela frequencia; no empate, pelo valor do byte
 *
 * @param h hash com as frequencias
 * @param keyA primeiro byte
 * @param keyB segundo byte
 * @return negativo se keyA vem antes, positivo se vem depois, 0 se sao iguais
 */
int comparar(hashDeFrequencia *h, unsigned char keyA, unsigned char keyB)
{
    int freqA = h->frequencia[keyA];
    int freqB = h->frequencia[keyB];

    if (freqA != freqB)
    {
        return freqA - freqB;
    }
    //so importa o sinal dessa operacao pra saber a ordem correta 

    return keyA - keyB;
}

/**
 * @brief ordena a lista usando quick sort
 *
 * @param h hash com as keys e as frequencias
 * @param inicio primeira posicao do trecho a ordenar
 * @param fim ultima posicao do trecho a ordenar
 * @param Comparar funcao que decide a ordem entre duas keys
 */
void ordenarKeys(hashDeFrequencia *h, int inicio, int fim, int (*Comparar)(hashDeFrequencia*, unsigned char, unsigned char))
{
    unsigned char pivo;
    int i;
    int j;

    if (inicio >= fim || inicio < 0)
    {
        return;
    }

    pivo = h->keys[fim];
    i = inicio;

    for (j = inicio; j < fim; j++)
    {
        if (Comparar(h, h->keys[j], pivo) <= 0)
        {
            trocarKeys(h->keys, i, j);
            i++;
        }
    }

    trocarKeys(h->keys, i, fim);
    ordenarKeys(h, inicio, i - 1, Comparar);
    ordenarKeys(h, i + 1, fim, Comparar);
}

/**
 * @brief  conta a frequencia de cada byte do buffer e ordena as keys
 *
 * @param h hash vazio,criado pela initHash
 * @param b buffer com os bytes do arquivo
 */
void popularFrequencia(hashDeFrequencia *h, bufferDoArquivo *b)
{
    unsigned char *dados = b->bytes;
    int i;

    for (i = 0; i < b->tamanho; i++)
    {
        adicionar(h, &dados[i]);
    }

    ordenarKeys(h, 0, h->tamanhoAtual - 1, comparar);
}

/**
 * @brief cria a lista encadeada de nos a partir das keys ja ordenadas
 *
 * @param h hash com as keys ordenadas por frequencia
 * @return primeiro no da lista, ou NULL se o hash estiver vazio
 */
huffmanNode* criarListadeHuffman(hashDeFrequencia *h)
{
    huffmanNode *head;
    huffmanNode *atual;
    huffmanNode *novoNo;
    unsigned char key;
    int i;

    if (h->tamanhoAtual == 0)
    {
        return NULL;
    }

    head = criarNode(&h->keys[0], h->frequencia[h->keys[0]]);
    atual = head;

    for (i = 1; i < h->tamanhoAtual; i++)
    {
        key = h->keys[i];
        novoNo = criarNode(&key, h->frequencia[key]);
        atual->next = novoNo;
        atual = atual->next;
    }

    return head;
}