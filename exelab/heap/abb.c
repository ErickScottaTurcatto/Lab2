#include "abb.h"
#include "fila.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// declaração do registro que contém um nó da árvore


// funções auxiliares (não exportadas aos clientes)

// aloca e retorna um nó folha com 'chave' e 'valor'
static ABB abb_cria_folha(chave_t chave, valor_t valor)
{
  ABB nova = malloc(sizeof(*nova));
  assert(nova != NULL);
  nova->chave = chave;
  nova->valor = valor;
  nova->esq = NULL;
  nova->dir = NULL;
  return nova;
}

static bool abb_é_folha(ABB a)
{
  return abb_é_vazia(a->esq) && abb_é_vazia(a->dir);
}

// libera a memória usada pelo nó 'a'
static void abb_destrói_nó(ABB a)
{
  // se algum dado fosse alocado, seria liberado aqui
  free(a);
}

// implementação das funções da interface
ABB abb_cria()
{
  return NULL; // a árvore vazia é representada por NULL
}

void abb_destrói(ABB a)
{
  if (abb_é_vazia(a)) return;
  abb_destrói(a->esq);
  abb_destrói(a->dir);
  abb_destrói_nó(a);
}

bool abb_é_vazia(ABB a)
{
  return a == NULL;
}


valor_t abb_busca(ABB a, chave_t chave, comparador_t menor, comparador_t igual)
{
  if (abb_é_vazia(a)) return VALOR_NÃO_EXISTE;

  if(igual(chave, a->chave)) return a->valor;

  if(menor(a->chave, chave)) 
    return abb_busca(a->dir, chave, menor, igual);
  else 
    return abb_busca(a->esq, chave, menor, igual);
}

ABB abb_insere(ABB a, chave_t chave, valor_t valor, valor_t *panterior, comparador_t menor, comparador_t igual)
{
  if(abb_é_vazia(a)) {
    if (panterior != NULL) *panterior = VALOR_NÃO_EXISTE;
    a = abb_cria_folha(chave, valor);
  } else if (igual(a->chave,chave)) {
    if (panterior != NULL) *panterior = a->valor;
    a->valor = valor;
  } else {
    if(menor(a->chave, chave)) {
      a->dir = abb_insere(a->dir, chave, valor, panterior, menor, igual);
    }
    else {
      a->esq = abb_insere(a->esq, chave, valor, panterior, menor, igual);
    }
  }
  return a;
}

ABB abb_remove(ABB a, chave_t chave, valor_t *pvalor, comparador_t menor, comparador_t igual)
{
  if (a == NULL) {
    if (pvalor != NULL) *pvalor = VALOR_NÃO_EXISTE;
    return NULL;
  }

  if (igual(a->chave, chave)) {
    if (pvalor != NULL) *pvalor = a->valor;

    if(a->dir == NULL && a->esq == NULL) {
      free(a);
      return NULL;
    } else {
      if (a->dir != NULL && a->esq != NULL) {
        ABB aux = a->esq;

        while(aux->dir != NULL) {
          aux = aux->dir;
        }

        a->chave = aux->chave;
        a->valor = aux->valor;

         valor_t descarte;
        a->esq = abb_remove(a->esq, a->chave, &descarte, menor, igual);
        return a;
      } else {
        ABB aux;
        if (a->esq != NULL) {
          aux =  a->esq;
        } else{
          aux = a->dir;
        }
        free(a);
        return aux;
      }
    }
  } else{
    if (menor(a->chave, chave)) {
      a->dir = abb_remove (a->dir, chave, pvalor, menor, igual);
    } else{
      a->esq = abb_remove (a->esq, chave, pvalor, menor, igual);
    }
    return a;
  }
}

void imprime_por_nivel(ABB a)
{
  if(abb_é_vazia(a)) {
    printf("Arvore vazia\n");
    exit(1);
  }

  Fila *f = criaFila();

  f_insere(f, a);

  while(f->qtd_total > 0) {
    for (int i = 0; i < f->qtd_total; i++) {
      ABB n = f_remove(f);
      printf("%d ", n->valor);

      if (n->esq != NULL) f_insere(f, n->esq);
      if (n->dir != NULL) f_insere(f, n->dir);
    }
  }

  printf("\n");
}
