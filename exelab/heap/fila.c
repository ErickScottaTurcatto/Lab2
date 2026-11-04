#include "fila.h"
#include "abb.h"
#include <stdio.h>
#include <stdlib.h>

Fila *criaFila(void)
{
    Fila *f = malloc(sizeof(Fila));
    if (f == NULL) {
        printf("Erro de alocação\n");
        exit(1);
    }

    f->fim = NULL;
    f->ini = NULL;
    f->qtd_total = 0;
    f->quant_nive = 0;

    return f;
}

bool f_vazia(Fila *f) 
{
    if (f->ini == NULL) return true;
    return false;
}

void *f_insere(Fila *f, ABB v)
{
    No *n = malloc(sizeof(No));
    if (n == NULL) {
        printf("Erro de alocação\n");
        exit(1);
    }

    n->dado = v;
    n->prox = NULL;

    if (f->ini == NULL) {
        f->ini = n;
        f->fim = n;
    } else {
        f->fim->prox = n;
        f->fim = n; 
    }

    f->qtd_total++;
}

ABB f_remove(Fila *f)
{
    if (f_vazia(f)) {
        printf("Fila vazia\n");
        exit(1);    
    }

    ABB v = f->ini->dado;

    if (f->ini == f->fim) {
        free(f->ini);
        f->ini = NULL;
        f->fim = NULL;
    } else{
        No *aux = f->ini;
        f->ini = f->ini->prox;
        free(aux);
    }

    f->qtd_total--;
    return v;
}
