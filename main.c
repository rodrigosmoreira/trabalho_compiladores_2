/*
 * Alunos:
 * Alexandre Saura.
 * Fábio Thomaz.
 * Ricardo Oliveira.
 * Rodrigo Suarez.
 *
 * Trabalho de Compiladores I - UNISANTOS - março de 2021.
 * Prof.: José Fontebasso Neto.
 */
#include <stdlib.h> // free
#include <stdio.h>  // printf

#include "util.h"             // read_string
#include "token.h"            // check_tokens
#include "analyzer.h" // analyse

int main()
{
    printf("Exemplo de expressão: <3><.><2><+><7>\n");
    printf("Insira sua expressão: ");

    char *expression;
    expression = read_string();

    printf("\n");

    if (!check_tokens(expression))
        return 1;
    if (!analyse(expression))
    {
        printf("\nEntrada inválida!\n");
        free(expression);
        return 1;
    }
    else {
        printf("\nEntrada válida!\n");
    }

    free(expression);
    return 0;
}
