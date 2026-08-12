#define _POSIX_C_SOURCE 199309L // linha adds por conta de bug no CLOCK_MONOTONIC causado provavelmente pelo wsl
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

//#define armas_dia[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'n'};
//#define armas_noite[] = {'0', '2', '4', '6', '8', 'n'};

//troca de armas pode ser feita usando o codigo ascii 

typedef struct {
    enum {
        dia,
        noite,
        fim
    } estados;
    int pontuacao;
    int municao;
    bool encerrar;
    bool fim_partida;
    bool fim_onda;
    bool proxima_onda;
    bool saida_esc;
    bool gameover;
    int arma_atual;
    int inimigo_inativos;
    int inimigos_vivos;
    int onda;
    int escudos;
    double tempo;
} Dados_jogo;

typedef struct timespec crono;

void configura_terminal();
char lechar();
void crono_inicia(crono *c);
double crono_parcial(crono *c);
void inicializacao (Dados_jogo *dados);
void joga_partida (Dados_jogo *dadosjogo);
void joga_onda (Dados_jogo *dadosjogo);
char inimigos_random();
void inicia_mapa (char *mapa, Dados_jogo *dadosjogo);
void atualiza_mapa (char *mapa, Dados_jogo *dadosjogo);
void desenha_jogo(char *mapa, Dados_jogo *dadosjogo);
void troca_arma(Dados_jogo *dadosjogo, int tecla);
void atira(Dados_jogo *dadosjogo, char *mapa, int tecla);
void sair(Dados_jogo *dadosjogo, int tecla);
void newonda(Dados_jogo *dadosjogo, int tecla);
void proxima_onda(Dados_jogo *dadosjogo);
void pontuacao_mortes(Dados_jogo *dadosjogo, int indice, bool n); 
void pontuacao_itens(Dados_jogo *dadosjogo);
void gameover(Dados_jogo *dadosjogo);

int main()
{
    configura_terminal();
    Dados_jogo dadosjogo;
    srand(time(NULL));

    inicializacao (&dadosjogo);
    
    while (!(dadosjogo.encerrar)) {
        joga_partida(&dadosjogo);
        if (dadosjogo.gameover)
            gameover(&dadosjogo);
    }

    

    system("stty sane");
    //se jogar novamente fazer => dadosjogo.estados = dia;
}







void configura_terminal()
{
    if (system("stty raw opost -echo min 0 time 1") != 0) {
        perror("erro na execução de system(\"stty\")");
        fprintf(stderr, "você tem o programa stty instalado?\n");
        exit(1);
    };
    if (setvbuf(stdin, NULL, _IONBF, 0) != 0) {
        perror("erro na execução de setvbuf()");
        exit(1);
    }
}

char lechar()
{
    fflush(stdout);
    char c;
    if (fread(&c, 1, 1, stdin) == 1) return c;
    return 0;
}

void crono_inicia(crono *c)
{
    clock_gettime(CLOCK_MONOTONIC, c);
}

double crono_parcial(crono *c)
{
    crono agora;
    clock_gettime(CLOCK_MONOTONIC, &agora);

    double segundos = agora.tv_sec - c->tv_sec;
    double nanosegundos = agora.tv_nsec - c->tv_nsec;
    return segundos + 1e-9 * nanosegundos;
}

void inicializacao (Dados_jogo *dados)
{
    dados->estados = dia;
    dados->pontuacao = 0;
    dados->encerrar = false;
    dados->municao = 30;
    dados->encerrar = false;
    dados->fim_partida = false;
    dados->arma_atual  = 0;
    dados->inimigo_inativos = 20;
    dados->onda = 0;
    dados->fim_onda = false;
    dados->inimigos_vivos = 20;
    dados->escudos = 3;
    dados->tempo = 2.0;
    dados->proxima_onda = false;
    dados->saida_esc = false;
    dados->gameover = false;
}

void joga_partida (Dados_jogo *dadosjogo)  // quando que é o fim da partida? quando o jogador morrer/ ele quitar
{
    while (!dadosjogo->fim_partida) {
        dadosjogo->inimigos_vivos = 20;
        dadosjogo->inimigo_inativos = 20;
        dadosjogo->municao = 30;
        dadosjogo->onda++;
        if (dadosjogo->onda != 1)
            dadosjogo->tempo = dadosjogo->tempo - (dadosjogo->tempo*0.1);
        joga_onda(dadosjogo);
        if (dadosjogo->saida_esc == false && dadosjogo->gameover == false)
            proxima_onda(dadosjogo);
    }
}

void gameover(Dados_jogo *dadosjogo)
{
    bool exit = false;
    while (!exit) {
        int tecla = lechar();
        printf("GAME OVER Pontuacao: %d  Onda: %d\r", dadosjogo->pontuacao, dadosjogo->onda); //arrunar essa função, a onda e pontuação esta somando quando clico no r
        //printf(" Digite 'esc' para sair ou 'r' para comecar outra partida\r");
        sair(dadosjogo, tecla);
        newonda(dadosjogo, tecla);
        if (tecla == 'r' || tecla == 27 ) {
            printf("\n");
            exit = true;
        }     
    }
}

void joga_onda (Dados_jogo *dadosjogo)
{
    char mapa[13];
    inicia_mapa(mapa, dadosjogo);
    crono c_inimigos;
    crono_inicia(&c_inimigos);
    while (!dadosjogo->fim_onda) {
        int tecla = lechar();
        sair(dadosjogo, tecla);
        troca_arma(dadosjogo, tecla); // correção de bug, não esta sendo estantaneo  a troca de arma
        atira(dadosjogo, mapa, tecla);
        if (dadosjogo->inimigos_vivos <= 0)
            dadosjogo->fim_onda = true;
        if (crono_parcial(&c_inimigos) >= dadosjogo->tempo) {
            atualiza_mapa(mapa, dadosjogo);
            crono_inicia(&c_inimigos);  // reinicia a contagem do zero
        }
        desenha_jogo(mapa, dadosjogo);
    }
    if (dadosjogo->saida_esc == false)
        pontuacao_itens(dadosjogo);
}

void pontuacao_itens(Dados_jogo *dadosjogo)
{
    dadosjogo->pontuacao += dadosjogo->municao*2; 
    dadosjogo->pontuacao += dadosjogo->escudos*10;
}

char inimigos_random()
{
    int numero = rand() % 11;

    if (numero == 10) {
        return 'N';
    } else {
        return '0' + numero;
    }
}

void inicia_mapa (char *mapa, Dados_jogo *dadosjogo)
{
    for (int i = 0; i < 13; i++) {
        if (i < 3) {
            mapa[i] = ')';
        } else if (i != 12) {
            mapa[i] = ' ';
        } else {
            mapa[i] = inimigos_random();
            dadosjogo->inimigo_inativos--;
        }
    }
}

void atualiza_mapa (char *mapa, Dados_jogo *dadosjogo) 
{
    for (int i = 0; i < 13; i++) {
        if (mapa[i] != ')' && mapa[i] != ' ') {
            if(i == 0) {
                dadosjogo->fim_onda = true;
                dadosjogo->fim_partida = true;
                dadosjogo->gameover = true;
            } else if(mapa[i-1] == ')') {
                mapa[i-1] = (mapa[i] == 'N') ? 'n' : ' ';
                if (mapa[i] != 'N')
                    dadosjogo->inimigos_vivos--;
                mapa[i] = ' ';
                dadosjogo->escudos--;
            } else {
                mapa[i-1] = mapa[i];
                mapa[i] = ' ';
            }
        }
    }
    if(dadosjogo->inimigo_inativos > 0) {
        mapa[12] = inimigos_random(); 
        dadosjogo->inimigo_inativos--; 
    }    
}

void desenha_jogo(char *mapa, Dados_jogo *dadosjogo)
{
    printf("  %d %d %c", dadosjogo->pontuacao, dadosjogo->municao, dadosjogo->arma_atual == 10 ? 'n' : dadosjogo->arma_atual + '0');
    fflush(stdout);
    for(int i = 0; i < 13; i++){
        printf("%c", mapa[i]);
    }
    printf("\r");
    fflush(stdout); //limpa o buffer de saida, para que o printf seja executado imediatamente
}

void troca_arma(Dados_jogo *dadosjogo, int tecla)
{
    if (tecla == 9) {
        if(dadosjogo->arma_atual == 10){
            dadosjogo->arma_atual = 0;
        } else{
            dadosjogo->arma_atual++;
        }
    }
}

void atira(Dados_jogo *dadosjogo, char *mapa, int tecla) //não da para adds mais nada ta com 21 já
{
    if (tecla == 13 && dadosjogo->municao > 0) {
        for(int i = 0; i < 13; i++) {
            if ((mapa[i] == 'N' || mapa[i] == 'n') && dadosjogo->arma_atual == 10) {
                mapa[i] = (mapa[i] == 'N') ? 'n' : ' ';
                if (mapa[i] == ' ') {
                    pontuacao_mortes(dadosjogo, i, true);
                    dadosjogo->inimigos_vivos--;   
                }
                break;
            } else if (mapa[i] == dadosjogo->arma_atual + '0') {
                mapa[i] = ' ';
                pontuacao_mortes(dadosjogo, i, false);
                dadosjogo->inimigos_vivos--;
                break;
            }
        }
        dadosjogo->municao--;
    }
}

void pontuacao_mortes(Dados_jogo *dadosjogo, int indice, bool n) 
{
    if (n) {
        if (dadosjogo->estados == dia)
            dadosjogo->pontuacao += (13-indice)*2;
        else    
            dadosjogo->pontuacao += (13-indice)*4; 
    } else {
        if (dadosjogo->estados == dia)
            dadosjogo->pontuacao += (13-indice);
        else    
            dadosjogo->pontuacao += (13-indice)*2; 
    }
}


void sair(Dados_jogo *dadosjogo, int tecla)
{
    if (tecla == 27) {
        dadosjogo->fim_onda = true;
        dadosjogo->fim_partida = true;
        dadosjogo->encerrar = true;
        dadosjogo->saida_esc = true;
    }
}

void newonda(Dados_jogo *dadosjogo, int tecla)
{
    if (tecla == 'r') {
        dadosjogo->proxima_onda = true;
        dadosjogo->fim_partida = false;
        dadosjogo->fim_onda = false;
    }
}
/*
implementar depois
void probabilidade_estado(Dados_jogo *dadosjogo)
{
    int chance_dia;

    switch (dadosjogo->onda)
    {
    case 1:
        chance_dia = 100;
        break;
    case 2:
        chance_dia = 80;
        break;
    case 3:
        chance_dia = 60;
        break;
    case 4:
        chance_dia = 40;
        break;
    default:
        chance_dia = 20;
        break;
    }

    int sorteio = rand() % 100;

    if (sorteio < chance_dia) {
        dadosjogo->estados = dia;
    } else {
        dadosjogo->estados = noite;
    }
}
*/

void proxima_onda(Dados_jogo *dadosjogo)
{
    bool exit = false;
    while (!exit) {
        int tecla = lechar();
        printf("  Tempo: %f Onda: %d Pontuacao: %d  -- Digite 'r' para jogar a proxima onda ou 'esc' para sair\r", dadosjogo->tempo, dadosjogo->onda, dadosjogo->pontuacao);
        sair(dadosjogo, tecla);
        newonda(dadosjogo, tecla);
        if (tecla == 'r' || tecla == 27 ) {
            printf("\n");
            exit = true;
        }     
    }
}