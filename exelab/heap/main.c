#include <stdint.h>
#include <stdbool.h>
#include "abb.h"
#include "heap.h"
#include "fila.h"
#include <stdio.h>

static bool menor(chave_t a, chave_t b) { return (intptr_t)a < (intptr_t)b; }
static bool igual(chave_t a, chave_t b) { return (intptr_t)a == (intptr_t)b; }

#define INS(n) a = abb_insere(a, (chave_t)(intptr_t)(n), (valor_t)(intptr_t)(n), NULL, menor, igual)

int main(void)
{
  ABB a = abb_cria();

  INS(8);  INS(4);  INS(12);
  INS(2);  INS(6);  INS(10); INS(14);
  INS(1);  INS(3);  INS(5);  INS(7);
  INS(9);  INS(11); INS(13); INS(15);

  imprime_por_nivel(a);

  Heap *h = cria_heap(8);
  converte_trie_heap(h, a);
  imprimr_ordem_linear(h);

  Heap *new = cria_heap(4);
  inserir_heap(new, 4);
  inserir_heap(new, 6);
  inserir_heap(new, 3);
  inserir_heap(new, 11);
  inserir_heap(new, 8);
  inserir_heap(new, 2);
  inserir_heap(new, 1);
  inserir_heap(new, 9);
  inserir_heap(new, 12);
  inserir_heap(new, 10);
  inserir_heap(new, 5);
  inserir_heap(new, 13);
  inserir_heap(new, 14);
  inserir_heap(new, 7);
  inserir_heap(new, 15);
  
  imprimr_ordem_linear(new);
}