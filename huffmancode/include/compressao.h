#ifndef Compressao_h
#define Compressao_h

#include "leitura.h"
#include "criarArvore.h"

void pre_ordem(huffmanNode* noAtual, bufferDoArquivo* header, int *contador);
bufferDoArquivo *criar_header(huffmanNode* raiz, int tamanho_arvore, int tamanho_lixo);
bufferDoArquivo *criar_corpo(bufferDoArquivo* buffer, char* tabela[tamanhoHash], int* lixo);
void escrever_arquivo(bufferDoArquivo* header, bufferDoArquivo* corpo, char* nome_arquivo);
void liberar_buffer(bufferDoArquivo* b);
void liberar_arvore(huffmanNode* no);
void comprimir(char* path); 

#endif
