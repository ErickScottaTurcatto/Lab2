#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

#define N 4
#define ALOCADO (sizeof(Vertice *) * N)

static void Erro_alocacao(void)
{
    printf("Erro de alocação\n");
    exit(1);
}

Grafo g_cria(int tam_nó, int tam_aresta)
{
    Grafo g = malloc(sizeof(struct grafo));
    if (g == NULL) {
        Erro_alocacao();
    }

    g->n_vertices = 0;
    g->tam_aresta = tam_aresta;
    g->tam_vertice = tam_nó;
    g->consulta_chegam = false;
    g->consulta_a = NULL;

    g->vet_vertices = malloc(ALOCADO);
    if (g->vet_vertices ==  NULL) {
        Erro_alocacao();
    }

    g->tam_alocado_vet = N;

    //inicializar o que falta

    return g;
}

void g_destrói(Grafo self)
{
    if (self == NULL) return;

    Aresta *a;
    for(int i = 0; i < self->n_vertices; i++) {
        a = self->vet_vertices[i]->arestas;
        while(a != NULL) {
            Aresta *p = a->prox;
            free(a->peso);
            free(a);
            a = p;
        }

        free(self->vet_vertices[i]->peso);
        free(self->vet_vertices[i]);
    }

    free(self->vet_vertices);
    free(self);
}

static void verifica_espaco(Grafo g)
{
    if (g->n_vertices >= g->tam_alocado_vet) {
        int espaco_elementos = g->tam_alocado_vet * 2;
        
        Vertice **v = realloc(g->vet_vertices, espaco_elementos * sizeof(Vertice *));

        if (v == NULL) {
            Erro_alocacao();
        }
        g->tam_alocado_vet = espaco_elementos;
        g->vet_vertices = v;
    }
}

int g_insere_nó(Grafo self, void *p_dado)
{
    if (self == NULL) {
        printf("O grafo não existe\n");
        exit(1);
    }

    int n_no = self->n_vertices;

    verifica_espaco(self);

    Vertice *v = malloc(sizeof(Vertice));
    if (v == NULL) {
        Erro_alocacao();
    }

    v->peso = malloc(self->tam_vertice);
    if (v->peso == NULL) {
        Erro_alocacao();
    }

    memcpy(v->peso, p_dado, self->tam_vertice);
    v->arestas = NULL;

    self->vet_vertices[n_no] = v;
    self->n_vertices++;

    return n_no;
}

void g_remove_nó(Grafo self, int nó)
{
    //fazer
}

void g_altera_valor_do_nó(Grafo self, int nó, void *p_dado)
{
    if (self == NULL) {
        printf("O grafo esta vazio\n");
        exit(1);
    }

    if (nó >= self->n_vertices){
        printf("O nó não existe\n");
        exit(1);
    }

    memcpy(self->vet_vertices[nó]->peso, p_dado, self->tam_vertice);
}

void *g_valor_do_nó(Grafo self, int nó)
{
    if (self == NULL) {
        printf("O grafo não existe\n");
        exit(1);
    }

    if (nó >= self->n_vertices){
        printf("O nó não existe\n");
        exit(1);
    }

    return self->vet_vertices[nó]->peso;
}

int g_num_nós(Grafo self)
{
    return self->n_vertices;
}


//Filas

static Aresta *insere_aresta(Aresta *a, void *v, int dest, int tam)
{   
    Aresta *new = malloc(sizeof(Aresta));
    if (new == NULL) Erro_alocacao();

    new->destino = dest;
    new->peso = malloc(tam);
    if (new->peso == NULL) Erro_alocacao();
    memcpy(new->peso, v, tam);

    new->prox = a;

    return new;
}

static Aresta *remove_aresta(Aresta *a, int dest)
{
    if (a == NULL) return NULL;

    Aresta *rem;

    if (a->destino == dest) {
        rem = a;
        a = a->prox;
        free(rem->peso);
        free(rem);

        return a;
    }

    Aresta *aux = a;
    bool achou = false;

    while (aux != NULL && aux->prox != NULL ) {
        if (aux->prox->destino == dest) {
            achou = true;
            break;
        }
        aux = aux->prox;
    }

    if (achou == true) {
        rem = aux->prox;
        aux->prox = aux->prox->prox;
        free(rem->peso);
        free(rem);
    }

    return a;
}

static Aresta *acha_aresta(Grafo self, int origem, int destino){
    if (origem < 0 || origem >= self->n_vertices ||
        destino < 0 || destino >= self->n_vertices) {
        return NULL;
    }
    Aresta *aux = self->vet_vertices[origem]->arestas;

    while(aux != NULL) {
        if (aux->destino == destino){
            break;
        }
        aux = aux->prox;
    }

    return aux;
}

//Arestas

void g_altera_valor_da_aresta(Grafo self, int nó_origem, int nó_destino,
                              void *p_dado)
{
    if (self == NULL){
        printf("O grafo não existe\n");
        exit(1);
    }

    if(nó_origem >= self->n_vertices || nó_destino >= self->n_vertices){
        printf("O nó não existe\n");
        exit(1);
    }

    Aresta *lista = self->vet_vertices[nó_origem]->arestas;

    Aresta *aux = acha_aresta(self,nó_origem, nó_destino);

    if (p_dado != NULL) {
        if (aux != NULL) { //encontrou
            lista = insere_aresta(lista, p_dado, nó_destino, self->tam_aresta);
            self->vet_vertices[nó_origem]->arestas = lista;
        } else {
            memcpy(aux->peso, p_dado, self->tam_aresta);
        }
    } else {
        self->vet_vertices[nó_origem]->arestas = remove_aresta(lista, nó_destino);
    }
}

void *g_valor_da_aresta(Grafo self, int nó_origem, int nó_destino)
{
    if (self == NULL) {
        printf("O grafo não existe\n");
        exit(1);
    }

    Aresta *aux = acha_aresta(self, nó_origem, nó_destino);

    if (aux == NULL) return NULL;

    return aux->peso;
}


void g_arestas_que_partem(Grafo self, int nó_origem)
{
    if (self == NULL){
        printf("O grafo não existe\n");
        exit(1);
    }

    if (nó_origem < 0 || nó_origem >= self->n_vertices) {
        printf("O no não existe\n");
        exit(1);
    }

    self->consulta_chegam = false;
    self->consulta_a = self->vet_vertices[nó_origem]->arestas;
    self->consulta_no = nó_origem;
}

void g_arestas_que_chegam(Grafo self, int nó_destino)
{
    if (self == NULL) {
        printf("O grafo não existe\n");
        exit(1);
    }

    if (nó_destino < 0 || nó_destino >= self->n_vertices) {
        printf("O no não existe\n");
        exit(1);
    }
    int i = 0;
    Aresta *a = NULL;
    while (i < self->n_vertices) {
        a = acha_aresta(self, i, nó_destino);
        if (a != NULL) break;

        i++;
    }

    self->consulta_chegam = true;
    self->consulta_no = nó_destino;
    self->consulta_v = i;
    self->consulta_a = a;
}

void *g_próxima_aresta(Grafo self, int *p_nó_vizinho)
{
    if (self == NULL) {
        printf("O grafo não existe\n");
        exit(1);
    }

    if (self->consulta_a == NULL) return NULL;

    void *retorno = self->consulta_a->peso;
    Aresta *a = NULL;
    
    if (p_nó_vizinho != NULL) {
        if (self->consulta_chegam) 
            *p_nó_vizinho = self->consulta_v;
        else                       
            *p_nó_vizinho = self->consulta_a->destino;
    }

    if (self->consulta_chegam) {
        int i = self->consulta_v + 1;

        while (i < self->n_vertices) {
            a = acha_aresta(self, i, self->consulta_no);
            if (a != NULL) break;
            i++;
        }
        self->consulta_a = a;
        self->consulta_v = i;
    } else {
        self->consulta_a = self->consulta_a->prox;
    }

    return retorno;
}   