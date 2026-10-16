#ifndef _Descompressao_h
#define _Descompressao_h

#include "../include/leitura.h"
#include <stdbool.h>

typedef struct node
{
    bool folha;
    void *valor;
    struct node *left;
    struct node *right;
}descomp_node;

descomp_node *criar_no_descomp(void *byte, bool eh_folha);
descomp_node *criar_arvore_descomp(unsigned char *bufferDoArquivo, int tamanho, int *i);
bool bit_i_esta_definido(unsigned char byte, int i);
bufferDoArquivo *fazer_buffer_descomp(bufferDoArquivo *buffer, descomp_node *raiz, int lixo, int tamanho_arvore);
void descomprimir(char *caminho);
void escrever(bufferDoArquivo *buffer, char *nome_arquivo);


#endif