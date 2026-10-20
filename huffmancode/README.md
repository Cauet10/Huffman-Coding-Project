# Compressor de Huffman

Compacta e descompacta **qualquer tipo de arquivo** (texto, imagem, PDF...) usando a codificação de Huffman.

## Estrutura

```
huffmancode/
├── fonte/            código-fonte (.c)
│   ├── main.c            menu do programa
│   ├── leitura.c         leitura do arquivo em um buffer de bytes
│   ├── criarArvore.c     frequências, lista ordenada, árvore e tabela de códigos
│   ├── compressao.c      gera o .huff (cabeçalho + corpo)
│   └── descompressao.c   reconstrói a árvore e o arquivo original
├── include/          headers (.h)
├── inputs/           coloque aqui os arquivos a compactar
├── outputs/          aqui saem os .huff e os arquivos descompactados
└── testes_leitura/   arquivos e programas de teste
```

## Compilar

Abra o terminal **dentro da pasta `huffmancode`** e rode:

```
gcc -Iinclude fonte/leitura.c fonte/criarArvore.c fonte/compressao.c fonte/descompressao.c fonte/main.c -o huffman.exe
```

> Se aparecer "gcc não é reconhecido", use o caminho completo: `C:\msys64\ucrt64\bin\gcc.exe`.

## Rodar

Ainda dentro de `huffmancode` (as pastas `inputs/` e `outputs/` são procuradas a partir de onde o programa é executado):

```
.\huffman.exe
```

```
--------------------------
ESCOLHA UMA OPÇÃO:
[1] - COMPACTAR ARQUIVO
[2] - DESCOMPACTAR ARQUIVO
[3] - SAIR
--------------------------
```

### Compactar

1. Coloque o arquivo em `inputs/`.
2. Escolha `1` e digite **só o nome** do arquivo (ex.: `fototerence.png`).
3. O resultado sai em `outputs/fototerence.png.huff`.

### Descompactar

1. Escolha `2` e digite **só o nome** do `.huff` que está em `outputs/` (ex.: `fototerence.png.huff`).
2. O arquivo original é recriado em `outputs/fototerence.png`.

### Conferir se voltou igual

```
fc.exe /b inputs\fototerence.png outputs\fototerence.png
```

"Nenhuma diferença encontrada" = a descompactação reconstruiu o arquivo byte a byte.

## Erros

Qualquer erro mostra uma mensagem e **encerra o programa** (função `mensagemDeErro`):

| situação | mensagem |
|---|---|
| arquivo não está na pasta | `erro: arquivo nao existe` |
| descompactar algo que não termina em `.huff` | `O arquivo precisa ter extensão .huff!` |
| pasta `outputs/` não existe | `erro ao criar o arquivo .huff (a pasta outputs existe?)` |
| opção do menu inválida | `Opção inválida` |

## Formato do arquivo .huff

```
| 3 bits: lixo | 13 bits: tamanho da árvore | árvore em pré-ordem | corpo comprimido |
 \_______________ 2 bytes ________________/
```

- **Lixo:** quantos bits do último byte do corpo são só preenchimento (0 a 7).
- **Tamanho da árvore:** quantos bytes a árvore ocupa no cabeçalho.
- **Árvore em pré-ordem:** `*` = nó interno; qualquer outro byte = folha. Folhas que são `*` ou `\` vão precedidas de `\` (escape), pra não serem confundidas com nó interno.
- **Corpo:** os códigos de Huffman de cada byte do original, colados bit a bit.

## Casos especiais

- **Arquivo vazio:** gera um `.huff` vazio, e descompactá-lo gera um arquivo vazio.
- **Arquivo com um único tipo de byte** (ex.: `AAAA`): uma folha sozinha teria código vazio, então é adicionada uma folha fictícia de frequência 0 pra forçar uma fusão; o byte real ganha código de 1 bit.
- **Arquivo pequeno pode ficar maior** depois de compactado, porque o cabeçalho (2 bytes + árvore) pesa mais que a economia.
