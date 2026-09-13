#include "lista.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct no{
    dado_t dado;
    struct no *ant;
    struct no *prox;
} No;

struct lista{
    No *sentinela;
    int tam;
    No *ultimo;
};

Lista l_cria(void)
{
    Lista l = malloc(sizeof(struct lista));
    l->sentinela = malloc(sizeof(No));
    if (l == NULL || l->sentinela == NULL) {
        printf("Erro de alocacao\n");
        return NULL;
    }
    l->sentinela->ant = NULL;
    l->sentinela->prox = NULL;
    l->tam = 0;
    l->ultimo = l->sentinela;

    return l;
}

Lista l_cria_separando(Str s, Str sep)
{
    Lista l = l_cria();
    int pos = 0;
    int tam_total= s_tam(s);
    int tamdasub = 0;
    int pos_retorno = 0;

    //tratar strings vazias

    while(true) {
        pos_retorno = s_busca_c(s, pos, sep);
        if(pos_retorno == -1) {
            tamdasub = tam_total - pos;//se der algum bug é aqui
        } else
            tamdasub = pos_retorno - pos;

        if (tamdasub != 0) {
            Str sub = s_cria("");
            s_substring(sub, s, pos, tamdasub);
            l_insere(l, sub);
        } 
        
        if(pos_retorno == -1) break;
        pos = pos_retorno+1;
    }

    return l;
}

void l_destroi(Lista l)
{
    No *atual = l->sentinela;

    while(atual != NULL) {
        No *temp = atual;
        atual = atual->prox;

        free(temp);
    }

    free(l);
}

int l_tam(Lista l)
{
    return l->tam;
}

bool l_cheia(Lista l)
{
    if  (l->sentinela->prox == NULL)
        return false;
    return true;
}

bool l_vazia(Lista l)
{
    if(l->sentinela->prox == NULL)
        return true;
    return false;
}

void l_imprime(Lista l)
{
    No *p = l->sentinela->prox;

    if(l_vazia(l)) return;

    while(p != NULL) {
        s_imprime(p->dado);
        printf("\n");
        p = p->prox;
    }
}

void l_insere_inicio(Lista l, dado_t d)
{ 
    No *n = malloc(sizeof(No));
    if(l->sentinela->prox == NULL)
        l->ultimo = n;
    n->dado = d;
    n->ant = l->sentinela;
    n->prox = l->sentinela->prox;
    l->sentinela->prox = n;

    l->tam++;
}

void l_insere_fim(Lista l, dado_t d)
{
    No *n = malloc(sizeof(No));
    n->dado = d;
    n->prox = NULL;
    l->ultimo->prox = n;
    n->ant = l->ultimo;

    l->tam++;
    l->ultimo = n;
}

void l_insere_pos(Lista l, dado_t d, int p)
{
    if (p > l->tam-1) return;
    
    No *n = malloc(sizeof(No));
    if(n == NULL){
        printf("Erro de alocação\n");
        return;
    }

    No *pos = l->sentinela->prox;

    for(int i = 0; i != p; i++){
        pos = pos->prox;
    }
    n->dado = d;
    pos->ant->prox = n;
    pos->ant = n;
    n->ant = pos->ant;
    n->prox = pos;

    l->tam++;
}

dado_t l_dado_inicio(Lista l)
{
    if(l_vazia(l)) return NULL;

    return l->sentinela->prox->dado;
}

dado_t l_dado_fim(Lista l)
{
    if(l_vazia(l)) return NULL;

    return l->ultimo->dado;
}

dado_t l_dado_pos(Lista l, int pos)
{
    if(l_vazia(l)) return NULL;
    if(pos > l->tam-1) return NULL;

    No *p = l->sentinela->prox;

    for(int i = 0; i < pos; i++) {
        p = p->prox;
    }

    return p->dado;
}

dado_t l_remove_inicio(Lista l)
{
    No *rem = l->sentinela->prox;
    dado_t retorno = rem->dado;
    l->sentinela->prox = rem->prox;
    rem->prox->ant = l->sentinela;
    free(rem);
    l->tam--;

    if(l->tam <= 1) l->ultimo = l->sentinela->prox;

    return retorno;
}

dado_t l_remove_fim(Lista l)
{
    if(l_vazia(l)) return NULL;

    l->ultimo->ant->prox = l->ultimo->prox;
    dado_t valor = l->ultimo->dado;
    No *rem = l->ultimo;
    l->ultimo = l->ultimo->ant;
    free(rem);
    l->tam--;

    return valor;
}

dado_t l_remove_pos(Lista l, int pos)
{
    if(l_vazia(l)) return NULL;
    if(pos > l->tam-1) return NULL;
    if (pos == l->tam-1)
        return l_remove_fim(l);
    No *p = l->sentinela->prox;

    for(int i = 0; i < pos; i++) 
        p = p->prox;

    dado_t valor = p->dado;
    No *rem = p;
    p->ant->prox = p->prox;
    p->prox->ant = p->ant;
    free(rem);
    l->tam--;

    return valor;
}

//funções para usar a lista como uma fila

dado_t l_primeiro(Lista l)
{
    return l_dado_inicio(l);
}

void l_insere(Lista l, dado_t d)
{
    l_insere_fim(l, d);
}

dado_t l_remove(Lista l)
{
    return l_remove_inicio(l);
}

//funções para usar a lista como pilha

dado_t l_topo(Lista l)
{
    return l_dado_inicio(l);
}

void l_empilha(Lista l, dado_t d)
{
    l_insere_inicio(l, d);
}

dado_t l_desempilha(Lista l)
{
    return l_remove_inicio(l);
}

