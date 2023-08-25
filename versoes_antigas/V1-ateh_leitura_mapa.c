#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINHA 40
#define MAX_COLUNA 100

#define PAREDE #
#define COMIDA *

#define PACMAN >
#define GHOST1 B
#define GHOST2 P
#define GHOST3 I 
#define GHOST4 C

//estrutura para manipular as estatisticas do ganhas pelo mapa
typedef struct {
    int nMovimentos;
    int nMovimentosSemPontos;
    int nColisoesComParede;
    int nMovimentosCima;
    int nMovimentoBaixo;
    int nMovimentoDireita;
    int nMovimentoEsquerda;
} tEstatisticas;

//estrutura para manipular o pacman e a qtd de vida
typedef struct {
    int qtdVida;
    tEstatisticas pontuacoes;
} tPacman;

//estrutura para manipular os fantasmas e a qtd de vida
typedef struct {
    int qtdVida;
    char GHOST1;
    char GHOST2;
    char GHOST3;
    char GHOST4;
} tFantasma;

// estrutura geral do jogo
typedef struct {
    tFantasma fantasmas;
    tPacman pacman;
    int qtdComida;
    int qtdJogadas;
    int nLinhas;
    int nColunas;
    char mapa[MAX_LINHA][MAX_COLUNA];
} tMapa;

//dump de funcoes implementadas 
void InicializaJogo ();
void ImprimeMapa (tMapa mapa);
tMapa LeMapa ();


int main () {
    tMapa mapinha;
    mapinha = LeMapa();
    ImprimeMapa(mapinha);
    InicializaJogo(mapinha);



    return 0;
}

//funcao q recebe o arquivo de entrada do mapa e suas informacoes 
//para salvar na respectiva struct 
tMapa LeMapa () {
    tMapa map;
    FILE * fMapa;
    fMapa = fopen("C:/Users/DELL/Documents/TI/Prog1/Trabalho/Casos-v1/Gabarito/simples/01/mapa.txt", "r");
    fscanf(fMapa, "%d", &map.nLinhas);
    fscanf(fMapa, "%d", &map.nColunas);
    fscanf(fMapa, "%d", &map.qtdJogadas);

    int i, j;
    for (i = 0; i < map.nLinhas; i++) {
        fscanf(fMapa,"\n");
        for (j = 0; j < map.nColunas; j++) {
            fscanf(fMapa,"%c", &map.mapa[i][j]);
        }
    }
    fclose (fMapa);
    return map;
}

// funcao pra imprimir o mapa qnd solicitado
void ImprimeMapa (tMapa mapa) {
    int i, j;
    for (i = 0; i < mapa.nLinhas; i++) {
        printf("\n");
        for (j = 0; j < mapa.nColunas; j++) {
            printf("%c", mapa.mapa[i][j]);
        }
    }
}

// funcao pra comecar o jogo
void InicializaJogo () {

}