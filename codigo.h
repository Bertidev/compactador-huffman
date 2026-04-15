#ifndef CODIGO_H
#define CODIGO_H
#include "meustipos.h"

typedef struct {
    U8* byte;      //aponta para onde os bits ficam
    U32 capacidade; //limite maximo de bits, ou seja se capacidade = 16 temos 16 bits (e não bytes)
    U32 tamanho;    //tamanho atual em bits 
} Codigo;

boolean novo_codigo(Codigo* c);
void free_codigo(Codigo* c);
boolean adiciona_bit(Codigo* c, U8 valor);
boolean clone_codigo(Codigo original, Codigo* copia);

#endif