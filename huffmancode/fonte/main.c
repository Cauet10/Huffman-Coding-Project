#include "../include/leitura.h"
#include "../include/criarArvore.h"
#include "../include/compressao.h"
#include "../include/descompressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//mostra as opcoes do menu
void imprimirMenu()
{
    printf("\n--------------------------\n");
    printf("ESCOLHA UMA OPÇÃO:\n[1] - COMPACTAR ARQUIVO\n[2] - DESCOMPACTAR ARQUIVO\n[3] - SAIR");
    printf("\n--------------------------\n");
}

//pede o nome, monta inputs/<nome> e comprime; o .huff vai pra outputs/
//se o arquivo nao existir o lerArquivo ja chama a mensagemDeErro
void chamarComprimir()
{
    char nome[PATH_MAX];
    char caminho[PATH_MAX];

    printf("Informe o nome do arquivo (em %s): ", pastaEntrada);
    //o espaco pula o \n que sobrou do menu e o %[^\n] le a linha toda, com espacos
    scanf(" %[^\n]", nome);

    snprintf(caminho, sizeof(caminho), "%s%s", pastaEntrada, nome);
    comprimir(caminho);
    printf("Arquivo comprimido com sucesso! Salvo em %s%s.huff\n", pastaSaida, nome);
}

//pede o nome do .huff, monta outputs/<nome> e descomprime pra outputs/
void chamarDescomprimir()
{
    char nome[PATH_MAX];
    char caminho[PATH_MAX];
    int tamanho;

    printf("Informe o nome do arquivo .huff (em %s): ", pastaSaida);
    scanf(" %[^\n]", nome);

    //o tamanho <= 5 vem primeiro pra o || nao deixar o ponteiro cair antes do inicio do vetor
    //nome + tamanho - 5 aponta pros 5 ultimos chars, e o strcmp da 0 se forem ".huff"
    tamanho = strlen(nome);
    if (tamanho <= 5 || strcmp(nome + tamanho - 5, ".huff") != 0)
    {
        mensagemDeErro("O arquivo precisa ter extensão .huff!");
    }

    snprintf(caminho, sizeof(caminho), "%s%s", pastaSaida, nome);
    descomprimir(caminho);
    nome[tamanho - 5] = '\0';
    printf("Arquivo descompactado com sucesso! Salvo em %s%s\n", pastaSaida, nome);
}

int main()
{
    int op;

    while (1)
    {
        imprimirMenu();
        //se digitar algo que nao eh numero, cai no default e da erro
        if (scanf("%d", &op) != 1)
        {
            op = 0;
        }
        switch (op)
        {
            case 1:
                chamarComprimir();
                break;
            case 2:
                chamarDescomprimir();
                break;
            case 3:
                exit(0);
            default:
                mensagemDeErro("Opção inválida");
        }
    }
}
