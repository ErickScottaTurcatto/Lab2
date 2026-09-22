#include "str.h"
#include <stdio.h>
#include "lista.h"
#include "calc.h"

int main(){
    Str txt = s_cria("7+8/5");
    Lista calc = tokeniza(txt);
    l_imprime(calc);
    printf("\n");

    Str res = s_cria("");
    res = calculadora(txt);
    s_imprime(res);
    printf("\n");
}
