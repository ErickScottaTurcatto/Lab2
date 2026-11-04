#include "abb.h"

struct no{
    ABB dado;
    struct no *prox;
};
typedef struct no No;

struct fila{
    int qtd_total;
    int quant_nive;
    No *ini;
    No *fim;
};
typedef struct fila Fila;


Fila *criaFila(void);
bool f_vazia(Fila *f);
void *f_insere(Fila *f, ABB v);
ABB f_remove(Fila *f);