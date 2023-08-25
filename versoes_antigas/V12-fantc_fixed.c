#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIR_TAM_MAX 1001
#define MAX_LINHA 41
#define MAX_COLUNA 101

//defines do conteudo do mapa
#define VAZIO  ' '   //vazio
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

//estrutura para manipular o pacman e a qtd de vida
typedef struct {
    tCoordenada cordPac;
    int qtdVida;
} tPacman;

typedef struct {
    char g1;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantB;
} tFantasma1B;

typedef struct {
    char g2;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantP;
} tFantasma2P;

typedef struct {
    char g3;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantI;
} tFantasma3I;

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
    tFantasma1B fb;
    tFantasma2P fp;
    tFantasma3I fi;
    tFantasma4C fc;
    tCoordenada aux;
    tPacman pacman;
    int deltaMovF1;  
    int deltaMovF2;
    int deltaMovF3;
    int deltaMovF4;
    int deltaMovP;   // a qtd de avancos q o pacman faz por round
    int gameover;   // define o fim do jogo
    int qtdComida;  // diz qnts comidas tem no jogo
    int qtdJogadas; // diz qnts jogadas o pacman pode ter na partida
    int nLinhas;    // num de linhas presentes na matriz do mapa
    int nColunas;   // num de colunas presentes na matriz do mapa
    char mapa[MAX_LINHA][MAX_COLUNA];
} tJogo;

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

void ImprimeMapa (tJogo jogo) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        if (i != 0) printf("\n");
        for (j = 0; j < jogo.nColunas; j++) {
            printf("%c", jogo.mapa[i][j]);
        }
    }
}

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

int EhFantasma(tJogo jogo, int l, int c) {
    if (jogo.mapa[l][c] == GHOST1 ||
        jogo.mapa[l][c] == GHOST2 ||
        jogo.mapa[l][c] == GHOST3 ||
        jogo.mapa[l][c] == GHOST4) {
            return 1;
        }

    return 0;
}

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

tJogo EfetuaJogadaFantasma (tJogo jogo) {
    tCoordenada temp;
    temp = jogo.aux;

    tPacman pac;
    pac = jogo.pacman;

    tFantasma1B fb;
    fb = jogo.fb;

    tFantasma2P fp;
    fp = jogo.fp;

    tFantasma3I fi;
    fi = jogo.fi;

    tFantasma4C fc;
    fc = jogo.fc;

    tCoordenada coord;
    if (jogo.fb.presenteNoJogo) {
        coord = IdentificaCoordenada(jogo, GHOST1);
        if (coord.y - jogo.deltaMovF1 >= 0 || coord.y + jogo.deltaMovF1 < jogo.nLinhas) {

            if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] != PAREDE) {
                 if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] == VAZIO) {
                    jogo.mapa[coord.x][coord.y-jogo.deltaMovF1] = GHOST1;
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
                    jogo.aux = temp;
                    fb.fantasmaGulao = 1;
                    jogo.fb = fb;
                    jogo.mapa[coord.x][coord.y-jogo.deltaMovF1] = GHOST1;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }

            if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] == PAREDE) {
                jogo.deltaMovF1 *= -1;
                jogo.mapa[coord.x][coord.y - jogo.deltaMovF1] = GHOST1;
                jogo.mapa[coord.x][coord.y] = VAZIO;
            }
        }
    }

    //implementar
    if (jogo.fp.presenteNoJogo) {
        coord = IdentificaCoordenada(jogo, GHOST2);
        if (coord.y - jogo.deltaMovF2 >= 0 || coord.y + jogo.deltaMovF2 < jogo.nLinhas) {

            if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF2] != PAREDE) {
                 if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF2] == VAZIO) {
                    jogo.mapa[coord.x][coord.y-jogo.deltaMovF2] = GHOST2;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF2] == PACMAN) {
                    jogo.mapa[coord.x][coord.y-jogo.deltaMovF2] = GHOST2;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                    pac.qtdVida--;
                    jogo.pacman = pac;
                    
                }
                if (fb.fantasmaGulao) {
                    fb.fantasmaGulao = 0;
                    jogo.fp = fp;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                }

                if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF2] == COMIDA) {
                    temp.x = coord.x; 
                    temp.y = coord.y - jogo.deltaMovF2;
                    jogo.aux = temp;
                    fb.fantasmaGulao = 1;
                    jogo.fb = fb;
                    jogo.mapa[coord.x][coord.y-jogo.deltaMovF2] = GHOST2;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }

            if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF2] == PAREDE) {
                jogo.deltaMovF2 *= -1;
                jogo.mapa[coord.x][coord.y-jogo.deltaMovF2] = GHOST2;
                jogo.mapa[coord.x][coord.y] = VAZIO;
            }
        }
    }
    
    //implementar 
    if (jogo.fi.presenteNoJogo) {
        coord = IdentificaCoordenada(jogo, GHOST3);
        if (coord.y - jogo.deltaMovF3 >= 0 || coord.y + jogo.deltaMovF3 < jogo.nLinhas) {

            if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF3] != PAREDE) {
                 if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF3] == VAZIO) {
                    jogo.mapa[coord.x][coord.y - jogo.deltaMovF3] = GHOST3;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }

                if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF3] == PACMAN) {
                    jogo.mapa[coord.x][coord.y - jogo.deltaMovF3] = GHOST3;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                    pac.qtdVida--;
                    jogo.pacman = pac;
                    
                }
                if (fb.fantasmaGulao) {
                    fb.fantasmaGulao = 0;
                    jogo.fi = fi;
                    jogo.mapa[temp.x][temp.y] = COMIDA;
                }

                if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF3] == COMIDA) {
                    temp.x = coord.x; 
                    temp.y = coord.y - jogo.deltaMovF3;
                    jogo.aux = temp;
                    fi.fantasmaGulao = 1;
                    jogo.fi = fi;
                    jogo.mapa[coord.x][coord.y-jogo.deltaMovF3] = GHOST3;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }

            if (jogo.mapa[coord.x][coord.y - jogo.deltaMovF3] == PAREDE) {
                jogo.deltaMovF3 *= -1;
                jogo.mapa[coord.x][coord.y-jogo.deltaMovF3] = GHOST3;
                jogo.mapa[coord.x][coord.y] = VAZIO;
            }
        }
    }

    if (jogo.fc.presenteNoJogo) {
        coord = IdentificaCoordenada(jogo, GHOST4);
        if (coord.y - jogo.deltaMovF4 >= 0 || coord.y + jogo.deltaMovF4 < jogo.nLinhas) {

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
                    jogo.aux = temp;
                    fc.fantasmaGulao = 1;
                    jogo.fc = fc;
                    jogo.mapa[coord.x][coord.y+jogo.deltaMovF4] = GHOST4;
                    jogo.mapa[coord.x][coord.y] = VAZIO;
                }
            }

            if (jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] == PAREDE) {
                jogo.deltaMovF4 *= -1;
                jogo.mapa[coord.x][coord.y + jogo.deltaMovF4] = GHOST4;
                jogo.mapa[coord.x][coord.y] = VAZIO;
            }
        }
    }

    return jogo;
}

int RetornaVidaPACMAN (tJogo jogo) {
    tPacman pac;
    pac = jogo.pacman;
    return pac.qtdVida;
}

int RetornaPontuacao (tJogo jogo) {
    tEstatisticas s;
    s = jogo.stats;
    return s.pacmanComeu;
}

void RetornaGameOver (tJogo jogo) {
    printf("GAME OVER!\n");
    printf("Pontuação final: %d\n", RetornaPontuacao(jogo));
}

void RealizaJogo(tJogo jogo) {
    tEstatisticas status;
    status = jogo.stats; 
    int nMov = 0;

    char acao;
    jogo = IdentificaFantasmasExistentes(jogo);
    while (scanf("%c%*c", &acao) == 1 && 
           RetornaPontuacao(jogo) != jogo.qtdComida) {

        if (nMov == jogo.qtdJogadas) {
            RetornaGameOver(jogo);
            exit(1);
        }

        jogo = EfetuaJogadaPacman(jogo, acao); nMov++;
        RetornaVidaPACMAN(jogo);
        jogo = EfetuaJogadaFantasma(jogo);

        if (!RetornaVidaPACMAN(jogo)) {
            RetornaGameOver(jogo);
            exit(1);
        }

        ImprimeMapa(jogo);
        printf("\nPontuacao: %d\n\n", RetornaPontuacao(jogo));
    }
    
    printf("\nVoce venceu!");
    printf("\nPontuação final: %d\n", RetornaPontuacao(jogo));
}
