#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include "compressao.h"

/**
 * Percorre a árvore de Huffman em pré-ordem e grava sua representação
 * serializada no buffer do cabeçalho.
 *
 * @param head     Nó atual da árvore (raiz na primeira chamada). Se for NULL,
 *                 a função retorna sem fazer nada.
 * @param header   Buffer do cabeçalho onde a árvore será gravada, a partir
 *                 da posição headerSize (primeiro e segundo bytes são o lixo e o tamanho da árvore).
 * @param contador Ponteiro para o índice de escrita atual, relativo a
 *                 headerSize. É incrementado a cada byte.
 * 
 */
void pre_ordem(huffmanNode* head, bufferDoArquivo* header, int *contador)
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

/**
 * Cria o cabeçalho do arquivo comprimido.
 *
 * @param head           Raiz da árvore de Huffman. Se for NULL (arquivo
 *                       vazio), é retornado um buffer vazio.
 * @param tamanho_arvore Tamanho em bytes da árvore serializada, incluindo
 *                       os caracteres de escape.
 * @param tamanho_lixo   Quantidade de bits de lixo no último byte do corpo.
 *
 * @return Ponteiro para o buffer do cabeçalho alocado, pronto para ser
 *         escrito no arquivo.
 * 
 * @note OR bit a bit funciona porque o buffer é inicializado com calloc (todos os bits definidos como 0).
 */
bufferDoArquivo *criar_header(huffmanNode* head, int tamanho_arvore, int tamanho_lixo)
{
    if(head == NULL) return initBuffer(0); //arquivo vazio

    //tinha um segundo initBuffer aqui que sobrescrevia esse e vazava memoria, tirei
    bufferDoArquivo *header = initBuffer(headerSize + tamanho_arvore);
    *(unsigned char*)(header->bytes) = *(unsigned char*)(header->bytes) | tamanho_lixo << 5;
    *(unsigned char*)(header->bytes+1) = *(unsigned char*)(header->bytes+1) | tamanho_arvore;
    *(unsigned char*)(header->bytes) = *(unsigned char*)(header->bytes) | tamanho_arvore >> 8; //pega apenas os bits junto com o lixo

    int contador = 0;
    pre_ordem(head, header, &contador);

    return header;
}
/**
 * Gera o corpo do arquivo comprimido, substituindo cada byte do arquivo
 * original pelo código de Huffman correspondente e empacotando os bits em
 * bytes de forma contínua.
 *
 * Se o último byte ficar incompleto, o restante é preenchido com zeros
 * (bits de lixo) e a quantidade de bits de lixo é gravada em *lixo.
 *
 * @param original Buffer com o conteúdo do arquivo original (não é alterado).
 * @param tabela   Tabela de códigos de Huffman, indexada pelo valor do byte.
 *                 Cada posição contém uma string de '0' e '1'.
 * @param lixo     Ponteiro de saída que recebe a quantidade de bits de lixo
 *                 (0 a 7) do último byte do corpo.
 *
 * @return Novo buffer contendo o corpo comprimido.
 */
bufferDoArquivo *criar_corpo(bufferDoArquivo* original, char* tabela[tamanhoHash], int* lixo)
{
    bufferDoArquivo* comprimido = initBuffer(0);

    unsigned char novo_byte = 0; //byte a ser montado.
    int n = 0; //variavel que determina o shift bit.

    for(int i = 0; i < original->tamanho; i++)
    {
        unsigned char valor_byte = *((unsigned char*)original->bytes + i);
        char* codigo = tabela[valor_byte];
        int tamanho_codigo = strlen(codigo);

        unsigned char mask;
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
                adcionaraoBuffer(comprimido, &novo_byte);
                novo_byte = 0;
                n = 0;
            }
        }
    }

    if(n != 0) //último byte incompleto: tem bits de lixo
    {
        *lixo = byteSize - n;
        adcionaraoBuffer(comprimido, &novo_byte);
    }
    else *lixo = 0; //não tem bits de lixo

    return comprimido;
}

/**
 * Escreve o arquivo comprimido, gravando o cabeçalho seguido do corpo.
 *
 * O arquivo de saída recebe o nome do arquivo original acrescido da
 * extensão ".huff" e é aberto em binário.
 *
 * @param header                 Buffer com o cabeçalho.
 * @param corpo                  Buffer com os dados comprimidos.
 * @param nome_arquivo_original  Nome do arquivo original. A string é
 *                               modificada, pois ".huff" é concatenado a
 *                               ela via strcat.
 */
void escrever_arquivo(bufferDoArquivo* header, bufferDoArquivo* corpo, char* nome_arquivo_original)
{
    //o .huff agora vai pra pasta de saida, entao reservo espaco pro nome dela tambem
    size_t n = strlen(pastaSaida) + strlen(nome_arquivo_original) + strlen(".huff") + 1;
    char* nome_arquivo_comprimido = malloc(n);
    if (nome_arquivo_comprimido == NULL)
    {
        fprintf(stderr, "Erro: falha de alocacao de memoria.\n");
        return;
    }
    snprintf(nome_arquivo_comprimido, n, "%s%s.huff", pastaSaida, nome_arquivo_original);

    FILE* fp = fopen(nome_arquivo_comprimido, "wb");
    //se a pasta de saida nao existir o fopen falha, ai encerro com a mensagemDeErro
    if (fp == NULL)
    {
        free(nome_arquivo_comprimido);
        mensagemDeErro("erro ao criar o arquivo .huff (a pasta outputs existe?)");
    }
    fwrite(header->bytes, 1, header->tamanho, fp);
    fwrite(corpo->bytes, 1, corpo->tamanho, fp);
    fclose(fp);
    free(nome_arquivo_comprimido);
}

/* Libera um buffer e seus bytes. */
void liberar_buffer(bufferDoArquivo* b)
{
    if(b == NULL) return;
    free(b->bytes);
    free(b);
}

/* Libera a árvore de Huffman recursivamente. */
void liberar_arvore(huffmanNode* no)
{
    if(no == NULL) return;
    liberar_arvore(no->left);
    liberar_arvore(no->right);
    free(no->valor);
    free(no);
}

/**
 * Comprime um arquivo usando o algoritmo de Huffman.
 *
 * @param path Caminho do arquivo a ser comprimido.
 *
 * O arquivo comprimido é gravado com base no nome obtido por basename(path).
 */
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
    //se so tem um tipo de byte, a criarArvoreDeHuffman bota uma folha ficticia, entao a arvore tem 3 nos e nao 1
    //o +2 eh a folha ficticia + o no de fusao (ela eh 0 ou 1, entao nunca precisa de escape)
    if (hash->tamanhoAtual == 1)
    {
        tamanho_arvore += 2;
    }
    bufferDoArquivo* header = criar_header(arvore, tamanho_arvore, lixo);
    aniquilarHash(hash);

    char* nome_arquivo_original = basename(path);
    escrever_arquivo(header, corpo, nome_arquivo_original);

    //liberação de memoria
    for(int i = 0; i < tamanhoHash; i++) free(tabela[i]);
    liberar_arvore(arvore);
    liberar_buffer(header);
    liberar_buffer(corpo);
    liberar_buffer(original);
}
