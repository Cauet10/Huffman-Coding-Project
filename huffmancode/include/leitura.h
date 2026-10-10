
#ifndef Leitura_h
#define Leitura_h


#define headersize 2
#define bytesize 8
#define TotaldeBytes 256


typedef struct buffer
{
    void *bytes; //pra guardar os bytes do arquivo lido
    int tamanho; // o tamano do arquivbo
}bufferDoArquivo;

bufferDoArquivo* initBuffer(int tamanho);

void adcionaraobuffer(bufferDoArquivo *buffer, void* byte);

void exterminarBuffer(bufferDoArquivo *buffer);

bufferDoArquivo* lerArquivo(char enderecoDoArquivo[]);

//a quantidade de bytes diferentes que podemos ter, combinacoes de 2^8

void ContarFrequencias(bufferDoArquivo* caminho, unsigned long long frequencias[TotaldeBytes]);

#endif