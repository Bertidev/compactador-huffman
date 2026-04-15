#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

// Cria um novo nó da árvore
No* criar_no(U8 caracter, U32 frequencia, No* esquerda, No* direita) {
    No* no = (No*)malloc(sizeof(No));
    no->caracter = caracter;
    no->frequencia = frequencia;
    no->esquerda = esquerda;
    no->direita = direita;
    return no;
}

// Fila de prioridade simples (inserção ordenada)
void inserir_ordenado(No** fila, U32* tamanho, No* no) {
    U32 i = *tamanho;
    while (i > 0 && fila[i - 1]->frequencia < no->frequencia) {
        fila[i] = fila[i - 1];
        i--;
    }
    fila[i] = no;
    (*tamanho)++;
}

No* remover_menor(No** fila, U32* tamanho) {
    (*tamanho)--;
    return fila[*tamanho];
}

// Gera os códigos recursivamente navegando na árvore
void gerar_codigos(No* raiz, Codigo codigo_atual, Codigo* tabela) {
    if (raiz == NULL) return;

    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        clone_codigo(codigo_atual, &tabela[raiz->caracter]);
        return;
    }

    Codigo esq_cod, dir_cod;
    clone_codigo(codigo_atual, &esq_cod);
    adiciona_bit(&esq_cod, 0);
    gerar_codigos(raiz->esquerda, esq_cod, tabela);
    free_codigo(&esq_cod);

    clone_codigo(codigo_atual, &dir_cod);
    adiciona_bit(&dir_cod, 1);
    gerar_codigos(raiz->direita, dir_cod, tabela);
    free_codigo(&dir_cod);
}

void liberar_arvore(No* raiz) {
    if (raiz == NULL) return;
    liberar_arvore(raiz->esquerda);
    liberar_arvore(raiz->direita);
    free(raiz);
}

void compactar(const char* arquivo_entrada, const char* arquivo_saida) {
    FILE* in = fopen(arquivo_entrada, "rb");
    if (!in) { printf("Erro ao abrir entrada.\n"); return; }

    U32 frequencias[256] = {0};
    U8 buffer;
    while (fread(&buffer, 1, 1, in)) {
        frequencias[buffer]++;
    }

    No* fila[256];
    U32 tamanho_fila = 0;
    for (I32 i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            inserir_ordenado(fila, &tamanho_fila, criar_no(i, frequencias[i], NULL, NULL));
        }
    }

    while (tamanho_fila > 1) {
        No* esq = remover_menor(fila, &tamanho_fila);
        No* dir = remover_menor(fila, &tamanho_fila);
        No* pai = criar_no(0, esq->frequencia + dir->frequencia, esq, dir);
        inserir_ordenado(fila, &tamanho_fila, pai);
    }
    No* raiz = fila[0];

    Codigo tabela[256];
    for (I32 i = 0; i < 256; i++) tabela[i].byte = NULL;
    
    Codigo cod_base;
    novo_codigo(&cod_base);
    gerar_codigos(raiz, cod_base, tabela);
    free_codigo(&cod_base);

    FILE* out = fopen(arquivo_saida, "wb");
    fwrite(frequencias, sizeof(U32), 256, out); // Cabeçalho necessário para descompactar

    fseek(in, 0, SEEK_SET);
    U8 byte_saida = 0;
    U32 bits_escritos = 0;

    while (fread(&buffer, 1, 1, in)) {
        Codigo c = tabela[buffer];
        for (U32 i = 0; i < c.tamanho; i++) {
            U8 bit = (c.byte[i / 8] >> (7 - (i % 8))) & 1;
            if (bit) byte_saida |= (1 << (7 - bits_escritos));
            bits_escritos++;

            if (bits_escritos == 8) {
                fwrite(&byte_saida, 1, 1, out);
                byte_saida = 0;
                bits_escritos = 0;
            }
        }
    }
    if (bits_escritos > 0) fwrite(&byte_saida, 1, 1, out);

    for (I32 i = 0; i < 256; i++) free_codigo(&tabela[i]);
    liberar_arvore(raiz);
    fclose(in);
    fclose(out);
}

void descompactar(const char* arquivo_entrada, const char* arquivo_saida) {
    FILE* in = fopen(arquivo_entrada, "rb");
    if (!in) { printf("Erro ao abrir entrada.\n"); return; }

    U32 frequencias[256];
    fread(frequencias, sizeof(U32), 256, in);

    No* fila[256];
    U32 tamanho_fila = 0;
    U32 total_caracteres = 0;

    for (I32 i = 0; i < 256; i++) {
        if (frequencias[i] > 0) {
            inserir_ordenado(fila, &tamanho_fila, criar_no(i, frequencias[i], NULL, NULL));
            total_caracteres += frequencias[i];
        }
    }

    if (tamanho_fila == 0) { fclose(in); return; }

    while (tamanho_fila > 1) {
        No* esq = remover_menor(fila, &tamanho_fila);
        No* dir = remover_menor(fila, &tamanho_fila);
        No* pai = criar_no(0, esq->frequencia + dir->frequencia, esq, dir);
        inserir_ordenado(fila, &tamanho_fila, pai);
    }
    No* raiz = fila[0];

    FILE* out = fopen(arquivo_saida, "wb");
    No* atual = raiz;
    U8 buffer;
    U32 caracteres_decodificados = 0;

    while (fread(&buffer, 1, 1, in) && caracteres_decodificados < total_caracteres) {
        for (I32 i = 7; i >= 0 && caracteres_decodificados < total_caracteres; i--) {
            U8 bit = (buffer >> i) & 1;
            atual = bit ? atual->direita : atual->esquerda;

            if (atual->esquerda == NULL && atual->direita == NULL) {
                fwrite(&(atual->caracter), 1, 1, out);
                caracteres_decodificados++;
                atual = raiz;
            }
        }
    }

    liberar_arvore(raiz);
    fclose(in);
    fclose(out);
}