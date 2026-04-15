#include <stdio.h>
#include <string.h>
#include "huffman.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <-c|-d> <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        printf("-c : Compactar\n");
        printf("-d : Descompactar\n");
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0) {
        compactar(argv[2], argv[3]);
        printf("Compactação concluída.\n");
    } else if (strcmp(argv[1], "-d") == 0) {
        descompactar(argv[2], argv[3]);
        printf("Descompactação concluída.\n");
    } else {
        printf("Flag inválida.\n");
    }

    return 0;
}