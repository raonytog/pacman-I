#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIR_TAM_MAX 1001
#define MAX_LINHA 41
#define MAX_COLUNA 101

//defines do conteudo do mapa
#define VAZIO ' '   //vazio
#define PAREDE '#'  //parede
#define PORTAL '@'  //portal 
#define COMIDA '*'  //comida
#define PACMAN '>'  //pacman
#define GHOST1 'B'  //fantasma horizontal   - start: esquerda
#define GHOST2 'P'  //fantasma vertical     - start: cima
#define GHOST3 'I'  //fantasma vertical     - start: baixo
#define GHOST4 'C'  //fantasma horizontal   - start: direita

//estrutura para manipular as estatisticas do ganhas pelo mapa
typedef struct {
    int nMovimentos;
    int nMovimentosSemPontos;
    int nColisoesComParede;
    int nMovimentosCima;
    int nMovimentoBaixo;
    int nMovimentoDireita;
    int nMovimentoEsquerda;
    int eatenFood;
} tEstatisticas;

//estrutura para manipular o pacman e a qtd de vida
typedef struct {
    int qtdVida;
    tEstatisticas pontuacoes;
} tPacman;

//estrutura para manipular os fantasmas e a qtd de vida
typedef struct {
    int qtdVida;
    char g1;
    char g2;
    char g3;
    char g4;
} tFantasma;

//estrutura geral do jogo
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
tMapa LeMapa (char *diretorio);
tMapa EfetuaJogada(tMapa mapa, char jogada);
void InicializaJogo ();
void ImprimeMapa (tMapa mapa);
void RealizaJogo(tMapa mapa);
int IdentificaColunaPacMan(tMapa mapa);
int IdentificaLinhaPacMan(tMapa mapa);

int main (int argc, char * argv[]) {
    if (argc <= 1) {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 1;
    }

    char diretorio[DIR_TAM_MAX];
    sprintf(diretorio, "%s", argv[1]);

    tMapa mapinha;
    mapinha = LeMapa(diretorio);
    InicializaJogo(mapinha);
    RealizaJogo(mapinha);

    return 0;
}

//funcao q recebe e le o arquivo de entrada do mapa p salvar no struct
tMapa LeMapa (char *diretorio) {
    tMapa map;
    FILE * fMapa = NULL;

    char diretorioDoMapa[DIR_TAM_MAX];
    sprintf(diretorioDoMapa, "%s/mapa.txt", diretorio);

    fMapa = fopen(diretorioDoMapa, "r");

    if (!fMapa) {
        printf("ERRO ao tentar abrir o arquivo de mapa!");
        exit(1);
    }

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

//funcao pra imprimir o mapa 
void ImprimeMapa (tMapa mapa) {
    int i, j;
    for (i = 0; i < mapa.nLinhas; i++) {
        if (i != 0) printf("\n");
        for (j = 0; j < mapa.nColunas; j++) {
            printf("%c", mapa.mapa[i][j]);
        }
    }
}

//funcao pra comecar o jogo
//imprime info iniciais e A IMPLEMENTAR saida 
void InicializaJogo (tMapa mapa) {
    ImprimeMapa(mapa);

    int linhaInicial, colunaInicial;
    linhaInicial  = IdentificaLinhaPacMan(mapa); 
    colunaInicial = IdentificaColunaPacMan(mapa);
    printf("\nPac-Man comecara o jogo na linha %d e coluna %d\n", 
                                    linhaInicial+1, colunaInicial+1);
}

// funcao para ler as jogadas do pacman
void RealizaJogo(tMapa mapa) {
    int pontos = 0;
    char aux;

    while (scanf("%c%*c", &aux) == 1) {
        mapa = EfetuaJogada(mapa, aux);
        printf("\nEstado do jogo após o movimento '%c':\n", aux);
        ImprimeMapa(mapa);
        printf("\nPontuação: %d\n", pontos);
    }
}

tMapa EfetuaJogada(tMapa mapa, char jogada) {
    int linha, coluna; char aux;

    switch (jogada) {
    case 'w':
        linha = IdentificaLinhaPacMan(mapa);
        coluna = IdentificaColunaPacMan(mapa);
        if (linha - 1 >= 0 && 
            mapa.mapa[linha-1][coluna] != PAREDE) {  
                mapa.mapa[linha][coluna] = VAZIO;
                mapa.mapa[linha-1][coluna] = PACMAN;
            return mapa;
        }
        break;

    case 's':
        linha = IdentificaLinhaPacMan(mapa);
        coluna = IdentificaColunaPacMan(mapa);
        if (linha + 1 < mapa.nLinhas && 
            mapa.mapa[linha+1][coluna] != PAREDE) {  
                mapa.mapa[linha][coluna] = VAZIO;
                mapa.mapa[linha+1][coluna] = PACMAN;
            return mapa;
        }
        break; 

    case 'a':
        linha = IdentificaLinhaPacMan(mapa);
        coluna = IdentificaColunaPacMan(mapa);
        if (coluna - 1 >= 0 && 
            mapa.mapa[linha][coluna] != PAREDE) {  
                mapa.mapa[linha][coluna] = VAZIO;
                mapa.mapa[linha][coluna-1] = PACMAN;
            return mapa;
        }
        break; 

    case 'd':
        linha = IdentificaLinhaPacMan(mapa);
        coluna = IdentificaColunaPacMan(mapa);
        if (coluna + 1 < mapa.nLinhas && 
            mapa.mapa[linha][coluna] != PAREDE) {  
                mapa.mapa[linha][coluna] = VAZIO;
                mapa.mapa[linha][coluna+1] = PACMAN;
            return mapa;
        }
        break; 

    default:
        printf("\nJogada invalida.\nJogadas validas: a, d, s, w\n");
        return mapa;
        break;
    }
}

//funcao para identificar a linha q o pacman esta no comeco do jogo
int IdentificaLinhaPacMan(tMapa mapa) {
    int i, j;
    for (i = 0; i < mapa.nLinhas; i++) {
        for (j = 0; j < mapa.nColunas; j++) {
            if (mapa.mapa[i][j] == PACMAN) {
                return i;
            }
        }
    }

    return 0;
}

//funcao para identificar a coluna q o pacman esta no comeco do jogo
int IdentificaColunaPacMan(tMapa mapa) {
    int i, j;
    for (i = 0; i < mapa.nLinhas; i++) {
        for (j = 0; j < mapa.nColunas; j++) {
            if (mapa.mapa[i][j] == PACMAN) {
                return j;
            }
        }
    }

    return 0;
}