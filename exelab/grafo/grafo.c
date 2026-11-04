#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>

Grafo *cria_grafo(int tam)
{
    Grafo *g = malloc(sizeof(Grafo));
    if (g == NULL) {
        printf("Erro de alocação\n");
        exit(1);
    }

    g->qtd_nos = tam;
    g->lista = calloc(tam, sizeof(No *));
    if (g->lista == NULL) {
        printf("Erro de alocação\n");
        free(g);
        exit(1);
    }

    return g;
}

void insere_no(No **lista, int vert)
{
    No *n = malloc(sizeof(No));
    if (n == NULL) {
        printf("Erro de alocação\n");
        exit(1);
    }

    n->vertice = vert;
    n->prox = *lista;
    *lista = n;

}

void adds_arestas(Grafo *g, int a, int b)
{
    if (a < 0 || a >= g->qtd_nos || b < 0 || b >= g->qtd_nos){
        printf("Vertice invalido\n");
        return;
    }

    insere_no(&g->lista[a], b);
    insere_no(&g->lista[b], a);
}