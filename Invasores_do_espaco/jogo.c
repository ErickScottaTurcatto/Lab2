#define _POSIX_C_SOURCE 199309L // linha adds por conta de bug no CLOCK_MONOTONIC causado provavelmente pelo wsl
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

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
void som_de_spawn(char *mapa, int i);
void som_trocaarma(Dados_jogo *dadosjogo);
void radar(char *mapa, int tecla);
void som_fim_onda(Dados_jogo *dadosjogo);
void inicializa_arquivo();
void atualiza_scores(int *scores, bool newrecord);
void verifica_pontuacao(Dados_jogo *dadosjogo);

int main()
{
    configura_terminal();
    Dados_jogo dadosjogo;
    srand(time(NULL));

    inicializa_arquivo();//tirar isso aqui, vai zerar toda vez as pontuações
    inicializacao (&dadosjogo);
    
    while (!(dadosjogo.encerrar)) {
        joga_partida(&dadosjogo);
        if (dadosjogo.gameover) {
            verifica_pontuacao(&dadosjogo);
            gameover(&dadosjogo);
        }
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

void joga_partida (Dados_jogo *dadosjogo)  
{
    while (!dadosjogo->fim_partida) {
        dadosjogo->inimigos_vivos = 20;
        dadosjogo->inimigo_inativos = 20;
        dadosjogo->municao = 30;
        dadosjogo->escudos = 3;
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
        printf("GAME OVER Pontuacao: %d  Onda: %d\r", dadosjogo->pontuacao, dadosjogo->onda); 
        //printf(" Digite 'esc' para sair ou 'r' para comecar outra partida\r");
        sair(dadosjogo, tecla);
        newonda(dadosjogo, tecla);
        if (tecla == 'r' || tecla == 27 ) {
            dadosjogo->pontuacao = 0;
            dadosjogo->onda = 0;
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
        radar(mapa,tecla); 
        troca_arma(dadosjogo, tecla); // correção de bug, não esta sendo estantaneo  a troca de arma
        atira(dadosjogo, mapa, tecla);
        if (dadosjogo->inimigos_vivos <= 0){
            dadosjogo->fim_onda = true;
        }
        if (crono_parcial(&c_inimigos) >= dadosjogo->tempo) {
            atualiza_mapa(mapa, dadosjogo);
            crono_inicia(&c_inimigos);  // reinicia a contagem do zero
        }
        desenha_jogo(mapa, dadosjogo);
        som_fim_onda(dadosjogo);
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
        } else {
            mapa[i] = ' ';
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
        som_de_spawn(mapa, 12);
        dadosjogo->inimigo_inativos--; 
    }    
}

void desenha_jogo(char *mapa, Dados_jogo *dadosjogo)
{
    printf("  %d %d %c", dadosjogo->pontuacao, dadosjogo->municao, dadosjogo->arma_atual == 10 ? 'n' : dadosjogo->arma_atual + '0');
    fflush(stdout);
    for(int i = 0; i < 13; i++){
        //som_de_spawn(mapa, i);
        printf("%c", mapa[i]);
        
    }
    printf("\r");
    fflush(stdout); //limpa o buffer de saida, para que o printf seja executado imediatamente
}

void som_de_spawn(char *mapa, int i)
{
    char comando[50];
    if (mapa[i] != ' ' && mapa[i] != ')') {
        if (mapa[i] == 'n' || mapa[i] == 'N') 
            system("aplay -q Sons/11.3.wav &");
        else {
            sprintf(comando, "aplay -q Sons/%c.3.wav &", mapa[i]);
            system(comando);
        }
    }
}

void troca_arma(Dados_jogo *dadosjogo, int tecla)
{
    if (tecla == 9) {
        if(dadosjogo->arma_atual == 10){
            dadosjogo->arma_atual = 0;
        } else{
            dadosjogo->arma_atual++;
        }
        som_trocaarma(dadosjogo);
    }
}

void som_trocaarma(Dados_jogo *dadosjogo)
{
    char comando[50];
    if (dadosjogo->arma_atual == 10) {
        system("aplay -q Sons/11.3.wav &");
    }
    else {
        sprintf(comando, "aplay -q Sons/%c.3.wav &", dadosjogo->arma_atual + '0');
        system(comando);
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

void radar(char *mapa, int tecla) //pensar em subtrair o tempo parado no cronometro
{
    if (tecla == ' ') {
        sleep(1);
        for (int i = 0; i < 13; i++) {
            if (mapa[i] == ' ') {
                system("aplay -q Sons/x.3.wav");
            }
            else if (mapa[i] == ')') {
                system("aplay -q Sons/12.3.wav");
            }
            else {
                som_de_spawn(mapa, i);
            }
            sleep(1); 
        }
        sleep(1);
    }
}

void som_fim_onda(Dados_jogo *dadosjogo) // quando esc clicado toca tbm
{
    if (dadosjogo->fim_onda) {
        system("aplay -q Sons/6.2.wav Sons/7.2.wav Sons/8.2.wav Sons/9.2.wav Sons/9.2.wav");
        sleep(1.5);
    }
}


void inicializa_arquivo()
{
    FILE *arquivo = fopen("score.txt", "w");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n");
    } else {
        //fprintf(arquivo, "1- %d\n2- %d\n3- %d\n", 0, 0, 0);
        fprintf(arquivo, "%d\n%d\n%d\n", 0, 0, 0);
    }

    fclose(arquivo);
}

void verifica_pontuacao(Dados_jogo *dadosjogo)
{
    bool new_record = true;
    int scores[3];
    FILE *arquivo = fopen("score.txt", "r");
    fscanf(arquivo, "%d", &scores[0]);
    fscanf(arquivo, "%d", &scores[1]);
    fscanf(arquivo, "%d", &scores[2]);
    if (dadosjogo->pontuacao > scores[0]) {
        scores[2] = scores[1];
        scores[1] = scores[0]; 
        scores[0] = dadosjogo->pontuacao;
    }
    else if (dadosjogo->pontuacao > scores[1]) {
        scores[2] = scores[1];
        scores[1] = dadosjogo->pontuacao;
    }
    else if (dadosjogo->pontuacao > scores[2]) {
        scores[2] = dadosjogo->pontuacao;
    }
    else {
        new_record = false;
    }
    fclose(arquivo);
    atualiza_scores(scores, new_record);
    
}


void atualiza_scores(int *scores, bool newrecord)
{
    FILE *arquivo = fopen("score.txt", "w");

    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo\n");
    } else {
        fprintf(arquivo, "%d\n%d\n%d\n", scores[0], scores[1], scores[2]);
        if (newrecord) {
            printf("\033[2K\r");
            printf("NOVO RECORD\n");
            printf("%d\n", scores[0]);
            printf("%d\n", scores[1]);
            printf("%d\n", scores[2]);
        }
    }
    fclose(arquivo);
}
