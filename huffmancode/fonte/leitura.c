#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/leitura.h"


/** 
 * @brief cria um buffer para guardar os bytes lidos do arquivo
 * 
 * se o tamanho for != 0, bytes serao alocados e zerados (funcionamento do calloc)
 * 
 * @param tamanho eh a quantidade de bytes, vem do fseek() e ftell que busca dos metadados do arquvio
 * @return bufferDoArquivo* ponteiro para o buffer alocado na heap, ou NULL se malloc falhar
 */
bufferDoArquivo *initBuffer(int tamanho)
{
    bufferDoArquivo *buffer = (bufferDoArquivo*)malloc(sizeof(bufferDoArquivo));

    if (buffer == NULL)
    {
        return NULL;
    }
    
    buffer->tamanho = tamanho;

    if (tamanho != 0)
    {
        // "reserve 1 bloco de t tamanho e zere tudo, ", limpa o buffer
        buffer->bytes = calloc(1,tamanho);
        //devolve la pro bytes, que eh um ponteiro
    }
    else
    {
        buffer->bytes = NULL;
    }
    return buffer;
}

/**
 * @brief desaloca o espaco alocado para bufferDoArquivo
 * 
 * @param buffer buffer a ser EXTERMINADO da memoria
 */
void exterminarBuffer(bufferDoArquivo* buffer)
{
    free(buffer->bytes);
    free(buffer);
}

/**
 * @brief adiciona 1 byte por vezz ao buffer, incrementando o tamanho do buffer nessa mesma quanitdade
 * 
 * 
 * @param buffer endereco do buffer a ser adcionado um elemento
 * @param byte elemento a ser adicionado
 */
void adcionaraoBuffer(bufferDoArquivo *buffer, void* byte)
{
    //se o buffer estiver vazio
    if (buffer->bytes == NULL)
    {
        buffer->bytes = calloc(1,1);
        memcpy(buffer->bytes,byte,1);
        buffer->tamanho++;
        return;
    }
    buffer->bytes = realloc(buffer->bytes, buffer->tamanho+1);

    //vai copiar pra o primeiro byte livre depois de todos os ocupados, por isso soma tamanho)
    //eh aritimetica de ponteiro
    memcpy((buffer->bytes + buffer->tamanho), byte, 1);
    buffer->tamanho++;
        
}

/**
 * @brief le um arquivo e armazena suas infromacoes no buffer
 * 
 * @param enderecoDoArquivo endereco do arquivo a ser comprimido (obvio)
 * @return buffer da struct bufferDoArquivo onde foram armazedos os bytes do arquivo
 * em caso de erro, nao devolve nada, encerra o programa via mensagemDeErro()
 */
bufferDoArquivo* lerArquivo(char *enderecoDoArquivo)
{
    FILE *ptrArquivo;
    size_t tamanhoDoArquivo;

    ptrArquivo = fopen(enderecoDoArquivo,"rb");
    

    if (ptrArquivo == NULL)
    {
        mensagemDeErro("erro: arquivo nao existe");
    }

    //pegar o tamanho do arquivo
    if (fseek(ptrArquivo,0,SEEK_END) != 0)
    {
        mensagemDeErro("erro no fseek de LerArquivo");
    }
    tamanhoDoArquivo = ftell(ptrArquivo);

    if (tamanhoDoArquivo == -1)
    {
        mensagemDeErro("erro ao pegar o tamanho do arquivo");
    }
    rewind(ptrArquivo);

    //alocacao e preenchimento
    bufferDoArquivo *buffer = initBuffer(tamanhoDoArquivo);

    if (buffer == NULL || (tamanhoDoArquivo > 0 && buffer->bytes == NULL))
    {
        mensagemDeErro("Erro durante alocacao de memoria pro buffer");
    }
    fread(buffer->bytes,1,tamanhoDoArquivo,ptrArquivo);

    if (ferror(ptrArquivo))
    {
        mensagemDeErro("erro ao ler o arquivo");
    }
    if (fclose(ptrArquivo) == EOF)
    {
        mensagemDeErro("erro ao fechar o arquivo");
    }
    
    return buffer;
    
    
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