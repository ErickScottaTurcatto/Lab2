
// teste_str.c
// programa com testes do TAD str

#include "str.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
  char *a = "abacaxi";
  char *b = "barco";
  Str s = s_cria(a);
  Str sb = s_cria(b);
  printf("Deve escrever [%s] ", a);
  s_imprime(s);
  printf("\n");
  printf("Deve escrever [%s] ", b);
  s_imprime(sb);
  printf("\n");
  //teste da função s_igual
  bool igual = s_igual(s, sb);
  printf("%s\n", igual ? "true" : "false");
  //teste da função s_busca_c
  int res = s_busca_c(s, 2, sb);
  printf("%d\n", res);
  //teste da função s_busca_c
  int buscanc= s_busca_nc(s, 3, sb);
  printf("%d\n", buscanc);
  //teste funcao s_grava_arquivo
  s_grava_arquivo(s, "arquivo.txt");
  //teste funcao s_cria_de_arquivo
  Str c = s_cria_de_arquivo("arquivo.txt");
  s_imprime(c);
  printf("\n");


  //teste da funçao s_strc
  char *str;
  str = s_strc(s);
  printf("%s\n", str);
  free(str);

  //teste funcao unichar s_ch

  unichar ab = s_ch(s, 0);
  printf("%u\n", ab);
  printf("%c\n", ab); //vai funcionar ate os caracteres ASCII

  //teste substitui
  //s_substitui(s, 2, 200, sb);
  //s_imprime(s);
  printf("\n");

  //teste s_substring
  //s_substring(s, sb, 2, -1);
  //s_imprime(s);
  printf("\n");

  //teste s_copia
  //s_copia(s, sb);
  //s_imprime(s);
  printf("\n");

  //teste s_insere
  //s_insere(s, 3, sb);
  //s_imprime(s);
  printf("\n");
 
  //teste s_insere_c
  s_imprime(s);
  printf("\n");
  char *crt = "Z";
  Str scrt = s_cria(crt);
  unichar car = s_ch(scrt, 0);
  s_insere_c(s, 5, car);
  s_imprime(s);
  printf("\n");
  

  Str ss = s_cria_substring(s, 3, 2);
  printf("\nDeve escrever [ic] ");
  s_imprime(ss);
  s_substring(ss, s, 1, 2);
  printf("\nDeve escrever [áb] ");
  s_imprime(ss);
  if (s_tam(ss) != 2) printf("\ntamanho de 'áb' (%d) deveria ser 2\n", s_tam(ss));
  s_substitui(ss, -2, 1, NULL);
  s_substitui(s, -7, 3, ss);
  printf("Deve escrever [Rica] ");
  s_imprime(s);
  s_destroi(ss);
  s_destroi(s);
  
}
