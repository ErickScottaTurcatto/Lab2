#include "abb.h"

struct heap {
    int *dado;
    int qtd_dados;
    int tam_alocado;
};
typedef struct heap Heap;

Heap *cria_heap(int tam);
void verifica_realoca_heap(Heap *h);
void inserir_heap(Heap *h, int dado);
void converte_trie_heap(Heap *h, ABB a);
void imprimr_ordem_linear(Heap *h);
void heap_corrige_subindo(Heap *h);
int remover_heap(Heap *h);
