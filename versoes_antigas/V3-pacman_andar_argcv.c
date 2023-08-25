#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIR_TAM_MAX 1001
#define MAX_LINHA 41
#define MAX_COLUNA 101

//defines do conteudo do mapa
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
void RealizaJogo(tMapa mapa, char * diretorio);
int IdentificaColunaPacMan(tMapa mapa);
int IdentificaLinhaPacMan(tMapa mapa);

int main (int argc, char * argv[]) {
    if (argc <= 0) {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 0;
    }

    char diretorio[DIR_TAM_MAX];
    sprintf(diretorio, "%s", argv[1]);

    tMapa mapinha;
    mapinha = LeMapa(diretorio);
    InicializaJogo(mapinha);
    RealizaJogo(mapinha, diretorio);

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
    } else {
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
    }

    fclose (fMapa);
    return map;
}

//funcao pra imprimir o mapa 
void ImprimeMapa (tMapa mapa) {
    int i, j;
    for (i = 0; i < mapa.nLinhas; i++) {
        printf("\n");
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
                                    linhaInicial, colunaInicial);
}

// funcao para ler as jogadas do pacman
void RealizaJogo(tMapa mapa, char * diretorio) {
    int pontos = 0;
    char aux, dirJogadas[DIR_TAM_MAX]; 
    sprintf(dirJogadas, "%s/jogadas.txt", diretorio);

    FILE * fJogadas = NULL;
    fJogadas = fopen(dirJogadas, "r");

    while (fscanf(fJogadas, "%c%*c", &aux) == 1) {
        mapa = EfetuaJogada(mapa, aux);
        printf("\nEstado do jogo após o movimento '%c':\n", aux);
        ImprimeMapa(mapa);
        printf("\nPontuação: %d", pontos);
    }

    fclose (fJogadas);
}

// le 
tMapa EfetuaJogada(tMapa mapa, char jogada) {
    int linha, coluna;

    switch (jogada) {
    case 'a':
        linha = IdentificaLinhaPacMan(mapa);
        coluna = IdentificaColunaPacMan(mapa);
        if (coluna - 1 >= 0 && mapa.mapa[linha][coluna] != PAREDE) {
            mapa.mapa[linha][coluna-1] = mapa.mapa[linha][coluna];
            mapa.mapa[linha][coluna] = ' ';
        }
        break;
    
    case 'd':
        linha = IdentificaLinhaPacMan(mapa);
        coluna = IdentificaColunaPacMan(mapa);
        if (coluna + 1 < mapa.nColunas && mapa.mapa[linha][coluna] != PAREDE) {
            mapa.mapa[linha][coluna+1] = mapa.mapa[linha][coluna];
            mapa.mapa[linha][coluna] = ' ';
        }
        break;

    case 's':
        linha = IdentificaLinhaPacMan(mapa);
        coluna = IdentificaColunaPacMan(mapa);
        if (linha - 1 >= 0 && mapa.mapa[linha][coluna] != PAREDE) {
            mapa.mapa[linha-1][coluna] = mapa.mapa[linha][coluna];
            mapa.mapa[linha][coluna] = ' ';
        }
        break;

    case 'w':
        linha = IdentificaLinhaPacMan(mapa);
        coluna = IdentificaColunaPacMan(mapa);
        if (linha + 1 < mapa.nLinhas && mapa.mapa[linha][coluna] != PAREDE) {
            mapa.mapa[linha+1][coluna] = mapa.mapa[linha][coluna];
            mapa.mapa[linha][coluna] = ' ';
        }
        break;

    default:
        printf("\nJogada invalida.\n Jogadas validas: A, D, S, W\n");
        break;
    }
    
    return mapa;
}

//funcao para identificar a linha q o pacman esta no comeco do jogo
int IdentificaLinhaPacMan(tMapa mapa) {
    int i, j;
    for (i = 0; i < mapa.nLinhas; i++) {
        for (j = 0; j < mapa.nColunas; j++) {
            if (mapa.mapa[i][j] == PACMAN) {
                return i+1;
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
                return j+1;
            }
        }
    }

    return 0;
}