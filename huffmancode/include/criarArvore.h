#ifndef criarArvore_h
#define criarArvore_h

#include "leitura.h"

//todos os valores possiveis 
#define tamanhoHash 256

/**
 * @brief struct de um hashmap pra contar a frequencia de cada byte
 * 
 */

typedef struct hash
{
    unsigned char keys[tamanhoHash];
    /* essa array guarda os caracteres (keys) usados
     * vai ser usado depois pra ordenar por frequencia
     * uns char eh uma forma facil de representar valores de 0 a 255 bytes
     * pois tem o tamanho de 1 byte e vai de 0 a 255, facilitando a representacao em hashmap
     */
     int frequencia[tamanhoHash]; //frequencia das keys
     int tamanhoAtual; // quantidade de keys
} hashDeFrequencia;

/**
 * @brief struct huffmannode
 * cada no guarda o valor do byte original, a frequencia,o proximo no na lista encadeada,e o endereco dos nos filhos a
 * esquerda e a direita
 */
typedef struct huffmanNode
{
    void *valor; //valor do byte original guardado pelo respectivo no
    int frequencia; // frequencia do item nesse arquivo
    
    struct huffmanNode * next;
    struct huffmanNode * left;
    struct huffmanNode * right;
}huffmanNode;

hashDeFrequencia* initHash();

void aniquilarHash(hashDeFrequencia * h);

huffmanNode *criarNode(void *byte,int frequencia);

void adicionar(hashDeFrequencia* h, void* byteKey);

void trocarKeys(unsigned char *array,int idxA,int idxB);

int comparar(hashDeFrequencia* h, unsigned char keyA, unsigned char keyB);

void ordenarKeys(hashDeFrequencia *h, int inicio, int fim, int (*funcaoComparar)(hashDeFrequencia*, unsigned char, unsigned char));

void popularFrequencia(hashDeFrequencia* h, bufferDoArquivo *b);

huffmanNode* criarListadeHuffman(hashDeFrequencia* h);

huffmanNode* criarArvoreDeHuffman(huffmanNode* head);

huffmanNode* inserirHuffmanNode(huffmanNode* head, huffmanNode* newNode);

huffmanNode* criarNoDeFusao(huffmanNode* primeiro, huffmanNode* segundo);

void criarHuffmanTable(char *table[tamanhoHash], huffmanNode *NoDaArvore, char *c);






#endif