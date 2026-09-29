#include <stdio.h>
#include "str.h"
#include "lista.h"
#include "calc.h"
#include <stdbool.h>
#include <stdlib.h>
#include "dicionario.h"
#include <math.h>

double Valor_numero(Str l, Dicionário dic);
Lista tokeniza(Str txt);

bool str_igual(chave_t a, chave_t b) {
    return s_igual((Str)a, (Str)b); 
}

bool str_menor(chave_t a, chave_t b) {
    return false; // não é usada
}

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
    [l_soma_sub] = {O,  O,  E,  E,  E,  O,   E},
    [l_mul_div] =  {O,  O,  O,  E,  E,  O,   E},
    [l_pot] =      {O,  O,  O,  O,  E,  O,   E},
    [l_op_parent] ={Er, E,  E,  E,  E,  D,   E},
    [l_igual] =    {O,  E,  E,  E,  E,  O,   E},
};

static void operacao_igual(Dicionário dic, Lista num, Str v_token, Str n_token)
{
    Str valor_Str = s_cria("");
    unichar d = s_ch(v_token, 0);
    if(dig_pont(d)){
        s_copia(valor_Str, v_token);
    } else {
        valor_t v = dic_busca(dic, v_token);
        if (v == VALOR_NÃO_EXISTE) {
            printf("Variavel indefinida\n");
            exit(1);
        }
        s_copia(valor_Str, (Str)v); 
        s_destroi(v_token);
    }

    unichar e = s_ch(n_token, 0);
    if (dig_pont(e)){
        printf("Erro\n");
        exit(1);
    }

    Str copia_para_dic = s_cria("");
    s_copia(copia_para_dic, valor_Str);

    valor_t antigo = dic_insere(dic, n_token, copia_para_dic);
    if (antigo != VALOR_NÃO_EXISTE) {
        s_destroi((Str)antigo);
        s_destroi(n_token);
    }

    l_empilha(num, valor_Str);
}

static void opera(Lista oper, Lista num, Dicionário dic)
{
    Str a  = l_desempilha(num); // operando da direita (topo)
    Str b = l_desempilha(num); // operando da esquerda
    Str operador = l_desempilha(oper);
    char c = s_ch(operador, 0);

    if (c == '='){
        operacao_igual(dic, num, a, b);
        s_destroi(operador);
        return;
    }

    double a1 = Valor_numero(a, dic);
    double b1 = Valor_numero(b, dic);
    double resultado;

    switch (c) {
        case '+':
            resultado = a1 + b1;
            break;
        case '-':
            resultado = b1 - a1;
            break;
        case '*':
            resultado =  b1 * a1;
            break;
        case '/':
            resultado = b1 /a1;
            break;
        case '^':
            resultado = pow(b1, a1);
            break;
        default:
            printf("Operador desconhecido: %c\n", c);
            exit(1);
    }

    s_destroi(a);
    s_destroi(b);
    s_destroi(operador);

    l_empilha(num, s_cria_número(resultado));
}


static void acaotomada(acao a, Lista oper, Lista num, Str dado_atual, Dicionário dic)
{
    switch (a) {
        case E: 
            l_empilha(oper, dado_atual);
            break;

        case O: 
            opera(oper, num, dic);
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

Str calculadora(Str expressão, Dicionário dic)
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

        acaotomada(oqFazer, pilha_de_operadores, pilha_de_operandos, token_atual, dic);

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

double Valor_numero(Str l, Dicionário dic) 
{
    unichar d =  s_ch(l, 0);

    if (dig_pont(d)){

        return s_número(l);
    }

    valor_t n = dic_busca(dic, l);
    if (n == VALOR_NÃO_EXISTE) {
        printf("Variavel indefinida\n");
        exit(1);
    }

    return s_número((Str)n);
}

