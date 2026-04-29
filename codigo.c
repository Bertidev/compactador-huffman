#include <stdlib.h>
#include "codigo.h"

boolean novo_codigo(Codigo* c) {    //aloca 1 byte 
    c->byte = (U8*)malloc(sizeof(U8));
    if (c->byte == NULL) return FALSE;    //verifica erro de memória
    c->byte[0] = 0;                       //zero o primeiro byte 
    c->capacidade = 8;                    //capacidade (em bits)
    c->tamanho = 0;                       //tamanho inicial 0
    return TRUE;
}
//após criar um código teremos
//00000000

void free_codigo(Codigo* c) {    //libera a memória
    if (c->byte != NULL) {
        free(c->byte);
        c->byte = NULL;
    }
    c->capacidade = 0;           //zera os metadados 
    c->tamanho = 0;
}

boolean adiciona_bit(Codigo* c, U8 valor) {
    if (c->tamanho == c->capacidade) {            //se capacidade = tamanho 
        U32 nova_cap_bytes = (c->capacidade / 8) + 1;     //aumenta a capacidade em 1 byte, se antes eram 8 bits, agora vão ser 16
        U8* novo = (U8*)realloc(c->byte, nova_cap_bytes * sizeof(U8));    //realloc pode expandir o vetor de bytes no lugar ou mover para outro endereço
        if (novo == NULL) return FALSE;        //se falhar 
        c->byte = novo;                        //atualiza o ponteiro
        c->byte[nova_cap_bytes - 1] = 0;       //zera o novo byte
        c->capacidade += 8;                    //aumenta a capacidade em 1 byte 
    }
    
    U32 byte_idx = c->tamanho / 8;            //indica qual em qual byte do vetor vai escrever 
    U32 bit_idx = 7 - (c->tamanho % 8);       //posição do bit dentro do byte
    
    if (valor == 1) {                           
        c->byte[byte_idx] |= (1 << bit_idx);     //se o valordo bit =1 faz um OR
    } else {
        c->byte[byte_idx] &= ~(1 << bit_idx);    //se valor=0 faz um NOT e um AND
    }
    
    c->tamanho++;    //atualiza o tamanho 
    return TRUE;
}

boolean clone_codigo(Codigo original, Codigo* copia) {
    U32 bytes_necessarios = (original.capacidade / 8) > 0 ? (original.capacidade / 8) : 1;    //calcula qunatos bytes copiar
    copia->byte = (U8*)malloc(bytes_necessarios * sizeof(U8));    //aloca um novo bloco de memória do mesmo tamanho do original
    if (copia->byte == NULL) return FALSE;        
    
    for (U32 i = 0; i < bytes_necessarios; i++) {    //clona os valores, bit a bit
        copia->byte[i] = original.byte[i];
    }
    copia->capacidade = original.capacidade;    //clona capacidade e tamanho
    copia->tamanho = original.tamanho;
    return TRUE;
}
