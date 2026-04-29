#ifndef HUFFMAN_H
#define HUFFMAN_H
#include "meustipos.h"
#include "codigo.h"

typedef struct No {        //arvore binaria que sera usada
    U8 caracter;           //um byte que representa um caracter
    U32 frequencia;        //quantas vezes o caracter aparece 
    struct No *esquerda, *direita;    //ponteiros para os filhos da arvore
} No;

void compactar(const char* arquivo_entrada, const char* arquivo_saida);
void descompactar(const char* arquivo_entrada, const char* arquivo_saida);

#endif
