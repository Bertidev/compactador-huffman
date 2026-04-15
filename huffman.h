#ifndef HUFFMAN_H
#define HUFFMAN_H
#include "meustipos.h"
#include "codigo.h"

typedef struct No {
    U8 caracter;
    U32 frequencia;
    struct No *esquerda, *direita;
} No;

void compactar(const char* arquivo_entrada, const char* arquivo_saida);
void descompactar(const char* arquivo_entrada, const char* arquivo_saida);

#endif