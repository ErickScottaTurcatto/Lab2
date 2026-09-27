#include "str.h"
#include <stdio.h>
#include "lista.h"
#include "calc.h"

int main(){
    Dicionário dic = dic_cria(str_menor, str_igual);
    Str txt = s_cria("7+8 / 5");
    Lista calc = tokeniza(txt);
    l_imprime(calc);
    printf("\n");

    Str res = s_cria("");
    res = calculadora(txt, dic);
    s_imprime(res);
    printf("\n");



    Str calca = s_cria("92+a ba 3b3 ** *  " );
    Lista caltest = tokeniza(calca);
    l_imprime(caltest);
    printf("\n");
    printf("qtd de tokens: %d\n", l_tam(caltest));
}
