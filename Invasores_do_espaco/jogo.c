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
    int arma_atual;
    int inimigo_inativos;
    int inimigos_vivos;
    int onda;
    
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

int main()
{
    configura_terminal();
    Dados_jogo dadosjogo;
    srand(time(NULL));

    inicializacao (&dadosjogo);
    
    while (!(dadosjogo.encerrar)) {
        joga_partida(&dadosjogo);
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
}

void joga_partida (Dados_jogo *dadosjogo)  // quando que é o fim da partida? quando o jogador morrer/ ele quitar
{
    while (!dadosjogo->fim_partida) {
        dadosjogo->inimigos_vivos = 20;
        dadosjogo->inimigo_inativos = 20;
        dadosjogo->municao = 30;
        joga_onda(dadosjogo);
    }
}

void joga_onda (Dados_jogo *dadosjogo)
{
    char mapa[13];
    inicia_mapa(mapa, dadosjogo);
    desenha_jogo(mapa, dadosjogo);
    crono c_inimigos;
    crono_inicia(&c_inimigos);
    while (!dadosjogo->fim_onda) {
        int tecla = lechar();

        if (dadosjogo->inimigos_vivos <= 0) { //mudar isso escrito so por exemplo
            dadosjogo->fim_onda = true;
            dadosjogo->fim_partida = true;
            dadosjogo->encerrar = true;
        }
        if (tecla == 27) {
            dadosjogo->fim_onda = true;
            dadosjogo->fim_partida = true;
            dadosjogo->encerrar = true;
        }
        
        troca_arma(dadosjogo, tecla); // correção de bug, não esta sendo estantaneo a troca de arma
        atira(dadosjogo, mapa, tecla);
        desenha_jogo(mapa, dadosjogo);
        

        if (crono_parcial(&c_inimigos) >= 2.0) {
            atualiza_mapa(mapa, dadosjogo);
            //desenha_jogo(mapa, dadosjogo);
            crono_inicia(&c_inimigos);  // reinicia a contagem do zero
        }
    }
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
            mapa[i] = inimigos_random();// criar essa função
            dadosjogo->inimigo_inativos--;
        }
    }
}

void atualiza_mapa (char *mapa, Dados_jogo *dadosjogo) //da para simplificar essas condições
{
    for (int i = 0; i < 13; i++) {
        if (i == 0 && (mapa[i] != ')' && mapa[i] != ' ')) {
            dadosjogo->fim_onda = true;
            dadosjogo->fim_partida = true;
            dadosjogo->encerrar = true; //talvez tirar essa linha futuramente
            break;
        } else if (i == 12) {
            mapa[i-1] = mapa[i];
            if(dadosjogo->inimigo_inativos > 0){
                mapa[i] = inimigos_random();
                dadosjogo->inimigo_inativos--;
            } else {
                mapa[i] = ' ';
            }
        } else if (mapa[i] == ' ' && mapa[i -1] != ')' && i != 0) {
            mapa[i-1] = ' ';
        }  else if (mapa[i] == 'N' && mapa[i - 1] == ')') {
            mapa[i - 1] = 'n';
        } else if (mapa[i] != ')' && mapa[i] != ' ' && mapa[i-1] == ')') {
            mapa[i - 1] = ' ';
        } else if (mapa[i] != ' ' && mapa[i] != ')' && mapa[i-1] != ')') {
            mapa[i-1] =  mapa[i];
        }

    }
}

void desenha_jogo(char *mapa, Dados_jogo *dadosjogo)
{
    printf("  %d %d %c", dadosjogo->pontuacao, dadosjogo->municao, dadosjogo->arma_atual == 10 ? 'n' : dadosjogo->arma_atual + '0');
    for(int i = 0; i < 13; i++){
        printf("%c", mapa[i]);
    }
    printf("\r");
    //fflush(stdout); //limpa o buffer de saida, para que o printf seja executado imediatamente
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

void atira(Dados_jogo *dadosjogo, char *mapa, int tecla)
{
    if (tecla == 13) {
        for(int i = 0; i < 13; i++) {
            if(mapa[i] == 'N' && dadosjogo->arma_atual == 10) { 
                mapa[i] = 'n';
                break;
            }
            else if(mapa[i] == 'n' && dadosjogo->arma_atual == 10) {
                mapa[i] = ' ';
                dadosjogo->inimigos_vivos--;
                break;
            }
            else if(mapa[i] == dadosjogo->arma_atual + '0') {
                mapa[i] = ' ';
                dadosjogo->inimigos_vivos--;
                break;
            }
        }
    }
}