
#ifndef Leitura_h
#define Leitura_h


#define headerSize 2

//pastas de onde as entradas sao lidas e pra onde as saidas vao (relativas a pasta huffmancode)
#define pastaEntrada "inputs/"
#define pastaSaida "outputs/"
#define byteSize 8
#define totaldeBytes 256

// fiz os nomes de funcao em camelCase pq acho agradavel
typedef struct buffer
{
    void *bytes; //pra guardar o endereço dos bytes do arquivo lido
    int tamanho; // o tamano do arquivbo
}bufferDoArquivo;

bufferDoArquivo* initBuffer(int tamanho);

void adcionaraoBuffer(bufferDoArquivo *buffer, void* byte);

void exterminarBuffer(bufferDoArquivo *buffer);

bufferDoArquivo* lerArquivo(char enderecoDoArquivo[]);

//a quantidade de bytes diferentes que podemos ter, combinacoes de 2^8

void mensagemDeErro(char mensagem[]);

#endif