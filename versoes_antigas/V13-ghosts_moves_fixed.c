#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIR_TAM_MAX 1001
#define MAX_LINHA 41
#define MAX_COLUNA 101

//defines do conteudo do mapa
#define VAZIO  ' '  
#define PAREDE '#'  
#define PORTAL '@'  
#define COMIDA '*'  
#define PACMAN '>' 
#define GHOST1 'B'  //fantasma horizontal   - inicio: esquerda
#define GHOST2 'P'  //fantasma vertical     - inicio: cima
#define GHOST3 'I'  //fantasma vertical     - inicio: baixo
#define GHOST4 'C'  //fantasma horizontal   - inicio: direita

//estrutura para manipular as estatisticas de acordo com as jogadas
typedef struct {
    int nMovimentosSemPontos;
    int nColisoesComParede;
    int nMovimentosCima;
    int nMovimentoBaixo;
    int nMovimentoDireita;
    int nMovimentoEsquerda;
    int pacmanComeu;
} tEstatisticas;

// estrutura que armazena a coordenada do elemento no mapa
typedef struct {
    int x;
    int y;
} tCoordenada;

//estrutura para manipular o pacman de acordo com a vida e as coordenadas
typedef struct {
    tCoordenada cordPac;
    int qtdVida;
} tPacman;

// estrutura para manipular o fantasma de de acordo:
//  * se comeu comida;
//  * se esta no jogo;
//  * qtd de vida;
//  * coordenadas;
typedef struct {
    char g1;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantB;
} tFantasma1B;

// mesma do tFantasma1B
typedef struct {
    char g2;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantP;
} tFantasma2P;

// mesma do tFantasma1B
typedef struct {
    char g3;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantI;
} tFantasma3I;

// mesma do tFantasma1B
typedef struct {
    char g4;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantC;
} tFantasma4C;

//estrutura geral do jogo
typedef struct {
    tEstatisticas stats;
    tCoordenada aux1;
    tCoordenada aux2;
    tCoordenada aux3;
    tCoordenada aux4;
    tFantasma1B fb;
    tFantasma2P fp;
    tFantasma3I fi;
    tFantasma4C fc;
    tPacman pacman;
    int deltaMovF1; // qtd de avancos do fantasma 1 (B)
    int deltaMovF2; // qtd de avancos do fantasma 2 (P)
    int deltaMovF3; // qtd de avancos do fantasma 3 (I)
    int deltaMovF4; // qtd de avancos do fantasma 4 (C)
    int deltaMovP;  // qtd de avancos do pacman
    int gameover;   // define se eh ou nao fim de jogo
    int qtdComida;  // diz qnts comidas tem inicialmente no mapa
    int qtdJogadas; // diz qnts jogadas o pacman pode ter na partida
    int nLinhas;    // num de linhas presentes na matriz do mapa
    int nColunas;   // num de colunas presentes na matriz do mapa
    char mapa[MAX_LINHA][MAX_COLUNA];
} tJogo;

//dump de funcoes, explicacoes sobre cada logo abaixo da main
tJogo LeMapa (char *diretorio);
tCoordenada IdentificaCoordenada(tJogo jogo, char s);
void ImprimeMapa (tJogo jogo);
tJogo InicializaJogo (tJogo jogo);
void RealizaJogo(tJogo jogo);
tJogo EfetuaJogadaPacman (tJogo jogo, char acao);
int RetornaVidaPACMAN (tJogo jogo);
int RetornaPontuacao (tJogo jogo);
int EhFantasma(tJogo jogo, int l, int c);
void RetornaGameOver (tJogo jogo);
tJogo IdentificaFantasmasExistentes(tJogo jogo);
tJogo EfetuaJogadaFantasma (tJogo jogo);
tJogo Fantasma1Bmovimento (tJogo jogo);
tJogo Fantasma2Pmovimento (tJogo jogo);
tJogo Fantasma3Imovimento (tJogo jogo);
tJogo Fantasma4Cmovimento (tJogo jogo);


int main (int argc, char * argv[]) {
    if (argc <= 1) {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 1;
    }

    char diretorio[DIR_TAM_MAX];
    sprintf(diretorio, "%s", argv[1]);

    tJogo mapinha;
    tEstatisticas status;
    mapinha = LeMapa(diretorio);
    mapinha = InicializaJogo(mapinha);
    RealizaJogo(mapinha);

    return 0;
}

// le o arquvio mapa.txt e armazena as 
// informacoes na estrutura do jogo para armazenar e manipular o mapa, posteriormente
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

// identifica a coordenada x e y dentro da matriz do mapa do elemento passado como parametro
// elemento deve ser um caractere sempre
tCoordenada IdentificaCoordenada(tJogo jogo, char s) {
    tCoordenada coord;
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapa[i][j] == s) {
                coord.x = i;
                coord.y = j;
                return coord;   
            }
        }
    }
}

// imprime o mapa salvo na estrutura do jogo
void ImprimeMapa (tJogo jogo) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        if (i != 0) printf("\n");
        for (j = 0; j < jogo.nColunas; j++) {
            printf("%c", jogo.mapa[i][j]);
        }
    }
}

// inicializa o jogo, dando as informacoes de linha e coluna inicial na saida
// inicializa as variaveis do jogo, tal como as estatisticas
// A IMPLEMENTAR saida de arquivos
tJogo InicializaJogo (tJogo jogo) {
    ImprimeMapa(jogo);
    tCoordenada coord;
    coord = IdentificaCoordenada(jogo, PACMAN);
    printf("\nPac-Man comecara o jogo na linha %d e coluna %d\n",  coord.x + 1, coord.y + 1);



    jogo.gameover = 0;
    jogo.deltaMovF1 = 1;
    jogo.deltaMovF2 = 1;
    jogo.deltaMovF3 = 1;
    jogo.deltaMovF4 = 1;
    jogo.deltaMovP = 1;
    jogo.qtdComida = 0;
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if(jogo.mapa[i][j] == COMIDA) jogo.qtdComida++;
        }
    }

    tEstatisticas status;
    status.nColisoesComParede = 0;
    status.nMovimentoBaixo = 0;
    status.nMovimentosCima = 0;
    status.nMovimentoEsquerda = 0;
    status.nMovimentoDireita = 0;
    status.nMovimentosSemPontos = 0;
    status.pacmanComeu = 0;
    jogo.stats = status;

    tPacman pac;
    pac.qtdVida = 1;
    pac.cordPac = coord;
    jogo.pacman = pac;
    return jogo;
}

// retorna true se encontra um fantasma na coordenada indicada
int EhFantasma(tJogo jogo, int l, int c) {
    if (jogo.mapa[l][c] == GHOST1 ||
        jogo.mapa[l][c] == GHOST2 ||
        jogo.mapa[l][c] == GHOST3 ||
        jogo.mapa[l][c] == GHOST4) {
            return 1;
        }

    return 0;
}

// realiza as jogadas do pacman de acordo com a tecla pressionada
tJogo EfetuaJogadaPacman (tJogo jogo, char acao) {
    tCoordenada coord;
    coord = IdentificaCoordenada(jogo, PACMAN);

    tPacman pac;
    pac = jogo.pacman;

    tEstatisticas stats;
    stats = jogo.stats;

    switch (acao) {
    case 'w':
        if (coord.x - 1 >= 0 && 
            jogo.mapa[coord.x-1][coord.y] != PAREDE) {  
                if (jogo.mapa[coord.x-1][coord.y] == COMIDA) {
                    stats.pacmanComeu++;
                    jogo.stats = stats;
                }

                if (EhFantasma(jogo, coord.x-1, coord.y)) {
                        pac.qtdVida--;
                        jogo.pacman = pac;
                        return jogo;
                    }
                jogo.stats = stats;
                jogo.mapa[coord.x][coord.y] = VAZIO;
                jogo.mapa[coord.x-1][coord.y] = PACMAN;
            return jogo;
        }
        break;

    case 's':
        if (coord.x + 1 < jogo.nLinhas && 
            jogo.mapa[coord.x+1][coord.y] != PAREDE) {  
                if (jogo.mapa[coord.x+1][coord.y] == COMIDA) {
                    stats.pacmanComeu++;
                    jogo.stats = stats;
                }

                if (EhFantasma(jogo, coord.x+1, coord.y)) {
                        pac.qtdVida--;
                        jogo.pacman = pac;
                        return jogo;
                    }

                jogo.mapa[coord.x][coord.y] = VAZIO;
                jogo.mapa[coord.x+1][coord.y] = PACMAN;
            return jogo;
        }
        break; 

    case 'a':
        if (coord.y - 1 >= 0 && 
            jogo.mapa[coord.x][coord.y-1] != PAREDE) {

                if (jogo.mapa[coord.x][coord.y-1]  == COMIDA) {
                    stats.pacmanComeu++;
                    jogo.stats = stats;
                }

                if (EhFantasma(jogo, coord.x, coord.y-1)) {
                        pac.qtdVida--;
                        jogo.pacman = pac;
                        return jogo;
                    }

                jogo.mapa[coord.x][coord.y] = VAZIO;
                jogo.mapa[coord.x][coord.y-1] = PACMAN;
            return jogo;
        }
        break; 

    case 'd':
        if (coord.y + 1 < jogo.nColunas && 
            jogo.mapa[coord.x][coord.y+1] != PAREDE) {

                if (jogo.mapa[coord.x][coord.y+1]  == COMIDA) {
                    stats.pacmanComeu++;
                    jogo.stats = stats;
                }

                if (EhFantasma(jogo, coord.x, coord.y+1)) {
                        pac.qtdVida--;
                        jogo.pacman = pac;
                        return jogo;
                    }

                jogo.mapa[coord.x][coord.y] = VAZIO;
                jogo.mapa[coord.x][coord.y+1] = PACMAN;
            return jogo;
        }
        break; 

    default:
        break;
    }
}

// indica quais fantasmas existem no mapa
tJogo IdentificaFantasmasExistentes(tJogo jogo) {

    tFantasma1B fb;
    tFantasma2P fp;
    tFantasma3I fi;
    tFantasma4C fc;

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapa[i][j] ==  GHOST1) {
                fb.presenteNoJogo = 1;
            }

            if (jogo.mapa[i][j] ==  GHOST2) {
                fp.presenteNoJogo = 1;
            }

            if (jogo.mapa[i][j] ==  GHOST3) {
                fi.presenteNoJogo = 1;
            }

            if (jogo.mapa[i][j] ==  GHOST4) {
                fc.presenteNoJogo = 1;
            }
        }
    }

    jogo.fb = fb;
    jogo.fp = fp;
    jogo.fi = fi;
    jogo.fc = fc;
    return jogo;
}

// realiza as jogadas dos fantasmas 
tJogo EfetuaJogadaFantasma (tJogo jogo) {
    jogo = Fantasma1Bmovimento(jogo);
    jogo = Fantasma2Pmovimento(jogo);
    jogo = Fantasma3Imovimento(jogo);
    jogo = Fantasma4Cmovimento(jogo);
    return jogo;
}

// movimenta o fantasma B
tJogo Fantasma1Bmovimento (tJogo jogo) {
    tCoordenada temp;   tFantasma1B fb;     tPacman pac;
    temp = jogo.aux1;   fb = jogo.fb;       pac = jogo.pacman;

    tCoordenada coord;
    if (jogo.fb.presenteNoJogo) {
        coord = IdentificaCoordenada(jogo, GHOST1);
        if (coord.y - jogo.deltaMovF1 >= 0 && coord.y + jogo.deltaMovF1 < jogo.nColunas) {

            if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] != PAREDE) {
                 if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] == VAZIO) {
                    jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] = GHOST1;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] == PACMAN) {
                    jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] = GHOST1;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                    pac.qtdVida--;
                    jogo.pacman = pac;
                }

                if (fb.fantasmaGulao) {
                    fb.fantasmaGulao = 0;
                    jogo.fb = fb;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                }

                if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] == COMIDA) {
                    temp.x = coord.x; 
                    temp.y = coord.y - jogo.deltaMovF1;
                    jogo.aux1 = temp;
                    fb.fantasmaGulao = 1;
                    jogo.fb = fb;
                    jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] = GHOST1;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }

            if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] == PAREDE) {
                jogo.deltaMovF1 *= -1;
                if (fb.fantasmaGulao) {
                    fb.fantasmaGulao = 0;
                    jogo.fb = fb;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                    jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] = GHOST1;
                }

                if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] == COMIDA) {
                    temp.x = coord.x;
                    temp.y = coord.y - jogo.deltaMovF1;
                    jogo.aux1 = temp;
                    fb.fantasmaGulao = 1;
                    jogo.fb = fb;
                    jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] = GHOST1;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] == VAZIO) {
                    jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] = GHOST1;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }
        }
    }
    return jogo;
}

// movimenta o fantasma P
tJogo Fantasma2Pmovimento (tJogo jogo) {
    tCoordenada temp;   tFantasma2P fp;     tPacman pac;
    temp = jogo.aux2;   fp = jogo.fp;       pac = jogo.pacman;

    tCoordenada coord;
    if (jogo.fp.presenteNoJogo) {
        coord = IdentificaCoordenada(jogo, GHOST2);
        if (coord.x - jogo.deltaMovF2 >= 0 && coord.x + jogo.deltaMovF2 < jogo.nLinhas) {

            if (jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] != PAREDE) {
                 if (jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] == VAZIO) {
                    jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] = GHOST2;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] == PACMAN) {
                    jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] = GHOST2;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                    pac.qtdVida--;
                    jogo.pacman = pac;
                }

                if (fp.fantasmaGulao) {
                    fp.fantasmaGulao = 0;
                    jogo.fp = fp;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                }

                if (jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] == COMIDA) {
                    temp.x = coord.x - jogo.deltaMovF2;
                    temp.y = coord.y;
                    jogo.aux2 = temp;
                    fp.fantasmaGulao = 1;
                    jogo.fp = fp;
                    jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] = GHOST2;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }

            if (jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] == PAREDE) {
                jogo.deltaMovF2 *= -1;
                if (fp.fantasmaGulao) {
                    fp.fantasmaGulao = 0;
                    jogo.fp = fp;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                    jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] = GHOST2;
                }

                if (jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] == COMIDA) {
                    temp.x = coord.x - jogo.deltaMovF2;
                    temp.y = coord.y;
                    jogo.aux2 = temp;
                    fp.fantasmaGulao = 1;
                    jogo.fp = fp;
                    jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] = GHOST2;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] == VAZIO) {
                    jogo.mapa[coord.x - jogo.deltaMovF2][coord.y] = GHOST2;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }
        }
    }
    return jogo;
}

// movimenta o fantasma I
tJogo Fantasma3Imovimento (tJogo jogo) {
    tCoordenada temp;   tFantasma3I fi;     tPacman pac;
    temp = jogo.aux3;   fi = jogo.fi;       pac = jogo.pacman;

    tCoordenada coord;
    if (jogo.fi.presenteNoJogo) {
        coord = IdentificaCoordenada(jogo, GHOST3);
        if (coord.x - jogo.deltaMovF3 >= 0 && coord.x + jogo.deltaMovF3 < jogo.nLinhas) {

            if (jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] != PAREDE) {
                 if (jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] == VAZIO) {
                    jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] = GHOST3;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] == PACMAN) {
                    jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] = GHOST3;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                    pac.qtdVida--;
                    jogo.pacman = pac;
                }

                if (fi.fantasmaGulao) {
                    fi.fantasmaGulao = 0;
                    jogo.fi = fi;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                }

                if (jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] == COMIDA) {
                    temp.x = coord.x + jogo.deltaMovF3;
                    temp.y = coord.y;
                    jogo.aux3 = temp;
                    fi.fantasmaGulao = 1;
                    jogo.fi = fi;
                    jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] = GHOST3;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }

            if (jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] == PAREDE) {
                jogo.deltaMovF3 *= -1;
                if (fi.fantasmaGulao) {
                    fi.fantasmaGulao = 0;
                    jogo.fi = fi;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                    jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] = GHOST3;
                }

                if (jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] == COMIDA) {
                    temp.x = coord.x + jogo.deltaMovF3;
                    temp.y = coord.y;
                    jogo.aux3 = temp;
                    fi.fantasmaGulao = 1;
                    jogo.fi = fi;
                    jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] = GHOST3;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] == VAZIO) {
                    jogo.mapa[coord.x + jogo.deltaMovF3][coord.y] = GHOST3;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }
        }
    }
    return jogo;
}

// movimenta o fantasma C
tJogo Fantasma4Cmovimento (tJogo jogo) {
    tCoordenada temp;   tFantasma4C fc;     tPacman pac;
    temp = jogo.aux4;   fc = jogo.fc;       pac = jogo.pacman;

    tCoordenada coord;
    if (jogo.fc.presenteNoJogo) {
        coord = IdentificaCoordenada(jogo, GHOST4);
        if (coord.y - jogo.deltaMovF4 >= 0 && coord.y + jogo.deltaMovF4 < jogo.nColunas) {

            if (jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] != PAREDE) {
                 if (jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] == VAZIO) {
                    jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] = GHOST4;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] == PACMAN) {
                    jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] = GHOST4;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                    pac.qtdVida--;
                    jogo.pacman = pac;
                }

                if (fc.fantasmaGulao) {
                    fc.fantasmaGulao = 0;
                    jogo.fc = fc;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                }

                if (jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] == COMIDA) {
                    temp.x = coord.x; 
                    temp.y = coord.y + jogo.deltaMovF4;
                    jogo.aux4 = temp;
                    fc.fantasmaGulao = 1;
                    jogo.fc = fc;
                    jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] = GHOST4;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }

            if (jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] == PAREDE) {
                jogo.deltaMovF4 *= -1;
                if (fc.fantasmaGulao) {
                    fc.fantasmaGulao = 0;
                    jogo.fc = fc;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                    jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] = GHOST4;
                }

                if (jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] == COMIDA) {
                    temp.x = coord.x;
                    temp.y = coord.y + jogo.deltaMovF4;
                    jogo.aux4 = temp;
                    fc.fantasmaGulao = 1;
                    jogo.fc = fc;
                    jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] = GHOST4;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] == VAZIO) {
                    jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] = GHOST4;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }
        }
    }
    return jogo;
}

// retorna um inteiro para manipulacao e checagem se o pacman esta vivo ou nao
int RetornaVidaPACMAN (tJogo jogo) {
    tPacman pac;
    pac = jogo.pacman;
    return pac.qtdVida;
}

// retorna quantos pontos o pacman realizou ate o momento
int RetornaPontuacao (tJogo jogo) {
    tEstatisticas s;
    s = jogo.stats;
    return s.pacmanComeu;
}

// imprime a mensagem de padrao game over e a pontuacao na tela
void RetornaGameOver (tJogo jogo) {
    printf("GAME OVER!\n");
    printf("Pontuação final: %d\n", RetornaPontuacao(jogo));
}

// funcao principal, realiza todo o jogo por aqui
void RealizaJogo(tJogo jogo) {
    tEstatisticas status;
    status = jogo.stats; 
    int nMov = 0;
    int pontos = 0;

    char acao;
    jogo = IdentificaFantasmasExistentes(jogo);
    while (scanf("%c%*c", &acao) == 1) {
        if (nMov == jogo.qtdJogadas) {
            RetornaGameOver(jogo);
            exit(1);
        }
        
        printf("Estado do jogo apos o movimento '%c':\n", acao);
        jogo = EfetuaJogadaPacman(jogo, acao); 
        nMov++;

        jogo = EfetuaJogadaFantasma(jogo);

        if (!RetornaVidaPACMAN(jogo)) {
            RetornaGameOver(jogo);
            exit(1);
        }

        ImprimeMapa(jogo);
        printf("\nPontuacao: %d\n\n", RetornaPontuacao(jogo));

        if (RetornaPontuacao(jogo) == jogo.qtdComida) break;
    }
            
    printf("Voce venceu!");
    printf("\nPontuação final: %d\n", RetornaPontuacao(jogo));
}
