#include "lista.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct no {
    dado_t dado;
    struct no *ant;
    struct no *prox;
} No;

struct lista {
    No *sentinela;
    int tam;
    No *ultimo;
};

Lista l_cria(void)
{
    Lista l = malloc(sizeof(struct lista));
    if (l == NULL) return NULL;

    l->sentinela = malloc(sizeof(No));
    if (l->sentinela == NULL) {
        free(l);
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
    int tam_total = s_tam(s);
    int tamdasub = 0;
    int pos_retorno = 0;

    while (true) {
        pos_retorno = s_busca_c(s, pos, sep);
        if (pos_retorno == -1) {
            tamdasub = tam_total - pos;
        } else {
            tamdasub = pos_retorno - pos;
        }

        if (tamdasub != 0) {
            Str sub = s_cria("");
            s_substring(sub, s, pos, tamdasub);
            l_insere(l, sub);
        }

        if (pos_retorno == -1) break;
        pos = pos_retorno + 1;
    }

    return l;
}

void l_destroi(Lista l)
{
    if (l == NULL) return;

    No *atual = l->sentinela;
    while (atual != NULL) {
        No *temp = atual;
        atual = atual->prox;
        free(temp);
    }

    free(l);
}

int l_tam(Lista l)
{
    return (l == NULL) ? 0 : l->tam;
}

bool l_cheia(Lista l)
{
    return false; // Lista dinamicamente alocada
}

bool l_vazia(Lista l)
{
    if (l == NULL || l->sentinela == NULL) return true;
    return l->sentinela->prox == NULL;
}

void l_imprime(Lista l)
{
    if (l_vazia(l)) return;

    No *p = l->sentinela->prox;
    while (p != NULL) {
        s_imprime(p->dado);
        printf("\n");
        p = p->prox;
    }
}

void l_insere_inicio(Lista l, dado_t d)
{
    if (l == NULL) return;

    No *n = malloc(sizeof(No));
    if (n == NULL) return;

    n->dado = d;
    n->ant = l->sentinela;
    n->prox = l->sentinela->prox;

    if (l->sentinela->prox != NULL) {
        l->sentinela->prox->ant = n;
    } else {
        l->ultimo = n;
    }

    l->sentinela->prox = n;
    l->tam++;
}

void l_insere_fim(Lista l, dado_t d)
{
    if (l == NULL) return;

    No *n = malloc(sizeof(No));
    if (n == NULL) return;

    n->dado = d;
    n->prox = NULL;
    n->ant = l->ultimo;

    l->ultimo->prox = n;
    l->ultimo = n;
    l->tam++;
}

void l_insere_pos(Lista l, dado_t d, int p)
{
    if (l == NULL || p < 0 || p > l->tam) return;

    if (p == 0) {
        l_insere_inicio(l, d);
        return;
    }
    if (p == l->tam) {
        l_insere_fim(l, d);
        return;
    }

    No *n = malloc(sizeof(No));
    if (n == NULL) return;

    No *pos = l->sentinela->prox;
    for (int i = 0; i < p; i++) {
        pos = pos->prox;
    }

    n->dado = d;
    n->prox = pos;
    n->ant = pos->ant;
    pos->ant->prox = n;
    pos->ant = n;

    l->tam++;
}

dado_t l_dado_inicio(Lista l)
{
    if (l_vazia(l)) return NULL;
    return l->sentinela->prox->dado;
}

dado_t l_dado_fim(Lista l)
{
    if (l_vazia(l)) return NULL;
    return l->ultimo->dado;
}

dado_t l_dado_pos(Lista l, int pos)
{
    if (l_vazia(l) || pos < 0 || pos >= l->tam) return NULL;

    No *p = l->sentinela->prox;
    for (int i = 0; i < pos; i++) {
        p = p->prox;
    }

    return p->dado;
}

dado_t l_remove_inicio(Lista l)
{
    if (l_vazia(l)) return NULL;

    No *rem = l->sentinela->prox;
    dado_t retorno = rem->dado;

    l->sentinela->prox = rem->prox;
    if (rem->prox != NULL) {
        rem->prox->ant = l->sentinela;
    } else {
        l->ultimo = l->sentinela;
    }

    free(rem);
    l->tam--;

    return retorno;
}

dado_t l_remove_fim(Lista l)
{
    if (l_vazia(l)) return NULL;

    No *rem = l->ultimo;
    dado_t valor = rem->dado;

    l->ultimo = rem->ant;
    l->ultimo->prox = NULL;

    free(rem);
    l->tam--;

    return valor;
}

dado_t l_remove_pos(Lista l, int pos)
{
    if (l_vazia(l) || pos < 0 || pos >= l->tam) return NULL;

    if (pos == 0) return l_remove_inicio(l);
    if (pos == l->tam - 1) return l_remove_fim(l);

    No *p = l->sentinela->prox;
    for (int i = 0; i < pos; i++) {
        p = p->prox;
    }

    dado_t valor = p->dado;
    p->ant->prox = p->prox;
    p->prox->ant = p->ant;

    free(p);
    l->tam--;

    return valor;
}

// Funções para usar a lista como fila
dado_t l_primeiro(Lista l) { return l_dado_inicio(l); }
void l_insere(Lista l, dado_t d) { l_insere_fim(l, d); }
dado_t l_remove(Lista l) { return l_remove_inicio(l); }

// Funções para usar a lista como pilha
dado_t l_topo(Lista l) { return l_dado_inicio(l); }
void l_empilha(Lista l, dado_t d) { l_insere_inicio(l, d); }
dado_t l_desempilha(Lista l) { return l_remove_inicio(l); }