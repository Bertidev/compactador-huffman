#include <stdlib.h>
#include "codigo.h"

boolean novo_codigo(Codigo* c) {
    c->byte = (U8*)malloc(sizeof(U8));
    if (c->byte == NULL) return FALSE;
    c->byte[0] = 0;
    c->capacidade = 8;
    c->tamanho = 0;
    return TRUE;
}

void free_codigo(Codigo* c) {
    if (c->byte != NULL) {
        free(c->byte);
        c->byte = NULL;
    }
    c->capacidade = 0;
    c->tamanho = 0;
}

boolean adiciona_bit(Codigo* c, U8 valor) {
    if (c->tamanho == c->capacidade) {
        U32 nova_cap_bytes = (c->capacidade / 8) + 1;
        U8* novo = (U8*)realloc(c->byte, nova_cap_bytes * sizeof(U8));
        if (novo == NULL) return FALSE;
        c->byte = novo;
        c->byte[nova_cap_bytes - 1] = 0;
        c->capacidade += 8;
    }
    
    U32 byte_idx = c->tamanho / 8;
    U32 bit_idx = 7 - (c->tamanho % 8); 
    
    if (valor == 1) {
        c->byte[byte_idx] |= (1 << bit_idx);
    } else {
        c->byte[byte_idx] &= ~(1 << bit_idx);
    }
    
    c->tamanho++;
    return TRUE;
}

boolean clone_codigo(Codigo original, Codigo* copia) {
    U32 bytes_necessarios = (original.capacidade / 8) > 0 ? (original.capacidade / 8) : 1;
    copia->byte = (U8*)malloc(bytes_necessarios * sizeof(U8));
    if (copia->byte == NULL) return FALSE;
    
    for (U32 i = 0; i < bytes_necessarios; i++) {
        copia->byte[i] = original.byte[i];
    }
    copia->capacidade = original.capacidade;
    copia->tamanho = original.tamanho;
    return TRUE;
}