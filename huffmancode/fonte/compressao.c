#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compressao.h"

void pre_ordem(huffmanNode* head, bufferDoArquivo* header, int *contador) //contador vai começar em 2 (para pular os dois primeiros bytes com o lixo e o tamanho da arvore)
{
    
    if(head == NULL) return;//arquivo vazio

    if(head->right == NULL && head->left == NULL) //verifica se é folha
    {
        if(*(unsigned char*)(head->valor) == '*' || *(unsigned char*)(head->valor) == '\\') //verifica se a folha é uma barra ou asterisco. necessario para nao confundir com nó interno ou caracter de escape.
        {
            *(unsigned char*)(header->bytes + headerSize + *contador) = *(unsigned char*)(header->bytes + headerSize + *contador) | '\\';
            (*contador)++;
        }
        *(unsigned char*)(header->bytes + headerSize + *contador) = *(unsigned char*)(header->bytes + headerSize + *contador) | *(unsigned char*)(head->valor);
    }
    else //nó interno
    {
        *(unsigned char*)(header->bytes + headerSize + *contador) = *(unsigned char*)(header->bytes + headerSize + *contador) | '*';
    }
    (*contador)++;
    pre_ordem(head->left, header, contador);
    pre_ordem(head->right, header, contador);
}

bufferDoArquivo *criar_header(huffmanNode* head, int tamanho_arvore, int tamanho_lixo)
{
    bufferDoArquivo *header = initBuffer(headerSize + tamanho_arvore);

    if(head == NULL) return header = initBuffer(NULL);//arquivo vazio

    header = initBuffer(headerSize + tamanho_arvore);
    *(unsigned char*)(header->bytes) = *(unsigned char*)(header->bytes) | tamanho_lixo << 5;
    *(unsigned char*)(header->bytes+1) = *(unsigned char*)(header->bytes+1) | tamanho_arvore;
    *(unsigned char*)(header->bytes) = *(unsigned char*)(header->bytes) | tamanho_arvore >> 8; //pega apenas os bits junto com o lixo

    int contador = 0;
    pre_ordem(head, header, &contador);

    return header;
}

bufferDoArquivo *criar_corpo(bufferDoArquivo* original, char* tabela[tamanhoHash], int* lixo)
{
    bufferDoArquivo* comprimido = initBuffer(0);

    for(int i = 0; i < original->tamanho; i++)
    {
        unsigned char valor_byte = *(unsigned char*)(original->bytes+i);
        char* codigo = tabela[valor_byte]; //codigo novo comprimido
        int tamanho_codigo = strlen(codigo);

        unsigned char novo_byte = 0, mask; //novo byte é zero pois ainda precisa ser setado.
        int n = 0; //variável que determina o shift bit
        for(int j = 0; j < tamanho_codigo; j++)
        {
            mask = 0b10000000 >> n;
            if(codigo[j] == '1') //bit setado
            {
                novo_byte = novo_byte | mask;
            }
            n++;
            if(n == byteSize)
            {
                adcionaraoBuffer(original, &novo_byte);
                novo_byte = 0;
                n = 0;
            }
        }
        
        if(n != 0) //tem bit lixo
        {
            *lixo = byteSize - n;
            adcionaraoBuffer(original, &novo_byte);
        }
        else *lixo = 0; //não tem bit lixo

        return original;
    }
    
}

void escrever_arquivo(bufferDoArquivo* header, bufferDoArquivo* corpo, char* nome_arquivo_original)
{
    char* nome_arquivo_comprimido = strcat(nome_arquivo_original, ".huff");
    FILE* fp = fopen(nome_arquivo_comprimido, "wb");
    fwrite(header->bytes, 1, header->tamanho, fp);
    fwrite(corpo->bytes, 1, corpo->tamanho, fp);
    fclose(fp);
}

void comprimir(char* path)
{
    //processo de montagem da arvore huffman
    bufferDoArquivo* original = lerArquivo(path);
    hashDeFrequencia* hash = initHash();
    popularFrequencia(hash, original);
    huffmanNode* head = criarListadeHuffman(hash);
    huffmanNode* arvore = criarArvoreDeHuffman(head);

    int lixo = 0;
    char *tabela[tamanhoHash] = {NULL};
    criarHuffmanTable(tabela, arvore, "");
    bufferDoArquivo* corpo = criar_corpo(original, tabela, &lixo);

    int escape_char = 0;
    escape_char += (hash->frequencia['*'] > 0);
    escape_char += (hash->frequencia['\\'] > 0);
    int tamanho_arvore = (2 * hash->tamanhoAtual) - 1 + escape_char;
    bufferDoArquivo* header = criar_header(arvore, tamanho_arvore, lixo);
    aniquilarHash(hash);

    char* nome_arquivo_original = basename(path);
    escrever_arquivo(header, corpo, nome_arquivo_original);
}
