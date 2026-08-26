// includes, constantes e declarações {{{1
#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8    // alocação mínima

struct str {
  byte *dados;
  int tam_caracteres;
  int tam_bytes;
  int tam_bytes_alocados;
};

// A memória para conter os bytes de uma string deve ser alocada e/ou
//   realocada conforme a necessidade, cuidando para que a quantidade
//   de memória alocada seja sempre:
//   - nula (não alocada) se a string for vazia, ou
//   - não inferior ao necessário para armazenar os bytes da codificação utf8;
//   - não inferior à alocação mínima;
//   - não superior ao triplo do número de bytes necessários
//     (exceto quando for o mínimo);
//   - uma potência de 2.

// funções auxiliares {{{1

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(Str_c s)
{
  
}

//...

static int capacidade(int nbytes)
{
  if (nbytes == 0)
    return 0;

  int cap = MIN_ALLOC;
  while (nbytes > cap) {
    cap = cap * 2;
  }
}


// operações de criação e destruição {{{1

Str s_cria(char const *strC)
{
  int i = 0;
  Str s = malloc(sizeof(*s));
  assert(s != NULL);
  
  if (strC != NULL) {
    int tam_bytes = 0;
    while (strC[tam_bytes] != '\0') {
      tam_bytes++;
    }

    if (tam_bytes > 0) {
      int tam_caracteres = u8_conta_unichar_nos_bytes(tam_bytes, (byte *) strC);

      if (tam_caracteres >= 0) {
        int tam_bytes_alocados = capacidade(tam_bytes);
        s->dados = malloc(tam_bytes_alocados);

        for (int i = 0; i < tam_bytes; i++) {
          s->dados[i] = strC[i];
        }
        s->tam_bytes = tam_bytes;
        s->tam_bytes_alocados = tam_bytes_alocados;
        s->tam_caracteres = tam_caracteres;
      }
    }
  }

  return s;
}

void s_destroi(Str s)
{
  s_ok(s);
  //...
  free(s);
}

Str s_cria_substring(Str_c s, int pos, int tam)
{
   Str nova = s_cria("");
   s_substring(nova, s, pos, tam);
   return nova;
}

Str s_cria_cópia(Str_c s)
{
   return s_cria_substring(s, 0, -1);
}

// Retorna uma nova string com o conteúdo do arquivo chamado nome.
// Retorna uma string vazia em caso de erro.
Str s_cria_de_arquivo(char *nome)
{
  Str s = s_cria("");
  //...
  return s;
}

// operações de acesso {{{1

int s_tam(Str_c s)
{
  s_ok(s);
  return s->tam_caracteres;
}

char *s_strc(Str_c s)
{
  s_ok(s);
  //...
  return NULL;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);
  //...
  return UNI_INV;
}


// operações de busca e comparação {{{1

bool s_igual(Str_c s, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  
  if (s->tam_bytes != sb->tam_bytes || s->tam_caracteres != sb->tam_caracteres) {
    return false;
  }

  for (int i = 0; i < s->tam_bytes; i++) {
    if (s->dados[i] != sb->dados[i])
      return false;
  }

  return true;
}

int s_busca_c(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  
  if (pos < 0) {
    pos = pos + s->tam_caracteres + 1;
  }

  byte *posicao = u8_avanca_unichar(s->dados, pos);
  if (posicao == NULL) return -1;

  while (posicao < s->dados + s->tam_bytes) {
    unichar uni_s;
    int nb = u8_unichar_nos_bytes(s->dados + s->tam_bytes - posicao, posicao, &uni_s);
    if (nb < 0) return -1;

    byte *posicao_sb = sb->dados;
    bool achou = false;

    while (posicao_sb < sb->dados + sb->tam_bytes) {
      unichar uni_sb;
      int nsb = u8_unichar_nos_bytes(sb->dados + sb->tam_bytes - posicao_sb, posicao_sb, &uni_sb);
      if (nsb < 0) return -1;

      if (uni_s == uni_sb) {
        achou = true;
        break;
      }

      posicao_sb += nsb;
    }

    if (achou)
      return pos;

    posicao += nb;
    pos++;
  }

  return -1;
}

int s_busca_nc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  
  if (pos < 0) { // tratar posições invalidas, como por exemplo -10, max o maximo é -7
    pos = pos + s->tam_caracteres + 1;
  }

  byte *posicao = u8_avanca_unichar(s->dados, pos);
  if (posicao == NULL) return -1;

  while (posicao < s->dados + s->tam_bytes) {
    unichar uni_s;
    int nb = u8_unichar_nos_bytes(s->dados + s->tam_bytes - posicao, posicao, &uni_s);
    if (nb < 0) return -1;

    byte *posicao_sb = sb->dados;
    bool igual = false;

    while (posicao_sb < sb->dados + sb->tam_bytes) {
      unichar uni_sb;
      int nsb = u8_unichar_nos_bytes(sb->dados + sb->tam_bytes - posicao_sb, posicao_sb, &uni_sb);
      if (nsb < 0) return -1;

      if (uni_s == uni_sb) {
        igual = true;
        break;
      }

      posicao_sb += nsb;
    }

    if (!igual)
      return pos;
    
    posicao += nb;
    pos++;
  }

  return -1;
}

int s_busca_rc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  //...
  return -1;
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  //...
  return -1;
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  s_ok(buscada);
  //...
  return -1;
}


// operações de alteração {{{1

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  //...
}

void s_substring(Str s, Str_c sb, int pos, int tam)
{
  /*int inicio, i = 0, indice = 0;
  int fim = 0;
  int alocar = 0;
  s_ok(s);
  s_ok(sb);

  if (pos < 0) {
    pos = pos + sb->tam_caracteres + 1;
    if (pos < 0)
      inicio = 0;
  } else if (pos >= sb->tam_caracteres && tam >= 0) {
    s = ""; // verificar isso
  } else {
    inicio = pos;
  }

  byte *endereco_inicio;
  endereco_inicio =  u8_avanca_unichar(sb->dados[i], inicio);

  while (fim != tam) {
    int nb = u8_nbytes_no_unichar_que_comeca_com(endereco_inicio);
    alocar +=nb;
    endereco_inicio = endereco_inicio + nb;
    fim++;
  }*/

}

void s_copia(Str s, Str_c sb)
{
  s_substring(s, sb, 0, -1);
}

void s_insere(Str s, int pos, Str_c sb)
{
  s_substitui(s, pos, 0, sb);
}

void s_insere_c(Str s, int pos, unichar c)
{
  s_ok(s);
  //...
}

void s_anexa(Str s, Str_c sb)
{
  s_substitui(s, -1, 0, sb);
}

void s_anexa_c(Str s, unichar c)
{
  s_insere_c(s, -1, c);
}

void s_remove(Str s, int pos, int tam)
{
  s_substitui(s, pos, tam, NULL);
}

void s_apara(Str s, Str_c sobras)
{
  s_ok(s);
  s_ok(sobras);
  //...
}

// operações de E/S {{{1

void s_imprime(Str_c s)
{
  s_ok(s);

  for (int i = 0; i < s->tam_bytes; i++) {
    putchar(s->dados[i]);
  }
}

void s_grava_arquivo(Str_c s, char *nome)
{
  s_ok(s);
  //...
}


// vim: foldmethod=marker shiftwidth=2

