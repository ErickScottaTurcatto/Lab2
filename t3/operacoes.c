#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "str.h"
#include "operacoes.h"

double soma(Str a, Str b)
{
    double x, y;
    char *sa = s_strc(a);
    char *sb = s_strc(b);

    sscanf(sa, "%lf", &x);
    sscanf(sb, "%lf", &y);

    free(sa);
    free(sb);

    return x + y;
}

double subtracao(Str a, Str b)
{
    double x, y;
    char *sa = s_strc(a);
    char *sb = s_strc(b);

    sscanf(sa, "%lf", &x);
    sscanf(sb, "%lf", &y);

    free(sa);
    free(sb);

    return x - y;
}

double multiplicacao(Str a, Str b)
{
    double x, y;
    char *sa = s_strc(a);
    char *sb = s_strc(b);

    sscanf(sa, "%lf", &x);
    sscanf(sb, "%lf", &y);

    free(sa);
    free(sb);

    return x * y;
}

double divisao(Str a, Str b)
{
    double x, y;
    char *sa = s_strc(a);
    char *sb = s_strc(b);

    sscanf(sa, "%lf", &x);
    sscanf(sb, "%lf", &y);

    free(sa);
    free(sb);

    return x / y;
}

double potencia(Str a, Str b)
{
    double x, y;
    char *sa = s_strc(a);
    char *sb = s_strc(b);

    sscanf(sa, "%lf", &x);
    sscanf(sb, "%lf", &y);

    free(sa);
    free(sb);

    return pow(x, y);
}