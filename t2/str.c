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
static void desloca(Str s, int tambyte, int pos, Str_c sb, int comeco); //para garantir
// funções auxiliares {{{1

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(Str_c s)
{
  if (s->tam_bytes == 0){
    assert(s->tam_bytes_alocados == 0);
    assert(s->dados == NULL);
    return; 
  }

  assert(s->tam_bytes_alocados >= s->tam_bytes);
  assert(s->tam_bytes_alocados >= MIN_ALLOC);

  if (s->tam_bytes_alocados != MIN_ALLOC)
   assert(s->tam_bytes_alocados <= s->tam_bytes*3);

  //potencia de 2
  int n = s->tam_bytes_alocados;
  while (n != 1) {
    assert(n%2 == 0);
    n = n/2;
  }
}

//...

//minha funçao para deixar os bytes em potencia de 2
static int capacidade(int nbytes)
{
  if (nbytes == 0)
    return 0;

  int cap = MIN_ALLOC;
  while (nbytes > cap) {
    cap = cap * 2;
  }

  return cap;
}


// operações de criação e destruição {{{1

Str s_cria(char const *strC)
{
  int i = 0;
  Str s = malloc(sizeof(*s));
  assert(s != NULL);
  s->dados = NULL;
  s->tam_caracteres = 0;
  s->tam_bytes = 0;
  s->tam_bytes_alocados = 0;
  
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
  free(s->dados);
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
  unsigned char *temp;
  Str s = s_cria("");
  
  FILE *arquivo;

  arquivo = fopen(nome, "r");
  if (arquivo == NULL) {
    printf("Não foi possível abrir o arquivo '%s' para leitura\n", nome);
    return s;
  }
  fseek(arquivo, 0, SEEK_END);
  long tamanho = ftell(arquivo);

  int n = MIN_ALLOC;

  while (n < tamanho) {
    n*=2;
  }

  temp = realloc(s->dados, n);
  if (temp != NULL) {
    s->dados = temp;
    s->tam_bytes_alocados = n;
    s->tam_bytes = tamanho;
  }
  rewind(arquivo); //volta para o inicio

  int car;
  int i = 0;

  while ((car = fgetc(arquivo)) != EOF) {
      s->dados[i] = car;
      i++;
  }

  fclose(arquivo);
  s->tam_caracteres = u8_conta_unichar_nos_bytes(s->tam_bytes, s->dados);
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

  char *p;
  p = (char *) malloc(s->tam_bytes + 1);
  
  for(int i = 0; i < s->tam_bytes; i++) {
    p[i] = s->dados[i];
  }
  
  p[s->tam_bytes] = '\0';

  return p;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);
  byte *codigo;
  unichar endereco;

  if (pos >= s->tam_caracteres) return UNI_INV;

  if(pos < 0) {
    pos = pos + s->tam_caracteres +1;
    if (pos < 0) return UNI_INV;
  }

  codigo = u8_avanca_unichar(s->dados, pos);
  if (codigo == NULL)
    return UNI_INV;


  int a = u8_unichar_nos_bytes(s->tam_bytes - (codigo - s->dados), codigo, &endereco);
  return endereco;
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
  

  if (pos < 0) {
    pos = pos + s->tam_caracteres +1;
    if (pos < 0) return -1;
  }
  if (pos > s->tam_caracteres-1) {
    pos = s->tam_caracteres-1;
  }

  unichar carac;
  byte *endereco;
  int qtdbytes;
  pos--;
  while(pos >= 0) {
    endereco = u8_avanca_unichar(s->dados, pos);
    qtdbytes = u8_unichar_nos_bytes(s->tam_bytes - (endereco - s->dados), endereco, &carac);

    for(int i = 0; i < sb->tam_caracteres; i++) {
      unichar carsb;
      byte *p = u8_avanca_unichar(sb->dados, i);
      int qtdbytessb = u8_unichar_nos_bytes(sb->tam_bytes - (p - sb->dados), p, &carsb);
      
      if (carac == carsb) {
        return pos;
      }
    }
    pos--;
  }
  return -1;
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  
  if (pos < 0) {
    pos = pos + s->tam_caracteres +1;
    if (pos < 0) return -1;
  }
  if (pos > s->tam_caracteres-1) {
    pos = s->tam_caracteres-1;
  }

  unichar carac;
  byte *endereco;
  int qtdbytes;
  pos--;
  while(pos >= 0) {
    endereco = u8_avanca_unichar(s->dados, pos);
    qtdbytes = u8_unichar_nos_bytes(s->tam_bytes - (endereco - s->dados), endereco, &carac);

    bool diferente = false;
    for(int i = 0; i < sb->tam_caracteres; i++) {
      unichar carsb;
      byte *p = u8_avanca_unichar(sb->dados, i);
      int qtdbytessb = u8_unichar_nos_bytes(sb->tam_bytes - (p - sb->dados), p, &carsb);
      
      if (carac == carsb) {
        diferente = true;
      }
  
    }
    if(diferente == false) {
      return pos;
    }
    pos--;
  }
  return -1;
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  if(buscada != NULL)s_ok(buscada);

  if (pos < 0) {
    pos = pos + s->tam_caracteres +1;
    if (pos < 0) return -1;
  }
  if (pos > s->tam_caracteres-1) {
    pos = s->tam_caracteres-1;
  }

  if (buscada == NULL) return pos;

  unichar carac_s;
  byte *end_s;
  int qtdbytes_s;
  int j = 0;
  int posinicio;
  bool igual = true;

  while (pos < s->tam_caracteres) {
    end_s = u8_avanca_unichar(s->dados, pos);
    qtdbytes_s = u8_unichar_nos_bytes(s->tam_bytes - (end_s - s->dados), end_s, &carac_s);

    if (igual == true){
      posinicio = pos;
      igual = false;
    }

    unichar carsb;
    byte *p = u8_avanca_unichar(buscada->dados, j);
    int qtdbytessb = u8_unichar_nos_bytes(buscada->tam_bytes - (p - buscada->dados), p, &carsb);

    if (carac_s == carsb) {
      j++;
      if (j == buscada->tam_caracteres) {
        return posinicio;
      }
      pos++;
    } else {
      pos = posinicio + 1;
      j = 0;
      igual = true;
    }
  }

  return -1;
}


// operações de alteração {{{1

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  s_ok(s);
  if (sb != NULL) s_ok(sb);

  int sb_bytes = (sb != NULL) ? sb->tam_bytes : 0;
  int sb_chars = (sb != NULL) ? sb->tam_caracteres : 0;

  int inicio = pos;
  int fim = pos + tam - 1;

  if (pos < 0){
    inicio = pos + s->tam_caracteres + 1;
    if (inicio < 0) inicio = 0;
  }

  if (inicio > s->tam_caracteres) {
    inicio = s->tam_caracteres;
  }

  int byte_inicio;
  if(inicio == s->tam_caracteres) {
    byte_inicio = s->tam_bytes;
  } else{
    byte_inicio = (int)(u8_avanca_unichar(s->dados, inicio) - s->dados);
  }


  if (fim < pos) {
    int necessario = s->tam_bytes + sb_bytes;
    if (necessario > s->tam_bytes_alocados) {
        int nova_cap = capacidade(necessario);
        byte *d = realloc(s->dados, nova_cap);
        if (d == NULL) exit(1);
        s->dados = d;
        s->tam_bytes_alocados = nova_cap;
    }

    desloca(s, sb_bytes, byte_inicio-1, sb, byte_inicio);

    s->tam_bytes += sb_bytes;
    s->tam_caracteres += sb_chars;

  } else {
    if (fim < 0) {
        fim = fim + s->tam_caracteres + 1;
    }
    if (fim > s->tam_caracteres - 1) {
        fim = s->tam_caracteres - 1;
    }

    byte *cursor;
    int i = inicio;
    int qtdbyte = 0;

    while (i <= fim) {
        cursor = u8_avanca_unichar(s->dados, i);
        qtdbyte += u8_nbytes_no_unichar_que_comeca_com(*cursor);
        i++;
    }

    int bytefim = byte_inicio + qtdbyte;
    int bytesadeslocar = sb_bytes - qtdbyte;

    if (bytesadeslocar > 0) {
        int necessario = s->tam_bytes + bytesadeslocar;
        if (necessario > s->tam_bytes_alocados) {
            int nova_cap = capacidade(necessario);
            byte *d = realloc(s->dados, nova_cap);
            if (d == NULL) exit(1);
            s->dados = d;
            s->tam_bytes_alocados = nova_cap;
        }
    }

    desloca(s, bytesadeslocar, bytefim - 1, sb, byte_inicio);

    s->tam_bytes += bytesadeslocar;
    s->tam_caracteres += sb_chars - (fim - inicio + 1);
  }
}


//pos byte final para deslocar
static void desloca(Str s, int tambyte, int pos, Str_c sb, int comeco)
{
    if (tambyte > 0) {
        int i = s->tam_bytes - 1;
        while (i > pos) {
            s->dados[i + tambyte] = s->dados[i];
            i--;
        }
    } else if (tambyte < 0) {
        int i = pos + 1;
        while (i < s->tam_bytes) {
            s->dados[i + tambyte] = s->dados[i];
            i++;
        }
    }

    int sb_bytes = (sb != NULL) ? sb->tam_bytes : 0;
    for (int j = 0; j < sb_bytes; j++) {          // <-- usa sb_bytes, não sb->tam_bytes
        s->dados[comeco + j] = sb->dados[j];
    }
}

void s_substring(Str s, Str_c sb, int pos, int tam)
{
  
  s_ok(s);
  s_ok(sb);
  int inicio = pos;
  int fim;

  if (pos < 0){
    inicio = pos + sb->tam_caracteres + 1;
    if (inicio < 0) inicio = 0;
  }

  if (inicio > sb->tam_caracteres) {
    inicio = sb->tam_caracteres; //verificar isso
  }

  fim = pos + tam -1;
  if (fim <= 0) {
    fim = fim + sb->tam_caracteres +1;
  }
  if (fim > sb->tam_caracteres - 1) {
    fim = sb->tam_caracteres - 1;
  }
  int byte_inicio;
  if(inicio == sb->tam_caracteres) {
    byte_inicio = sb->tam_bytes;
  } else{
    byte_inicio = (int)(u8_avanca_unichar(sb->dados, inicio) - sb->dados);
  }

  
  byte *cursor;
  int i = inicio;
  int qtdbyte = 0;

  while (i <= fim) {
      cursor = u8_avanca_unichar(sb->dados, i);

      qtdbyte += u8_nbytes_no_unichar_que_comeca_com(*cursor);

      i++;
  }
  
  int bytefim = byte_inicio + qtdbyte;
  //int bytesadeslocar = sb->tam_bytes - qtdbyte;

  free(s->dados);
  int new_capacidade = capacidade(qtdbyte);
  
  byte *d;
  d = malloc(new_capacidade);
  if (d == NULL) {
    exit(1);
  }
  s->dados = d;

  for (int i = 0; i < qtdbyte; i++) {
    s->dados[i] = sb->dados[byte_inicio+i];
  }

  s->tam_bytes = qtdbyte;
  s->tam_caracteres = fim-inicio+1; // ou fim - inicio + 1, calculado corretamente
  s->tam_bytes_alocados = new_capacidade;
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
  
  if (pos < 0) {
    pos =  pos + s->tam_caracteres + 1;
    if (pos < 0) pos = 0;
  }
  if (pos > s->tam_caracteres-1)
    pos = s->tam_caracteres;


  int byte_inicio;
  if(pos == s->tam_caracteres) {
    byte_inicio = s->tam_bytes;
  } else{
    byte_inicio = (int)(u8_avanca_unichar(s->dados, pos) - s->dados);
  }

  byte bytes[4];   
  int qtdByte = u8_converte_pra_utf8(c, bytes);
  int new_cap = qtdByte + s->tam_bytes;

  if (new_cap > s->tam_bytes_alocados) {
    new_cap = capacidade(new_cap);
    byte *d;
    d = realloc(s->dados, new_cap);
    if (d == NULL) exit(1);
    s->dados = d;
    s->tam_bytes_alocados = new_cap;
  }

  if(byte_inicio < s->tam_bytes) {
    int i = s->tam_bytes - 1;
    while (i >= byte_inicio) {
      s->dados[i + qtdByte] = s->dados[i];
      i--;
    }
  }
  
  for (int i = 0; i < qtdByte; i++) {
    s->dados[byte_inicio + i] = bytes[i];
  }

  s->tam_bytes += qtdByte;
  s->tam_caracteres += 1;

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
  //como verificar se tudo for igual? Dara loop infinito


  //isso para saber quantos bytes remover no inicio
  int somador = 0;
  bool igual = true;
  byte *posicao_s = s->dados; 
  byte *posicao_sobras = sobras->dados;  
  unichar caracter_sobras;
  int qtdBytes_sobras;
  int quantidadebytesdel = 0;

  while(igual) {
    unichar caracter_s;
    int qtdBytes_s = u8_unichar_nos_bytes(s->tam_bytes - (posicao_s - s->dados), posicao_s, &caracter_s);

    int i = 0;
    while (i < 1) {
      qtdBytes_sobras = u8_unichar_nos_bytes(sobras->tam_bytes - (posicao_sobras - sobras->dados), posicao_sobras, &caracter_sobras);

      if (caracter_s != caracter_sobras) {
        igual = false;
        break;
      }
      somador++;
      if(somador == sobras->tam_caracteres) {
        quantidadebytesdel += sobras->tam_caracteres;
        posicao_sobras = sobras->dados;
        somador = 0;
      }

      posicao_sobras += qtdBytes_sobras;
      i++;
    }
    posicao_s += qtdBytes_s;
  }

  //-------------------------------------------------

  if (quantidadebytesdel > 0) {
    s_remove(s, 0, quantidadebytesdel);
  }

  somador = 0;
  posicao_s = s->dados; 
  posicao_sobras = sobras->dados;  
  quantidadebytesdel = 0;
  bool endereco = true;
  byte *enderecoinicio;
  int indice = 0;
  int indiceinicio;

  while (indice < s->tam_caracteres) {
    unichar caracter_s;
    int qtdBytes_s = u8_unichar_nos_bytes(s->tam_bytes - (posicao_s - s->dados), posicao_s, &caracter_s);

    
    if (endereco == true) {
      indiceinicio = indice;
      endereco = false;
    }
    int i = 0;
    while (i < 1) {
      qtdBytes_sobras = u8_unichar_nos_bytes(sobras->tam_bytes - (posicao_sobras - sobras->dados), posicao_sobras, &caracter_sobras);
      somador++;
      if (caracter_s != caracter_sobras) {
        endereco = true;
        indiceinicio = s->tam_caracteres;
        somador = 0;
        posicao_sobras = sobras->dados;
      }
      
      else if(somador == sobras->tam_caracteres) {
        quantidadebytesdel += sobras->tam_caracteres;
        posicao_sobras = sobras->dados;
        somador = 0;
      }
      else{
        posicao_sobras += qtdBytes_sobras;
      }
      
      i++;
    }
    posicao_s += qtdBytes_s;
    indice++;
  }

  s_remove(s, indiceinicio, s->tam_caracteres-indiceinicio);

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
  FILE *arquivo;

  arquivo = fopen(nome, "w");
  if (arquivo == NULL) {
    printf("Não foi possível abrir o arquivo 'dados' para escrita.\n");
    exit(1);
  }

  for (int i = 0; i < s->tam_bytes; i++) {
    fputc(s->dados[i], arquivo);
  }
  
  fclose(arquivo);

}


// vim: foldmethod=marker shiftwidth=2

