#include "fila.h"
#include "abb.h"
#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include <stdint.h>

Heap *cria_heap(int tam)
{
    Heap *h = malloc(sizeof(Heap));
    if (h == NULL) {
        printf("Erro de alocação\n");
        exit(1);
    }

    int qtd_alocada = tam * sizeof(int);
    h->dado = malloc(qtd_alocada);
    if (h->dado == NULL) {
        printf("Erro de alocação\n");
        free(h);
        exit(1);
    }

    h->qtd_dados = 0;
    h->tam_alocado = tam;

    return h;
}

void verifica_realoca_heap(Heap *h)
{
    if (h->tam_alocado == h->qtd_dados) {
        int novo_tam = h->tam_alocado * 2;
      
        int *temp = realloc(h->dado, novo_tam * sizeof(int));
        if (temp == NULL) {
            printf("Erro de alocação\n");
            exit(1);
        }
        h->dado = temp;
        h->tam_alocado = novo_tam;
    }
}

void inserir_heap(Heap *h, int dado)
{
    verifica_realoca_heap(h);
    h->dado[h->qtd_dados] = dado;
    h->qtd_dados++;

    heap_corrige_subindo(h);
}

void heap_corrige_subindo(Heap *h)
{
    int pos = h->qtd_dados-1;
    while(true) {
        int pai = (pos-1)/2;
        
        if (h->dado[pai] >= h->dado[pos])
            return; 

        if(h->dado[pai] < h->dado[pos]) {
            int aux = h->dado[pai];
            h->dado[pai] = h->dado[pos];
            h->dado[pos] = aux;

            pos = pai;
        }
    }
}

void converte_trie_heap(Heap *h, ABB a)
{
    if(abb_é_vazia(a)) {
    printf("Arvore vazia\n");
    exit(1);
  }

  Fila *f = criaFila();

  f_insere(f, a);

  while(f->qtd_total > 0) {
    int qtd_nivel = f->qtd_total;
    for (int i = 0; i < qtd_nivel; i++) {
      ABB n = f_remove(f);

      inserir_heap(h, (int)(intptr_t)n->valor); //o ponteiro armazena o numero e não uma variavel int com o numero. Por isso, intptr_t vai fazer a conversão para int
  
      if (n->esq != NULL) f_insere(f, n->esq);
      if (n->dir != NULL) f_insere(f, n->dir);
    }
  }
}

void imprimr_ordem_linear(Heap *h)
{
    if (h->qtd_dados == 0){
        printf("Nada a imprimir\n");
        return;
    }

    for(int i = 0; i < h->qtd_dados; i++) {
        printf("%d ", h->dado[i]);
    }
    printf("\n");
}


int remover_heap(Heap *h)
{
    if (h->qtd_dados == 0) {
        printf("Heap vazio\n");
        exit(1);
    }

    int rem = h->dado[0];
    h->qtd_dados--;
    h->dado[0] = h->dado[h->qtd_dados];

    int i = 0;
    while (true) {
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        int maior = i;

        if (esq < h->qtd_dados && h->dado[esq] > h->dado[maior])
            maior = esq;
        if (dir < h->qtd_dados && h->dado[dir] > h->dado[maior])
            maior = dir;

        if (maior == i)
            break;

        int aux = h->dado[i];
        h->dado[i] = h->dado[maior];
        h->dado[maior] = aux;

        i = maior;
    }

    return rem;
}

