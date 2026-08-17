#define _POSIX_C_SOURCE 199309L // linha adds por conta de bug no CLOCK_MONOTONIC 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    enum {
        dia,
        noite
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
    double tempo_noturno;
} Dados_jogo;

typedef struct timespec crono;

void configura_terminal();
char lechar();
void crono_inicia(crono *c);
double crono_parcial(crono *c);
void inicializacao (Dados_jogo *dados);
void joga_partida (Dados_jogo *dadosjogo);
void joga_onda (Dados_jogo *dadosjogo);
char inimigos_random(Dados_jogo *dadosjogo);
void inicia_mapa(char *mapa, int n);
void atualiza_mapa (char *mapa, Dados_jogo *dadosjogo, int n);
void desenha_jogo(char *mapa, Dados_jogo *dadosjogo, int n);
void troca_arma(Dados_jogo *dadosjogo, int tecla);
void atira(Dados_jogo *dadosjogo, char *mapa, int tecla, int n);
void sair(Dados_jogo *dadosjogo, int tecla);
void newonda(Dados_jogo *dadosjogo, int tecla);
void proxima_onda(Dados_jogo *dadosjogo);
void pontuacao_mortes(Dados_jogo *dadosjogo, int indice, bool n); 
void pontuacao_itens(Dados_jogo *dadosjogo);
void gameover(Dados_jogo *dadosjogo);
void som_de_spawn(char *mapa, int i);
void som_trocaarma(Dados_jogo *dadosjogo);
void radar(char *mapa, int tecla, int n);
void som_fim_onda(Dados_jogo *dadosjogo);
void inicializa_arquivo();
void atualiza_scores(int *scores, bool newrecord);
void verifica_pontuacao(Dados_jogo *dadosjogo);
void inicializacoes_partida (Dados_jogo *dadosjogo);
void probabilidade_estado(Dados_jogo *dadosjogo);
void atualiza_inimigos_onda(Dados_jogo *dadosjogo, char *mapa, crono *c_inimigos, int n);
void sorteio(Dados_jogo *dadosjogo, int chance_dia);
void troca_posicoes_ranking(Dados_jogo *dadosjogo, int *scores, bool *new_record);

int main()
{
    configura_terminal();
    Dados_jogo dadosjogo;
    srand(time(NULL));

    inicializa_arquivo();
    inicializacao (&dadosjogo);
    
    while (!(dadosjogo.encerrar)) {
        joga_partida(&dadosjogo);
        if (dadosjogo.gameover) {
            verifica_pontuacao(&dadosjogo);
            gameover(&dadosjogo);
        }
    }
    system("stty sane");
}

//deixa o terminal em modo cru
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

//para leitura do teclado
char lechar()
{
    fflush(stdout);
    char c;
    if (fread(&c, 1, 1, stdin) == 1) return c;
    return 0;
}

//inicia o cronometro
void crono_inicia(crono *c)
{
    clock_gettime(CLOCK_MONOTONIC, c);
}

// retorna o tempo passado desde que o cronômetro *c foi iniciado
double crono_parcial(crono *c)
{
    crono agora;
    clock_gettime(CLOCK_MONOTONIC, &agora);

    double segundos = agora.tv_sec - c->tv_sec;
    double nanosegundos = agora.tv_nsec - c->tv_nsec;
    return segundos + 1e-9 * nanosegundos;
}

//inicializa os campos do struct Dados_jogo
void inicializacao (Dados_jogo *dados)
{
    dados->estados = dia;
    dados->pontuacao = 0;
    dados->encerrar = false;
    dados->municao = 30;
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

//funcao que inicializa os dados antes da onda e chama a onda
void joga_partida (Dados_jogo *dadosjogo)  
{
    while (!dadosjogo->fim_partida) {
        dadosjogo->onda++;
        probabilidade_estado(dadosjogo);
        inicializacoes_partida (dadosjogo);
        if (dadosjogo->onda != 1) {
            dadosjogo->tempo = dadosjogo->tempo - (dadosjogo->tempo * 0.1);
        }

        if (dadosjogo->estados == noite) {
            dadosjogo->tempo_noturno = 3 * dadosjogo->tempo;
        }
        joga_onda(dadosjogo);
        if (dadosjogo->saida_esc == false && dadosjogo->gameover == false)
            proxima_onda(dadosjogo);
    }
}

//inicializações casa onda nova
void inicializacoes_partida (Dados_jogo *dadosjogo) 
{
    if (dadosjogo->estados == dia) {
        dadosjogo->inimigos_vivos = 20;
        dadosjogo->inimigo_inativos = 20;
        dadosjogo->municao = 30;
    }
    else {
        dadosjogo->inimigos_vivos = 15;
        dadosjogo->inimigo_inativos = 15;
        dadosjogo->municao = 30;
        if (dadosjogo->arma_atual%2 == 1)
            dadosjogo->arma_atual++;
    }
    dadosjogo->escudos = 3;
}

//gera a tela e o estado de gameover
void gameover(Dados_jogo *dadosjogo)
{
    bool exit = false;
    printf("GAME OVER Pontuacao: %d  Onda: %d\n", dadosjogo->pontuacao, dadosjogo->onda); 
    printf("Digite 'esc' para sair ou 'r' para comecar outra partida\n");
    while (!exit) {
        int tecla = lechar();
        sair(dadosjogo, tecla);
        newonda(dadosjogo, tecla);
        if (tecla == 'r') {
            dadosjogo->pontuacao = 0;
            dadosjogo->onda = 0;
            dadosjogo->gameover = false;
            dadosjogo->tempo = 2.0;
            exit = true;
        } else if (tecla == 27){
            dadosjogo->pontuacao = 0;
            dadosjogo->onda = 0;
            exit = true;
        }     
    }
}

//responsavel por iniciar a onda e conter toda a mecânica necessária da onda
void joga_onda (Dados_jogo *dadosjogo)
{
    char mapa[13];
    int n = (dadosjogo->estados == dia) ? 13 : 8;
    inicia_mapa(mapa, n);
    crono c_inimigos;
    crono_inicia(&c_inimigos);
    while (!dadosjogo->fim_onda) {
        int tecla = lechar();
        sair(dadosjogo, tecla);
        radar(mapa, tecla, n); 
        troca_arma(dadosjogo, tecla); 
        atira(dadosjogo, mapa, tecla, n);
        if (dadosjogo->inimigos_vivos <= 0)
            dadosjogo->fim_onda = true;
        else
            atualiza_inimigos_onda(dadosjogo, mapa, &c_inimigos, n);
        desenha_jogo(mapa, dadosjogo, n);
        som_fim_onda(dadosjogo);
    }
    if (dadosjogo->saida_esc == false)
        pontuacao_itens(dadosjogo);
}

//define qual é o valor definitivo do tempo (dia/noite), atualiza o mapa e faz a conta do tempo do cronometro
void atualiza_inimigos_onda(Dados_jogo *dadosjogo, char *mapa, crono *c_inimigos, int n) 
{
    double tempodefinido;
    if (dadosjogo->estados == dia) 
        tempodefinido = dadosjogo->tempo;
    else
        tempodefinido = dadosjogo->tempo_noturno;
    if (crono_parcial(c_inimigos) >= tempodefinido) {
        atualiza_mapa(mapa, dadosjogo, n);
        crono_inicia(c_inimigos);  // reinicia a contagem do zero
    }
}

//responsavel por pegar a pontuação da munição e dos escudos
void pontuacao_itens(Dados_jogo *dadosjogo)
{
    dadosjogo->pontuacao += dadosjogo->municao*2; 
    dadosjogo->pontuacao += dadosjogo->escudos*10;
}

//gera os inimigos
char inimigos_random(Dados_jogo *dadosjogo)
{
    int numero = rand() % 11;

    if (dadosjogo->estados == noite) {
        if (numero%2 == 1) {
            numero++;
        }
    }

    if (numero == 10) {
        return 'N';
    } else {
        return '0' + numero;
    }
}

//starta as posições do mapa
void inicia_mapa(char *mapa, int n)
{
    for (int i = 0; i < n; i++) {
        if (i < 3) {
            mapa[i] = ')';
        } else {
            mapa[i] = ' ';
        } 
    }
}

//faz as movimentações e colizões com os escudos
void atualiza_mapa (char *mapa, Dados_jogo *dadosjogo, int n) 
{
    for (int i = 0; i < n; i++)
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
    if(dadosjogo->inimigo_inativos > 0) {
        mapa[n-1] = inimigos_random(dadosjogo); 
        som_de_spawn(mapa, n-1);
        dadosjogo->inimigo_inativos--; 
    }    
}

//desenha na tela o jogo
void desenha_jogo(char *mapa, Dados_jogo *dadosjogo, int n)
{
    printf("  %d %d %c", dadosjogo->pontuacao, dadosjogo->municao, dadosjogo->arma_atual == 10 ? 'n' : dadosjogo->arma_atual + '0');
    fflush(stdout);
    if (dadosjogo->estados == dia) {
        for(int i = 0; i < n; i++)
            printf("%c", mapa[i]);
    }
    
    printf("\r");
    fflush(stdout); //limpa o buffer de saida, para que o printf seja executado imediatamente
}

//som dos inimigos spawnando
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

//troca as armas
void troca_arma(Dados_jogo *dadosjogo, int tecla)
{
    if (tecla == 9) {
        if (dadosjogo->estados == noite) {
            if(dadosjogo->arma_atual == 10)
                dadosjogo->arma_atual = 0;
            else
                dadosjogo->arma_atual+=2;
        }
        else {
            if(dadosjogo->arma_atual == 10){
                dadosjogo->arma_atual = 0;
            } else{
                dadosjogo->arma_atual++;
            }
        }
        
        som_trocaarma(dadosjogo);
    }
}

//som da troca de armas
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

//atira
void atira(Dados_jogo *dadosjogo, char *mapa, int tecla, int n) //não da para adds mais nada ta com 21 já
{
    if (tecla == 13 && dadosjogo->municao > 0) {
        for(int i = 0; i < n; i++) {
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

//controla a pontuação das casas em que o inimigo morre
void pontuacao_mortes(Dados_jogo *dadosjogo, int indice, bool n) 
{
    if (n) {
        if (dadosjogo->estados == dia)
            dadosjogo->pontuacao += (13-indice)*2;
        else    
            dadosjogo->pontuacao += (8-indice)*4; 
    } else {
        if (dadosjogo->estados == dia)
            dadosjogo->pontuacao += (13-indice);
        else    
            dadosjogo->pontuacao += (8-indice)*2; 
    }
}

//função da tecla esc
void sair(Dados_jogo *dadosjogo, int tecla)
{
    if (tecla == 27) {
        dadosjogo->fim_onda = true;
        dadosjogo->fim_partida = true;
        dadosjogo->encerrar = true;
        dadosjogo->saida_esc = true;
    }
}

//função da tecla r
void newonda(Dados_jogo *dadosjogo, int tecla)
{
    if (tecla == 'r') {
        dadosjogo->proxima_onda = true;
        dadosjogo->fim_partida = false;
        dadosjogo->fim_onda = false;
    }
}

//define se sera dia ou noite a onda
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
    sorteio(dadosjogo, chance_dia);
}

//faz o sorteio se o estado sera dia ou noite
void sorteio(Dados_jogo *dadosjogo, int chance_dia)
{
    int sorteio = rand() % 100;
    if (sorteio < chance_dia)
        dadosjogo->estados = dia;
    else 
        dadosjogo->estados = noite;
}

//gera a tela apos o fim da onda e define o que acontece depois
void proxima_onda(Dados_jogo *dadosjogo)
{
    bool exit = false;
    while (!exit) {
        int tecla = lechar();

        printf(" Onda: %d Pontuacao: %d  -- Digite 'r' para jogar a proxima onda ou 'esc' para sair\r", dadosjogo->onda, dadosjogo->pontuacao); 
        sair(dadosjogo, tecla);
        newonda(dadosjogo, tecla);
        if (tecla == 'r' || tecla == 27 ) {
            printf("\n");
            exit = true;
        }     
    }
}

//faz a função do radar
void radar(char *mapa, int tecla, int n) 
{
    if (tecla == ' ') {
        sleep(1);
        for (int i = 0; i < n; i++) {
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

//som de quando acaba a onda
void som_fim_onda(Dados_jogo *dadosjogo) 
{
    if (dadosjogo->fim_onda) {
        system("aplay -q Sons/6.2.wav Sons/7.2.wav Sons/8.2.wav Sons/9.2.wav Sons/9.2.wav");
        struct timespec espera = {1, 500000000};
        nanosleep(&espera, NULL);
    }
}

//inicia o arquivo caso não tenha nada gravado nele
void inicializa_arquivo()
{
    FILE *arquivo = fopen("score.txt", "r");

    if (arquivo == NULL) {
        arquivo = fopen("score.txt", "w");

        if (arquivo == NULL) {
            printf("Erro ao criar arquivo\n");
            return;
        }

        fprintf(arquivo, "0\n0\n0\n");
    }
    fclose(arquivo);
}

//verifica a pontuação do jogador com as pontuações salvas no arquivo para 
//definir o ranking de records
void verifica_pontuacao(Dados_jogo *dadosjogo)
{
    bool new_record = true;
    int scores[3] = {0, 0, 0}; 

    FILE *arquivo = fopen("score.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir score.txt para leitura\n");
        arquivo = NULL; 
    } else {
        int saida = fscanf(arquivo, "%d %d %d", &scores[0], &scores[1], &scores[2]);
        if (saida != 3) {
            scores[0] = scores[1] = scores[2] = 0;
        }
        fclose(arquivo);
    }
    troca_posicoes_ranking(dadosjogo, scores, &new_record);
    atualiza_scores(scores, new_record);
}

//troca as posições do ranking no vetor scores
void troca_posicoes_ranking(Dados_jogo *dadosjogo, int *scores, bool *new_record)
{
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
        *new_record = false;
    }
}

//atualiza no arquivo os scores
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
        fclose(arquivo);
    }
}