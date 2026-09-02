// teste_str.c
// Programa de testes completo para o TAD Str, cobrindo todas as funções da interface str.h

#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main()
{
  
  printf("--- TESTES ORIGINAIS DA ENTRADA ---\n");

  char *a = "Rábica";
  Str s = s_cria(a);
  printf("Deve escrever [%s] ", a);
  s_imprime(s);
  printf("\n");

  Str ss = s_cria_substring(s, 3, 2);
  printf("Deve escrever [ic] ");
  s_imprime(ss);
  printf("\n");

  s_substring(ss, s, 1, 2);
  printf("Deve escrever [áb] ");
  s_imprime(ss);
  printf("\n");

  if (s_tam(ss) != 2) printf("tamanho de 'áb' (%d) deveria ser 2\n", s_tam(ss));

  s_substitui(ss, -2, 1, NULL);
  s_substitui(s, -7, 3, ss);
  printf("Deve escrever [áica] ");
  s_imprime(s);
  printf("\n");

  s_destroi(ss);
  s_destroi(s);

  printf("\n");
  printf("--- 1. CRIAÇÃO E DESTRUIÇÃO ---\n");

  // Testando s_cria_cópia
  Str s_orig = s_cria("Computação");
  Str s_copia_str = s_cria_cópia(s_orig);
  printf("s_cria_cópia [Computação]: ");
  s_imprime(s_copia_str);
  printf("\n");

  // Testando s_grava_arquivo e s_cria_de_arquivo
  char *nome_arq = "teste_tmp_str.txt";
  s_grava_arquivo(s_orig, nome_arq);
  Str s_arq = s_cria_de_arquivo(nome_arq);
  printf("s_cria_de_arquivo (lido do disco) [Computação]: ");
  s_imprime(s_arq);
  printf("\n");

  s_destroi(s_orig);
  s_destroi(s_copia_str);
  s_destroi(s_arq);

  printf("\n");
  printf("--- 2. OPERAÇÕES DE ACESSO ---\n");

  Str s_acesso = s_cria("Olá UTF-8!");
  printf("s_tam de 'Olá UTF-8!' (esperado 10): %d\n", s_tam(s_acesso));

  // Testando s_strc
  char *str_c = s_strc(s_acesso);
  printf("s_strc gerou string C: \"%s\"\n", str_c);
  free(str_c); // Responsabilidade do chamador conforme especificado em str.h

  // Testando s_ch
  unichar ch_p0 = s_ch(s_acesso, 0);   // 'O'
  unichar ch_p1 = s_ch(s_acesso, 1);   // 'l'
  unichar ch_p2 = s_ch(s_acesso, 2);   // 'á'
  printf("s_ch pos 0, 1, 2 (esperado 'O', 'l', 'á'): '%c', '%c', codepoint U+%04X\n", (char)ch_p0, (char)ch_p1, ch_p2);

  s_destroi(s_acesso);

  printf("\n");
  printf("--- 3. BUSCA E COMPARAÇÃO ---\n");

  Str s_base = s_cria("abracadabra");
  Str s_outra = s_cria("abracadabra");
  Str s_diff = s_cria("algoritmo");

  // Testando s_igual
  printf("s_igual (iguais) (esperado true/1): %d\n", s_igual(s_base, s_outra));
  printf("s_igual (diferentes) (esperado false/0): %d\n", s_igual(s_base, s_diff));

  // Testando buscas por conjunto
  Str s_conj1 = s_cria("cd");
  printf("s_busca_c em 'abracadabra' por [cd] na pos 0 (esperado 4 ['c']): %d\n", s_busca_c(s_base, 0, s_conj1));

  Str s_conj2 = s_cria("ab");
  printf("s_busca_nc em 'abracadabra' por não-[ab] na pos 0 (esperado 2 ['r']): %d\n", s_busca_nc(s_base, 0, s_conj2));

  printf("s_busca_rc em 'abracadabra' por [cd] antes da pos 10 (esperado 6 ['d']): %d\n", s_busca_rc(s_base, 10, s_conj1));
  printf("s_busca_rnc em 'abracadabra' por não-[a] antes da pos 11 (esperado 9 ['r']): %d\n", s_busca_rnc(s_base, 11, s_conj2));

  // Testando s_busca_s
  Str s_sub = s_cria("cad");
  printf("s_busca_s 'cad' em 'abracadabra' a partir da pos 0 (esperado 4): %d\n", s_busca_s(s_base, 0, s_sub));

  s_destroi(s_base);
  s_destroi(s_outra);
  s_destroi(s_diff);
  s_destroi(s_conj1);
  s_destroi(s_conj2);
  s_destroi(s_sub);

  printf("\n");
  printf("--- 4. OPERAÇÕES DE ALTERAÇÃO ---\n");

  Str s_alt = s_cria("abácaxi");
  Str s_te = s_cria("te");

  // Testando s_substitui
  s_substitui(s_alt, 5, 2, s_te);
  printf("s_substitui pos 5, tam 2 por 'te' (esperado [abácate]): ");
  s_imprime(s_alt);
  printf("\n");

  // Testando s_copia
  Str s_copia_alvo = s_cria("");
  s_copia(s_copia_alvo, s_alt);
  printf("s_copia (esperado [abácate]): ");
  s_imprime(s_copia_alvo);
  printf("\n");

  // Testando s_insere e s_insere_c
  Str s_ins = s_cria("!!! ");
  s_insere(s_alt, 0, s_ins);
  printf("s_insere na pos 0 (esperado [!!! abácate]): ");
  s_imprime(s_alt);
  printf("\n");

  s_insere_c(s_alt, -1, '!');
  printf("s_insere_c '!' na pos -1 (esperado [!!! abácate!]): ");
  s_imprime(s_alt);
  printf("\n");

  // Testando s_anexa e s_anexa_c
  Str s_anx = s_cria(" OK");
  s_anexa(s_alt, s_anx);
  printf("s_anexa ' OK' (esperado [!!! abácate! OK]): ");
  s_imprime(s_alt);
  printf("\n");

  s_anexa_c(s_alt, '?');
  printf("s_anexa_c '?' (esperado [!!! abácate! OK?]): ");
  s_imprime(s_alt);
  printf("\n");

  // Testando s_remove
  s_remove(s_alt, 0, 4);
  printf("s_remove pos 0 tam 4 (esperado [abácate! OK?]): ");
  s_imprime(s_alt);
  printf("\n");

  // Testando s_apara
  Str s_sobras = s_cria(" ?!");
  s_apara(s_alt, s_sobras);
  printf("s_apara com ' ?!' (esperado [abácate! OK]): ");
  s_imprime(s_alt);
  printf("\n");

  s_destroi(s_alt);
  s_destroi(s_te);
  s_destroi(s_copia_alvo);
  s_destroi(s_ins);
  s_destroi(s_anx);
  s_destroi(s_sobras);

  printf("\n");
  printf("--- TODOS OS TESTES CONCLUÍDOS ---\n");

  printf("\n");
  char *abc = "abácaxi";
  Str sabc= s_cria(abc);
  s_imprime(sabc);
  printf("\n");
  Str sa_sobras = s_cria("");
  //s_apara(sabc, sa_sobras);
  s_substitui(sabc, 2, 200, sa_sobras);
  s_imprime(sabc);
  printf("\n");

  s_grava_arquivo(sabc, "arquivo.txt");

  Str narq = s_cria_de_arquivo("arquivo.txt");
  s_imprime(narq);
  
  printf("\n");
  
  s_destroi(sabc);
  s_destroi(sa_sobras);
  s_destroi(narq);

  return 0;
}