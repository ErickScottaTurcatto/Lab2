#include "auxiliar.h"

#include <math.h>
#include <stdlib.h>

// usa rand48 em vez de rand ou random, para ter valores aleatórios
//   independentes para o treinamento e para o jogo

//vantagens de usa drand48:
//-ha mais estados possiveis
//-ela por padrao gera em um intervalo entre 0.0 e 1.0, o que é util em RNs
float al01()
{
  return drand48();
  // return (random() % 1000000) / (float)1000000;
}

//da um retorno de um numero aleatorio que segue a distribuição normal
float gauss(float média, float desvio)
{
  float pi = acosf(-1);
  float x, y, z;
  x = al01();
  y = al01();
  z = sqrtf(-2 * logf(x)) * cosf(2 * pi * y);
  return média + desvio * z;
}

//vai gerar numeros ate 2³¹ - 1 e %limite transforma esse numeros para um 
//intervalo de [0; limite-1]
int aleatório(int limite)
{
  return lrand48() % limite;
  // return random() % limite;
}

//gera um numero aleatorio que segue a distribuição normal e depois transforma
//ele em um valor dentro do limite desejado.
//EX: se meu limite é 5, então queremos um valor entre [0;5], então pegamos
//o valor que segue a distribuição normal que estará no intervalo [-1; 1] 
//e multiplicar pelo limite; 0.35*5 = 1.75 
int aleatório_p(int limite)
{
  if (limite <= 1) return 0;
  float alp01;
  do {
    alp01 = fabsf(gauss(0, 1));
  } while (alp01 >= 1);
  return alp01 * limite;
}

int aleatório_entre(int min, int max)
{
  return min + aleatório(max - min);
}

int max(int a, int b)
{
  if (a > b) return a;
  return b;
}

bool probabilidade(int percentual)
{
  return aleatório(100) < percentual;
}
