#include "str.h"
#include <stdio.h>
#include "lista.h"
#include "calc.h"


int main()
{
    Str s;
    s = s_cria_número(98.49);
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

    //s = s_cria_unindo(umc, b);
    s_imprime(s);
    printf("\n");

    double decimal = s_número(s);
    printf("%f\n", decimal);


    
    Str calc = s_cria("92+a ba 3b3 ** *  " );
    Lista caltest = tokeniza(calc);
    l_imprime(caltest);
    printf("\n");
    printf("qtd de tokens: %d\n", l_tam(caltest));
}