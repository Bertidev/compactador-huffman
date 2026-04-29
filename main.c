#include <stdio.h>
#include <string.h>
#include "huffman.h"

int main(int argc, char *argv[]) { //funcao principal que recebe os argumentos direto da linha de comando do terminal
    if (argc != 4) { //verifica se o usuario passou exatamente 4 argumentos (ex: ./programa -c entrada saida)
        printf("Uso: %s <-c|-d> <arquivo_entrada> <arquivo_saida>\n", argv[0]); //mostra o manual de uso caso o comando esteja incompleto
        printf("-c : Compactar\n");
        printf("-d : Descompactar\n");
        return 1; //encerra o programa com codigo de erro (1)
    }

    if (strcmp(argv[1], "-c") == 0) { //compara a string do primeiro argumento para verificar se a flag é de compactar
        compactar(argv[2], argv[3]); //chama a funcao passando os nomes dos arquivos de entrada e saida
        printf("Compactação concluída.\n");
    } else if (strcmp(argv[1], "-d") == 0) { //compara a string para verificar se a flag é de descompactar
        descompactar(argv[2], argv[3]); //executa a descompactacao
        printf("Descompactação concluída.\n");
    } else {
        printf("Flag inválida.\n"); //trata o erro caso o usuario digite uma flag inexistente 
    }

    return 0; //encerra o programa com sucesso (0)
}
