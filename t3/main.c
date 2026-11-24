#include "calc.h"
#include "dicionario.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main()
{
    Dicionário dic = dic_cria(str_menor, str_igual);
    char linha[1024];

    while (fgets(linha, sizeof linha, stdin) != NULL) {
        if (linha[strspn(linha, " \t\r\n")] == '\0') continue;

        Str expr = s_cria(linha);
        Str res = calculadora(expr, dic);
        s_imprime(res);
        putchar('\n');
        s_destroi(res);
        s_destroi(expr);
    }

    dic_destrói(dic);
    return 0;
}