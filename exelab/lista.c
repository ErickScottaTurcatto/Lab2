#include "lista.h"
#include <stdlib.h>
#include <assert.h>

#define N 200

struct lista {
  int prim;         // índice do vetor onde está o primeiro dado da lista
  int livre;        // índice do vetor onde está o primeira pos. livre
  dado_t dados[N];  // vetor que contém os dados da lista
  int prox[N];      // vetor que contém os encadeamentos
};

// prox[i] contém o índice do vetor onde está o dado que segue o
//         dado que está no índice i do vetor

// inicializa a lista.
// a memória para a lista já foi alocada.
// a lista fica vazia (prim é -1, dizendo que não tem um índice válido
//   que armazene o primeiro dado da lista)
// todos as posições do vetor são colocadas na lista de livres
static void l_init(Lista l)
{
  l->prim = -1;

  l->livre = 0;
  for (int i = 0; i < N - 1; i++) l->prox[i] = i + 1;
  l->prox[N - 1] = -1;
}

Lista l_cria()
{
  Lista l = malloc(sizeof(struct lista));
  assert(l != NULL);
  l_init(l);
  return l;
}

void l_destroi(Lista l)
{
  free(l);
}

bool l_cheia(Lista l)
{
  return l->livre == -1;
}

bool l_vazia(Lista l)
{
  return l->prim == -1;
}

// encontra um índice livre, e retira da lista de livres
static int ind_livre(Lista l)
{
  // o teste de cheia pode ser l->livre == -1
  assert(!l_cheia(l)); // poderia realocar...
  // pega o índice do primeiro da lista de livres
  int ind = l->livre;
  // o primeiro livre passa a ser o seguinte
  l->livre = l->prox[ind];
  return ind;
}

// insere o dado d no início da lista l
void l_insere_inicio(Lista l, dado_t d)
{
  // acha um índice livre
  int novo = ind_livre(l);
  // coloca o dado no vetor
  l->dados[novo] = d;
  // o dado que segue o novo primeiro dado é o antigo primeiro
  // se a lista estava vazia, l->prim é -1, que é o que queremos como prox
  l->prox[novo] = l->prim;
  // atualiza a nova primeira posição
  l->prim = novo;
}

// insere o dado d no final da lista l
void l_insere_fim(Lista l, dado_t d)
{
  // se a lista estiver vazia, é mais fácil inserir no início...
  if (l_vazia(l)) { l_insere_inicio(l, d); return; }
  // acha um índice livre
  int novo = ind_livre(l);
  // coloca o dado no vetor
  l->dados[novo] = d;
  // ele é o novo último
  l->prox[novo] = -1;
  // acha a posição do último (vai virar penúltimo)
  // sabemos que a lista não está vazia, então l->prim é um índice válido
  // poderia poupar essa busca colocando a posição do último no descritor
  int ult;
  for (ult = l->prim; l->prox[ult] != -1; ult = l->prox[ult])
    /*nada*/;
  // o dado que segue o antigo último é o novo último
  l->prox[ult] = novo;
}

// insere o dado d na lista l, de forma que ele fique na posição p
void l_insere_pos(Lista l, dado_t d, int p)
{
  // se a inserção for no início, temos uma função pronta...
  if (p == 0 || l_vazia(l)) { l_insere_inicio(l, d); return; }
  // acha um índice livre
  int novo = ind_livre(l);
  // coloca o dado no vetor
  l->dados[novo] = d;
  // acha a posição do dado que deve anteceder o novo dado
  // caso a lista não tenha elementos suficientes, acha o último
  int anterior = l->prim;  // índice de um dado no vetor
  int pos_anterior = 0;    // posição na lista correspondente a esse dado
  while (pos_anterior < p - 1) {
    if (l->prox[anterior] == -1) {
      // chegamos no último da lista!
      break;
    }
    anterior = l->prox[anterior];
    pos_anterior++;
  }
  // o índice do dado seguinte
  int seguinte = l->prox[anterior];
  // o dado novo fica após o anterior
  l->prox[anterior] = novo;
  // depois do novo fica o que estava depois do anterior
  l->prox[novo] = seguinte;
}


// retorna o dado no início da lista
dado_t l_dado_inicio(Lista l)
{
  assert(!l_vazia(l));
  return l->dados[l->prim];
}

// retorna o dado no final da lista
dado_t l_dado_fim(Lista l)
{
  assert(!l_vazia(l));

  int ultimo;
  for (int i = l->prim; i != -1; i = l->prox[i]) {
    ultimo = i;
  }

  return l->dados[ultimo];
}

// retorna o dado na posição pos da lista
dado_t l_dado_pos(Lista l, int pos)
{
  int posicao_na_lista = l->prim;
  for (int i = 0; i < pos; i++) {
    posicao_na_lista = l->prox[posicao_na_lista];
  }

  return l->dados[posicao_na_lista];
}

// remove e retorna o dado no início da lista
dado_t l_remove_inicio(Lista l)
{
  return 0;
}

// remove e retorna o dado no final da lista
dado_t l_remove_fim(Lista l)
{
  return 0;
}

// remove e retorna o dado na posição pos da lista
dado_t l_remove_pos(Lista l, int pos)
{
  return 0;
}