/*#include <stdio.h>
#include "str.h"
#include "lista.h"
#include "calc.h"
#include "dicionario.h"

// Essas duas já existem no seu calc.c — se elas não forem exportadas
// (static), copie as definições aqui também.
bool str_igual(chave_t a, chave_t b);
bool str_menor(chave_t a, chave_t b);

static void testa(char const *expr_txt, Dicionário dic)
{
    Str expr = s_cria(expr_txt);

    printf("expressao: ");
    s_imprime(expr);
    printf(" -> ");

    Str resultado = calculadora(expr, dic);

    if (resultado != NULL) {
        s_imprime(resultado);
        printf("\n");
    } else {
        printf("(sem resultado)\n");
    }
}

int main(void)
{
    // Ajuste os parâmetros de dic_cria conforme a assinatura real do seu dicionario.h
    Dicionário dic = dic_cria(str_menor, str_igual);

    // precedência básica
    testa("7 + 8 / 5", dic);        // esperado: 14
    testa("2 * 3 + 4", dic);        // esperado: 10
    testa("2 + 3 - 1", dic);        // esperado: 4
    testa("2 ^ 3 * 2", dic);        // esperado: 16

    // parênteses
    testa("(2 + 3) * 4", dic);      // esperado: 20
    testa("2 * (3 + 4)", dic);      // esperado: 14

    // variáveis: atribuição e reuso
    testa("x = 10", dic);           // guarda x = 10
    testa("x + 5", dic);            // esperado: 15
    testa("y = x * 2", dic);        // guarda y = 20
    testa("y - x", dic);            // esperado: 10

    dic_destrói(dic);
    return 0;
}*/