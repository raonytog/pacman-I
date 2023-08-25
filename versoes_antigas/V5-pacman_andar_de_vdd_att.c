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

// estrutura que armazena a coordenada do elemento no mapa
typedef struct {
    int x;
    int y;
} tCoordenada;

//estrutura para manipular o pacman e a qtd de vida
typedef struct {
    tEstatisticas pontuacoes;
    tCoordenada cordPac;
    int qtdVida;
} tPacman;

typedef struct {
    char g1;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantB;
} tFantasma1B;

typedef struct {
    char g1;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantP;
} tFantasma2P;

typedef struct {
    char g1;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantI;
} tFantasma3I;

typedef struct {
    char g1;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantC;
} tFantasma4C;

//estrutura para manipular os fantasmas e a qtd de vida
typedef struct {
    tFantasma1B fantB;
    tFantasma2P fantP;
    tFantasma3I fantI;
    tFantasma4C fantC;
} tFantasmas;

//estrutura geral do jogo
typedef struct {
    tFantasmas fantasmas;
    tPacman pacman;
    int qtdComida;
    int qtdJogadas;
    int nLinhas;
    int nColunas;
    char mapa[MAX_LINHA][MAX_COLUNA];
} tJogo;

//dump de funcoes implementadas 
tJogo LeMapa (char *diretorio);
tJogo EfetuaJogadaPacman (tJogo jogo, char jogada);
tJogo EfetuaJogadaFantasmas (tJogo jogo);
tCoordenada IdentificaCoordenadaPACMAN (tJogo jogo);
tFantasmas IdentificaFantasmasPresentes (tJogo jogo);
void ImprimeMapa (tJogo jogo);
void RealizaJogo (tJogo jogo);
void InicializaJogo ();

int main (int argc, char * argv[]) {
    if (argc <= 1) {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 1;
    }

    char diretorio[DIR_TAM_MAX];
    sprintf(diretorio, "%s", argv[1]);

    tJogo mapinha;
    mapinha = LeMapa(diretorio);
    InicializaJogo(mapinha);
    RealizaJogo(mapinha);

    return 0;
}

//funcao q recebe e le o arquivo de entrada do mapa p salvar no struct
tJogo LeMapa (char *diretorio) {
    tJogo jogo;
    FILE * fMapa = NULL;

    char diretorioDoMapa[DIR_TAM_MAX];
    sprintf(diretorioDoMapa, "%s/mapa.txt", diretorio);

    fMapa = fopen(diretorioDoMapa, "r");

    if (!fMapa) {
        printf("ERRO ao tentar abrir o arquivo de mapa!\n");
        exit(1);
    }

    fscanf(fMapa, "%d", &jogo.nLinhas);
    fscanf(fMapa, "%d", &jogo.nColunas);
    fscanf(fMapa, "%d", &jogo.qtdJogadas);

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        fscanf(fMapa,"\n");
        for (j = 0; j < jogo.nColunas; j++) {
            fscanf(fMapa,"%c", &jogo.mapa[i][j]);
        }
    }

    fclose (fMapa);
    return jogo;
}

//funcao pra imprimir o mapa do jogo a e qualquer todo instante
void ImprimeMapa (tJogo jogo) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        if (i != 0) printf("\n");
        for (j = 0; j < jogo.nColunas; j++) {
            printf("%c", jogo.mapa[i][j]);
        }
    }
}

//funcao pra comecar o jogo
//imprime info iniciais e saida
void InicializaJogo (tJogo jogo) {
    ImprimeMapa(jogo);

    tCoordenada coord;
    coord = IdentificaCoordenadaPACMAN(jogo);
    printf("\nPac-Man comecara o jogo na linha %d e coluna %d\n", 
                                    coord.x + 1, coord.y + 1);
}

//funcao para ler as jogadas do pacman e a consequencia delas
void RealizaJogo(tJogo Jogo) {
    int pontos = 0;
    char aux;

    while (scanf("%c%*c", &aux) == 1) {
        Jogo = EfetuaJogadaPacman(Jogo, aux); // jogada do pacman
        // Jogo = EfetuaJogadaFantasmas(Jogo);
        printf("\nEstado do jogo após o movimento '%c':\n", aux);
        ImprimeMapa(Jogo);
        printf("\nPontuação: %d\n", pontos);
    }
}

//funcao para realizar as jogadas 
tJogo EfetuaJogadaPacman(tJogo jogo, char acao) {
    int linha, coluna; char aux;
    tCoordenada coord;
    coord = IdentificaCoordenadaPACMAN(jogo);

    tPacman pac;
    pac.cordPac = coord;

    switch (acao) {
    case 'w':
        if (coord.x - 1 >= 0 && 
            jogo.mapa[coord.x-1][coord.y] != PAREDE) {  

                jogo.mapa[coord.x][coord.y] = VAZIO;
                jogo.mapa[coord.x-1][coord.y] = PACMAN;
            return jogo;
        }
        break;

    case 's':
        if (coord.x + 1 < jogo.nLinhas && 
            jogo.mapa[coord.x+1][coord.y] != PAREDE) {  

                jogo.mapa[coord.x][coord.y] = VAZIO;
                jogo.mapa[coord.x+1][coord.y] = PACMAN;
            return jogo;
        }
        break; 

    case 'a':
        if (coord.y - 1 >= 0 && 
            jogo.mapa[coord.x][coord.y-1] != PAREDE) {

                jogo.mapa[coord.x][coord.y] = VAZIO;
                jogo.mapa[coord.x][coord.y-1] = PACMAN;
            return jogo;
        }
        break; 

    case 'd':
        if (coord.y + 1 < jogo.nColunas && 
            jogo.mapa[coord.x][coord.y+1] != PAREDE) {

                jogo.mapa[coord.x][coord.y] = VAZIO;
                jogo.mapa[coord.x][coord.y+1] = PACMAN;
            return jogo;
        }
        break; 

    default:
        printf("\nJogada invalida.\nJogadas validas: a, d, s, w\n");
        return jogo;
        break;
    }
}

tJogo EfetuaJogadaFantasmas (tJogo mapa) {
    
}

tFantasmas IdentificaFantasmasPresentes(tJogo jogo) {
    tFantasmas fant;

    tFantasma1B f1;
    tFantasma2P f2;
    tFantasma3I f3;
    tFantasma4C f4;

    int i, j;

    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapa[i][j] ==  GHOST1) {
                f1.presenteNoJogo = 1;
                fant.fantB = f1;
            }

            if (jogo.mapa[i][j] ==  GHOST2) {
                f2.presenteNoJogo = 1;
                fant.fantP = f2;
            }

            if (jogo.mapa[i][j] ==  GHOST3) {
                f3.presenteNoJogo = 1;
                fant.fantI = f3;
            }

            if (jogo.mapa[i][j] ==  GHOST4) {
                f4.presenteNoJogo = 1;
                fant.fantC = f4;
            }
        }
    }


    return fant;
}

//funcao para identificar a linha q o pacman esta no comeco do jogo
tCoordenada IdentificaCoordenadaPACMAN(tJogo jogo) {
    tCoordenada coord;
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapa[i][j] == PACMAN) {
                coord.x = i;
                coord.y = j;
                return coord;   
            }
        }
    }
}