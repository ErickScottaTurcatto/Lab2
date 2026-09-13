#include "str.h"
#include <stdio.h>
#include "lista.h"


int main()
{
    Str s;
    s = s_cria_número(98.49841984);
    s_imprime(s);
    printf("\n");

    Str a = s_cria("a,ba,ca, te");
    Str b = s_cria(", ");

    Lista l = l_cria();
    l = l_cria_separando(a, b);

    l_imprime(l);
    printf("\n");

    Str um = s_cria("d");
    Lista umc = l_cria();
    l_insere(umc, um);

    s = s_cria_unindo(umc, b);
    s_imprime(s);
    printf("\n");
}