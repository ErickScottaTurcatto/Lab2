#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "fila.h"
#include <string.h>

struct no{
    void *valor;
    struct no *prox;
    struct no *ant;
};
typedef struct no No;

struct fila{
    No *prim;
    No *ult;
    No *pos_atual;
    int sentido;
    int tam_dado;
    int tam_fila;
};


Fila f_cria(int tam_do_dado)
{
    Fila f = (Fila) malloc(sizeof(struct fila));
    assert(!(f == NULL));

    f->prim = NULL;
    f->ult = NULL;
    f->pos_atual = NULL;
    f->sentido = 0;
    f->tam_dado = tam_do_dado;
    f->tam_fila = 0;

    return f;
}

void f_destrói(Fila self)
{
    No *p = self->prim;
    No *rem;
    while(p != NULL) {
        rem = p;
        p = p->prox;
        free(rem->valor);
        free(rem);
    }

    free(self);
}

bool f_tá_vazia(Fila self)
{
    if (self->prim == NULL) return true;
    return false;
}

void f_remove(Fila self, void *pdado)
{
    if(f_tá_vazia(self)){
        printf("vazia\n");
        return;
    }

    No *rem = self->prim;
    if (pdado != NULL) memcpy(pdado, rem->valor, self->tam_dado);

    if (self->prim == self->ult){
        self->prim = NULL;
        self->ult = NULL;
    } else{
        self->prim->prox->ant = NULL;
        self->prim = self->prim->prox;
    }

    if(self->prim == NULL){
        self->ult = NULL;
    }

    self->tam_fila--;
    free(rem->valor);
    free(rem);
}

void f_insere(Fila self, void *pdado)
{
    No *f = (No *) malloc(sizeof(No));
    assert(!(f == NULL));

    f->valor = malloc(self->tam_dado);
    assert(f->valor != NULL);
    memcpy(f->valor, pdado, self->tam_dado);

    f->prox = NULL;

    if (self->ult == NULL) {
        self->prim = f;
        self->ult = f;
        f->ant = NULL;
    } else {
        f->ant = self->ult;
        self->ult->prox = f;
        self->ult = f;
    }

    self->tam_fila++;
}

void f_inicia_percurso(Fila self, int pos_inicial)
{
    self->pos_atual = NULL;
    self->sentido = 0;

    if (f_tá_vazia(self)) {
        return;
    }

    No *p = self->prim;
    int pos;
    if(pos_inicial < 0){
        pos = self->tam_fila + pos_inicial; 
        self->sentido = -1;
    } else{
        pos = pos_inicial;
        self->sentido = 1;
    }

    int i = 0;
    while(i < pos){
        p = p->prox;
        i++;
    }

    self->pos_atual = p;

}


bool f_próximo(Fila self, void *pdado)
{
     if (self->pos_atual == NULL) {
        return false;
    }

    if (pdado != NULL) {
        memcpy(pdado, self->pos_atual->valor, self->tam_dado);
    }

    if (self->sentido < 0){
        self->pos_atual = self->pos_atual->ant;
    } else {
        self->pos_atual = self->pos_atual->prox;
    }

    return true;
}




