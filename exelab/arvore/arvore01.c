#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct nó nó;
struct nó {
  int dado;
  nó *esq;
  nó *dir;
};

nó *cria_nó(int v, nó *esq, nó *dir)
{
  nó *n = malloc(sizeof(nó));
  assert(n != NULL);
  n->dado = v;
  n->esq = esq;
  n->dir = dir;
}
bool a_vazia(nó *a)
{
  return a == NULL;
}

int n_nós(nó *a)
{
  if (a_vazia(a)) return 0;
  return 1 + n_nós(a->esq) + n_nós(a->dir);
}
  
int altura(nó *a)
{
    if (a_vazia(a)) return -1; 

    int a1 = n_nós(a->esq);
    int a2 = n_nós(a->dir);

    if (a1 > a2) return a1+1;
    else return a2+1;
}
int soma(nó *a) {
  return 0;
}
int maior(nó *a) {
  return 0;
}
void desenha(nó *a, int n)
{
  if (a_vazia(a)) return;
  printf("%*s%d\n", n * 2, "", a->dado);
  desenha(a->esq, n + 1);
  desenha(a->dir, n + 1);	
}
int main()
{
  nó *a = cria_nó(5, cria_nó(4, cria_nó(9, cria_nó(3, NULL, NULL), cria_nó(1, NULL, NULL)), NULL), cria_nó(8, cria_nó(7, NULL, NULL), cria_nó(2, NULL, NULL)));
  desenha(a, 0);
  printf("n_nós: %d\n", n_nós(a));
  printf("altura: %d\n", altura(a));
  printf("soma: %d\n", soma(a));
  printf("maior: %d\n", maior(a));
}
