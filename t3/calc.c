#include <stdio.h>
#include "str.h"
#include "lista.h"
#include "calc.h"
#include <stdbool.h>

static bool dig_pont(unichar c)
{
    if(c == '.' || (c >= '0' && c <= '9'))
        return true;
    return false;
}

static bool e_espaço(unichar c) 
{
    return c == ' ' || c == '\t' || c == '\n';
}

static bool e_início_ident(unichar c) 
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$';
}

static bool ident(unichar c)
{
    return e_início_ident(c) || c >= '0' && c <= '9';
}

static double soma(Str a, Str b)
{
    return s_número(a) + s_número(b);
}

Lista tokeniza(Str txt)
{
    Lista texto = l_cria();
    int tam = s_tam(txt);
    int inicio;
    int i = 0;

    while(i < tam) {
        unichar c = s_ch(txt, i);

        if(e_espaço(c)) {
            i++;
            continue;
        }
        inicio = i;
        if(dig_pont(c)) {
            while(i < tam && dig_pont(s_ch(txt, i))) {
                i++;
            }
        } else if(e_início_ident(c)) {
            while (i < tam && ident(s_ch(txt, i))) i++;
        } else{
            i++;
        }


        Str s = s_cria("");
        s_substring(s, txt, inicio, i-inicio);
        l_insere_fim(texto, s);
    }

    return texto;
}