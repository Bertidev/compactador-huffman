#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

//cria um novo nó da arvore
No* criar_no(U8 caracter, U32 frequencia, No* esquerda, No* direita) {
    No* no = (No*)malloc(sizeof(No));    //aloca memoria para um nó
    no->caracter = caracter;             //inicializa caracter, frequencia e os nós filhos
    no->frequencia = frequencia;
    no->esquerda = esquerda;
    no->direita = direita;
    return no;
}

//fila de prioridade 
void inserir_ordenado(No** fila, U32* tamanho, No* no) {
    U32 i = *tamanho;
    while (i > 0 && fila[i - 1]->frequencia < no->frequencia) {    //aqui acontece a ordenação, quanto i>0 e freq_fila<freq_no
        fila[i] = fila[i - 1];
        i--;
    }
    fila[i] = no;
    (*tamanho)++;
}

No* remover_menor(No** fila, U32* tamanho) {        //remove os primeiros nós -> remove o menor valor
    (*tamanho)--;
    return fila[*tamanho];
}

void gerar_codigos(No* raiz, Codigo codigo_atual, Codigo* tabela) { //navega na arvore recursivamente para mapear os bits de cada caractere
    if (raiz == NULL) return; //protecao caso o nó seja nulo

    if (raiz->esquerda == NULL && raiz->direita == NULL) { //se nao tem filhos, achou o caractere
        clone_codigo(codigo_atual, &tabela[raiz->caracter]); //salva o codigo acumulado na tabela, usando o caractere como indice
        return; //encerra esse caminho
    }

    Codigo esq_cod, dir_cod; //variaveis temporarias para os caminhos da esquerda e direita
    
    clone_codigo(codigo_atual, &esq_cod); //copia o caminho percorrido ate aqui
    adiciona_bit(&esq_cod, 0); //adiciona o bit 0 porque vai descer pra esquerda
    gerar_codigos(raiz->esquerda, esq_cod, tabela); //chama a funcao de novo descendo para o filho esquerdo
    free_codigo(&esq_cod); //libera a copia da memoria na volta da recursao

    clone_codigo(codigo_atual, &dir_cod); //copia o caminho percorrido ate aqui
    adiciona_bit(&dir_cod, 1); //adiciona o bit 1 porque vai descer pra direita
    gerar_codigos(raiz->direita, dir_cod, tabela); //chama a funcao de novo descendo para o filho direito
    free_codigo(&dir_cod); //libera a copia da memoria na volta da recursao
}

void liberar_arvore(No* raiz) {    //liebra toda a memoria usada pela arvore
    if (raiz == NULL) return;      //quando a raiz estiver vazia significa que terminou de liberar, entao retorna
    liberar_arvore(raiz->esquerda);    //chama recursivamente para os nós da esquerda e da direita
    liberar_arvore(raiz->direita);
    free(raiz);    //libera o nó raiz
}

//funcao que compacta 
void compactar(const char* arquivo_entrada, const char* arquivo_saida) {
    FILE* in = fopen(arquivo_entrada, "rb");        //abre o arquivo, cria um ponteiro apontando para ele e define que vai ser lido em binario
    if (!in) { printf("Erro ao abrir entrada.\n"); return; }    //caso de erro na leitura do arquivo

    U32 frequencias[256] = {0};    //cria um vetor com 256 posicoes
    U8 buffer;                     //cria variavel de 1 byte      
    while (fread(&buffer, 1, 1, in)) {    //fread le 1 byte de "in", e armazena em buffer, o loop while continua até o arquivo acabar
        frequencias[buffer]++;            //buffer passa o caracter para as frequencias, entao se for a letra 'A' ele vai somar 1 na frequencia da letra
    }

    No* fila[256];        //inicializa a fila de prioridades com 256 ponteiros (um para cada byte possivel)
    U32 tamanho_fila = 0;    //controla quantos nós estão sendo gerenciados 
    for (I32 i = 0; i < 256; i++) {    //insere os caracteres na fila 
        if (frequencias[i] > 0) {      //filtra de modo que caracteres que não aparecem no texto não vão entrar na fila 
            inserir_ordenado(fila, &tamanho_fila, criar_no(i, frequencias[i], NULL, NULL)); //insere na fila
        }
    }

    while (tamanho_fila > 1) {            //enquanto a fila tiver mais que 1 nó vai continuar agrupando 
        No* esq = remover_menor(fila, &tamanho_fila);        //remove os dois menores nós da fila e adiciona nos nós filhos da arvore
        No* dir = remover_menor(fila, &tamanho_fila);
        No* pai = criar_no(0, esq->frequencia + dir->frequencia, esq, dir);    //cria nó dentro da árvore, com o pai sendo a soma das frequencias dos dois filhos 
        inserir_ordenado(fila, &tamanho_fila, pai);    
    }
    No* raiz = fila[0];    //final da sequencia, o nó final da fila será a árvore completa

    Codigo tabela[256];        //tabela onde cada bute possivel vai ter um codigo huffman associado
    for (I32 i = 0; i < 256; i++) tabela[i].byte = NULL;    //null é necessario, sem ele vai apontar para lixo de memória
    
    Codigo cod_base;    //cria um código vazio
    novo_codigo(&cod_base);
    gerar_codigos(raiz, cod_base, tabela);    //a funcao vai percorrer a arvore construindo caminhos e salvar na tabela
    free_codigo(&cod_base);        //libera a memoria do codigo base, nao é mais necessario depois de criar a tabela

    FILE* out = fopen(arquivo_saida, "wb");    //vai escrever o arquivo compactado, write binary
    fwrite(frequencias, sizeof(U32), 256, out); //salva a tabela de frequencias no comeco do arquivo 

    fseek(in, 0, SEEK_SET);    //como o arquivo original foi percorrido até o fim, preciso voltar o ponteiro para a posicao inicial
    U8 byte_saida = 0;         //buffer de 1 byte
    U32 bits_escritos = 0;     //monitora o buffer bit a bit 

    while (fread(&buffer, 1, 1, in)) {        //le 1 byte do arquivo original, buffer=byte atual
        Codigo c = tabela[buffer];            //busca na tabela a sequencia de bits que representa o byte
        for (U32 i = 0; i < c.tamanho; i++) {        //o for trabalha sobre a sequencia de bits atual
            U8 bit = (c.byte[i / 8] >> (7 - (i % 8))) & 1;    //extrai o valor especifico de um bit de dentro do vetor c.byte
            if (bit) byte_saida |= (1 << (7 - bits_escritos));    //se o bit for 1, aplica um OR para ativá-lo na posição correta da saída (preenche da esquerda pra direita)  
            bits_escritos++;    

            if (bits_escritos == 8) {    //quando atinge 8 preenche um byte
                fwrite(&byte_saida, 1, 1, out);
                byte_saida = 0;
                bits_escritos = 0;
            }
        }
    }
    if (bits_escritos > 0) fwrite(&byte_saida, 1, 1, out);    //garante que se passar do loop anterior e sobrar um numero de bits que nao preenche um byte
    //ele vai preencher o resto do byte com 0

    for (I32 i = 0; i < 256; i++) free_codigo(&tabela[i]);    //libera a memoria de todos os 256 vetores de bits alocados
    liberar_arvore(raiz);    //libera a memoria usada pela arvore de huffman 
    fclose(in);    //encerra o processo de I/O pros dois arquivos
    fclose(out);
}

void descompactar(const char* arquivo_entrada, const char* arquivo_saida) {
    FILE* in = fopen(arquivo_entrada, "rb");            //aponta para o inicio do arquivo compactado, rb read byte
    if (!in) { printf("Erro ao abrir entrada.\n"); return; }

    U32 frequencias[256];    //cria um vetor para armazenar a frequencia de cada byte
    fread(frequencias, sizeof(U32), 256, in);    //le o cabecalho do arquivo para recuperar as frequencias

    No* fila[256];            //cria a fila de frequencias 
    U32 tamanho_fila = 0;     //tamanho inicial da fila
    U32 total_caracteres = 0; //quantidade total de caracteres reais do arquivo

    for (I32 i = 0; i < 256; i++) {    
        if (frequencias[i] > 0) {
            inserir_ordenado(fila, &tamanho_fila, criar_no(i, frequencias[i], NULL, NULL)); //poe na fila apenas os caracteres que existem
            total_caracteres += frequencias[i]; //soma para saber o total de caracteres originais a restaurar
        }
    }

    if (tamanho_fila == 0) { fclose(in); return; } //se o arquivo estiver vazio, aborta

    while (tamanho_fila > 1) { //refaz a arvore de huffman identica a da compactacao
        No* esq = remover_menor(fila, &tamanho_fila); //tira o menor
        No* dir = remover_menor(fila, &tamanho_fila); //tira o segundo menor
        No* pai = criar_no(0, esq->frequencia + dir->frequencia, esq, dir); //cria o no pai unindo os dois
        inserir_ordenado(fila, &tamanho_fila, pai); //devolve o pai pra fila
    }
    No* raiz = fila[0]; //o ultimo no que sobra é a raiz da arvore

    FILE* out = fopen(arquivo_saida, "wb"); //abre o arquivo de saida em modo binario
    No* atual = raiz; //ponteiro para navegar na arvore começando do topo
    U8 buffer; //variavel para ler os bytes compactados
    U32 caracteres_decodificados = 0; //contador para ignorar o lixo no final do arquivo

    //le byte a byte enquanto nao atingir o total de caracteres originais
    while (fread(&buffer, 1, 1, in) && caracteres_decodificados < total_caracteres) {
        for (I32 i = 7; i >= 0 && caracteres_decodificados < total_caracteres; i--) { //le do bit mais a esquerda (7) para a direita (0)
            U8 bit = (buffer >> i) & 1; //extrai o valor do bit atual
            atual = bit ? atual->direita : atual->esquerda; //se bit for 1 desce pra direita, se 0 desce pra esquerda

            if (atual->esquerda == NULL && atual->direita == NULL) { //se chegou num nó (achou o caractere)
                fwrite(&(atual->caracter), 1, 1, out); //escreve o caractere traduzido no disco
                caracteres_decodificados++; //registra que mais um caractere foi salvo
                atual = raiz; //volta o ponteiro pro topo da arvore para ler o proximo codigo
            }
        }
    }

    liberar_arvore(raiz); //limpa a memoria dinamica da arvore
    fclose(in); //fecha o arquivo compactado
    fclose(out); //fecha o arquivo novo descompactado
}
