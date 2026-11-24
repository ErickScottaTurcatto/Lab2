#include <stdio.h>
#include "str.h"
#include "lista.h"
#include "calc.h"
#include <stdbool.h>
#include <stdlib.h>
#include "dicionario.h"
#include <math.h>

// destrói todas as Str que sobraram dentro de uma lista
static void esvazia(Lista l)
{
    while (l_tam(l) > 0) s_destroi(l_remove_inicio(l));
}

bool Valor_numero(Str l, Dicionário dic, double *out);
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
    [l_pot] =      {O,  O,  O,  E,  E,  O,   E},
    [l_op_parent] ={Er, E,  E,  E,  E,  D,   E},
    [l_igual] =    {O,  E,  E,  E,  E,  O,   E},
};



static bool operacao_igual(Dicionário dic, Lista num, Str v_token, Str n_token)
{
    if (!e_início_ident(s_ch(n_token, 0))) {   
        s_destroi(v_token);
        s_destroi(n_token);
        return false;
    }

    Str valor_Str = s_cria("");
    unichar d = s_ch(v_token, 0);
    if(!e_início_ident(d)){ //resolve problema do negativo
        s_copia(valor_Str, v_token);
    } else {
        valor_t v = dic_busca(dic, v_token);
        if (v == VALOR_NÃO_EXISTE) {
            printf("Variavel indefinida\n");
            s_destroi(valor_Str);
            s_destroi(v_token);
            s_destroi(n_token);
            return false;
        }
        s_copia(valor_Str, (Str)v); 
        
    }
    s_destroi(v_token);

    Str copia_para_dic = s_cria("");
    s_copia(copia_para_dic, valor_Str);

    valor_t antigo = dic_insere(dic, n_token, copia_para_dic);
    if (antigo != VALOR_NÃO_EXISTE) {
        s_destroi((Str)antigo);
        s_destroi(n_token);
    }

    l_empilha(num, valor_Str);
    return true;
}

static bool opera(Lista oper, Lista num, Dicionário dic)
{
    if (l_tam(num) < 2 || l_tam(oper) < 1) return false;

    Str a  = l_desempilha(num); // operando da direita (topo)
    Str b = l_desempilha(num); // operando da esquerda
    Str operador = l_desempilha(oper);
    char c = s_ch(operador, 0);

    if (c == '='){
        bool ok = operacao_igual(dic, num, a, b);
        s_destroi(operador);
        return ok;
    }

    double a1, b1, resultado = 0;
    bool ok = Valor_numero(a, dic, &a1) && Valor_numero(b, dic, &b1);

    if(ok) {
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
                if (a1 == 0) ok = false;
                else resultado = b1 / a1;
                break;
            case '^':
                resultado = pow(b1, a1);
                break;
            default:
                printf("Operador desconhecido: %c\n", c);
                ok = false;
        }
    }

    s_destroi(a);
    s_destroi(b);
    s_destroi(operador);

    if (ok) l_empilha(num, s_cria_número(resultado));
    return ok;
}


static bool acaotomada(acao a, Lista oper, Lista num, Str dado_atual, Dicionário dic)
{
    switch (a) {
        case E: 
            l_empilha(oper, dado_atual);
            return true;

        case O: 
           return opera(oper, num, dic);
        case Er:
            printf("Erro de sintaxe!\n");
            return false;

        case D: { 
            Str abre = l_desempilha(oper);
            s_destroi(abre);
            s_destroi(dado_atual);
            return true;
        }
        case T:
            return true;
    }
    return false;
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
            return N_COLUNAS;
    }
}

Str calculadora(Str expressão, Dicionário dic)
{
    Lista pilha_de_operadores = l_cria();
    Lista pilha_de_operandos = l_cria();
    Lista tokens = tokeniza(expressão);
    Str resultado = NULL;
    Str token_atual = NULL;

    bool fim = false;
    bool erro = false;
    bool precisa_novo_token = true;

    while (!fim && !erro) {
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

        if (dado_at == N_COLUNAS) {      // caractere inválido
            erro = true;
            break;
        }

        acao oqFazer = tabela[dado_da_pilha][dado_at];

        if (oqFazer == T) {
            fim = true;
            continue;
        }

        if (!acaotomada(oqFazer, pilha_de_operadores, pilha_de_operandos, token_atual, dic)) {
            erro = true;
            break;
        }

        precisa_novo_token = (oqFazer == E || oqFazer == D);
    }

     if (erro) {
        // nesses caminhos o token atual não está em nenhuma pilha
        if (token_atual) s_destroi(token_atual);
    } else if (l_tam(pilha_de_operandos) == 1) {
        resultado = l_desempilha(pilha_de_operandos);

        // expressão que é só uma variável: devolve o valor dela
        if (e_início_ident(s_ch(resultado, 0))) {
            valor_t v = dic_busca(dic, resultado);
            s_destroi(resultado);
            resultado = (v == VALOR_NÃO_EXISTE) ? NULL : s_cria_cópia((Str)v);
        }
    }
    

    esvazia(pilha_de_operadores);
    esvazia(pilha_de_operandos);
    esvazia(tokens);
    l_destroi(pilha_de_operadores);
    l_destroi(pilha_de_operandos);
    l_destroi(tokens);

    return resultado ? resultado : s_cria("#ERRO");
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

bool Valor_numero(Str l, Dicionário dic, double *out) 
{
    unichar d =  s_ch(l, 0);

    if (!e_início_ident(d)){ //resolve o problema dele achar que o negativo é variável
        *out =  s_número(l);
        return true;
    }

    valor_t n = dic_busca(dic, l);
    if (n == VALOR_NÃO_EXISTE) {
        printf("Variavel indefinida\n");
        return false;
    }

    *out = s_número((Str)n);
    return true;
}

