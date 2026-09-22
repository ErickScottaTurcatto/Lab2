#include <stdio.h>
#include "str.h"
#include "lista.h"
#include "calc.h"
#include <stdbool.h>
#include "operacoes.h"
#include <stdlib.h>


Lista tokeniza(Str txt);

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
    return e_início_ident(c) || (c >= '0' && c <= '9');
}

static bool e_operador(Str st)
{
    char s = s_ch(st, 0);
    return s == '=' || s == '+' || s == '*' || s == '-' || s == '/' || s == '^' || s == '(' || s == ')'; 
}

typedef enum {
    l_v,
    l_soma_sub,
    l_mul_div,
    l_pot,
    l_op_parent,
    l_igual,
    N_LINHAS
} op_pilha;

typedef enum {
    c_f,
    c_soma_sub,
    c_mul_div,
    c_pot,
    c_op_parent,
    c_f_parent,
    c_igual,
    N_COLUNAS
} op_atual;

typedef enum {E, O, Er, D, T} acao;

acao tabela[N_LINHAS][N_COLUNAS] = {
    //col          F   +-  */   ^   (    )  =
    [l_v] =        {T,  E,  E,  E,  E,  Er,  E},
    [l_soma_sub] = {O,  O,  E,  E,  E,  O,   O},
    [l_mul_div] =  {O,  O,  O,  E,  E,  O,   O},
    [l_pot] =      {O,  O,  O,  O,  E,  O,   O},
    [l_op_parent] ={Er, E,  E,  E,  E,  D,   Er},
    [l_igual] =    {O,  E,  E,  E,  E,  O,   O},
};

static void opera(Lista oper, Lista num)
{
    double numero;
    Str a  = l_desempilha(num); // operando da direita (topo)
    Str b = l_desempilha(num); // operando da esquerda
    Str operador = l_desempilha(oper);
    char c = s_ch(operador, 0);

    switch (c) {
        case '+':
            numero = soma(b, a);
            break;
        case '-':
            numero = subtracao(b, a);
            break;
        case '*':
            numero = multiplicacao(b, a);
            break;
        case '/':
            numero = divisao(b, a);
            break;
        case '^':
            numero = potencia(b, a);
            break;
        default:
            printf("Operador desconhecido: %c\n", c);
            exit(1);
    }

    s_destroi(a);
    s_destroi(b);
    s_destroi(operador);

    l_empilha(num, s_cria_número(numero));
}

static void acaotomada(acao a, Lista oper, Lista num, Str dado_atual)
{
    switch (a) {
        case E: 
            l_empilha(oper, dado_atual);
            break;

        case O: 
            opera(oper, num);
            break;

        case Er:
            printf("Erro de sintaxe!\n");
            exit(1);

        case D: { 
            Str abre = l_desempilha(oper);
            s_destroi(abre);
            s_destroi(dado_atual);
            break;
        }

        case T:
            break;
    }
}

op_pilha classifica_op_pilha(char c)
{
    switch(c){
        case 'V':
            return l_v;
        case '+': case '-': 
           return l_soma_sub;
        case '*': case '/': 
            return l_mul_div;
        case '^': 
            return l_pot;
        case '(':
            return l_op_parent;
        case '=':
            return l_igual;
        default:
            return l_v;
    }
}

op_atual classifica_op_atual(char c)
{
    switch(c){
        case 'F':
            return c_f;
        case '+': case '-': 
           return c_soma_sub;
        case '*': case '/': 
            return c_mul_div;
        case '^': 
            return c_pot;
        case '(':
            return c_op_parent;
        case ')':
            return c_f_parent;
        case '=':
            return c_igual;
        default:
            printf("Caractere/Operador invalido: '%c'\n", c);
            exit(1);
    }
}

Str calculadora(Str expressão)
{
    Lista pilha_de_operadores = l_cria();
    Lista pilha_de_operandos = l_cria();
    Lista tokens = tokeniza(expressão);
    Str resultado = NULL;

    bool fim = false;
    Str token_atual = NULL;
    bool precisa_novo_token = true;

    while (!fim) {
        if (precisa_novo_token) {
            token_atual = (l_tam(tokens) > 0) ? l_remove_inicio(tokens) : NULL;
        }

        if (token_atual != NULL && (dig_pont(s_ch(token_atual, 0)) || e_início_ident(s_ch(token_atual, 0)))) {
            l_empilha(pilha_de_operandos, token_atual);
            precisa_novo_token = true;
            continue;
        }

        Str v_na_pilha = (l_tam(pilha_de_operadores) > 0) ? l_topo(pilha_de_operadores) : NULL;
        
        op_pilha dado_da_pilha = (v_na_pilha == NULL)
            ? classifica_op_pilha('V')
            : classifica_op_pilha(s_ch(v_na_pilha, 0));

        op_atual dado_at = (token_atual == NULL)
            ? classifica_op_atual('F')
            : classifica_op_atual(s_ch(token_atual, 0));

        acao oqFazer = tabela[dado_da_pilha][dado_at];

        if (oqFazer == T) {
            fim = true;
            continue;
        }

        acaotomada(oqFazer, pilha_de_operadores, pilha_de_operandos, token_atual);

        precisa_novo_token = (oqFazer == E || oqFazer == D);
    }

    if (l_tam(pilha_de_operandos) > 0) {
        resultado = l_desempilha(pilha_de_operandos);
    }

    l_destroi(pilha_de_operadores);
    l_destroi(pilha_de_operandos);
    l_destroi(tokens);

    return resultado;
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