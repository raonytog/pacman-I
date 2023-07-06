#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// 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

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
    int qtdComidaJogo;
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
    int flag;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantB;
} tFantasma1B;

typedef struct {
    char g2;
    int flag;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantP;
} tFantasma2P;

typedef struct {
    char g3;
    int flag;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantI;
} tFantasma3I;

typedef struct {
    char g4;
    int flag;
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordFantC;
} tFantasma4C;

//estrutura para manipular os fantasmas e suas informacoes
typedef struct {   
    tFantasma1B fantB;
    tFantasma2P fantP;
    tFantasma3I fantI;
    tFantasma4C fantC;
} tFantasmas;

//estrutura geral do jogo
typedef struct {
    tEstatisticas stats;    
    tFantasmas fantasmas;   
    tPacman pacman;
    int dmov;   // a qtd de avancos que ele faz por jogada (delta movimento)
    int gameover;   // define o fim do jogo
    int qtdComida;  // diz qnts comidas tem no jogo
    int qtdJogadas; // diz qnts jogadas o pacman pode ter na partida
    int nLinhas;    // num de linhas presentes na matriz do mapa
    int nColunas;   // num de colunas presentes na matriz do mapa
    char mapa[MAX_LINHA][MAX_COLUNA];
} tJogo;

//dump de funcoes implementadas 
tJogo LeMapa (char *diretorio);
tJogo EfetuaJogadaPacman (tJogo jogo, char acao, tEstatisticas stats);
tJogo EfetuaJogadaFantasmas (tJogo jogo);
tCoordenada IdentificaCoordenadaPACMAN (tJogo jogo);
tCoordenada IdentificaCoordenadaFantasma (tJogo jogo, char s);
tFantasmas IdentificaFantasmasPresentes (tJogo jogo);
tEstatisticas ContabilizaNumComidasNoJogo (tJogo jogo); 
tEstatisticas RetornaStats(tJogo jogo);
void ImprimeMapa (tJogo jogo);
void RealizaJogo(tJogo jogo);
void InicializaJogo (tJogo jogo);

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
    InicializaJogo(mapinha);
    RealizaJogo(mapinha);

    return 0;
}

//funcao q recebe e le o arquivo de entrada do mapa p salvar no struct tJogo
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
    jogo.stats = ContabilizaNumComidasNoJogo(jogo);
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

tEstatisticas ContabilizaNumComidasNoJogo (tJogo jogo) {
    int i, j;
    tEstatisticas stats;
    stats.qtdComidaJogo = 0;
    stats.pacmanComeu = 0;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapa[i][j] == COMIDA) {
                stats.qtdComidaJogo++;
            }
        }
    }

    return stats;
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
void RealizaJogo(tJogo jogo) {
    char aux;
    tPacman pac; pac.qtdVida = 1;
    tFantasmas fants;
    tEstatisticas stats;
    jogo.dmov = 1;
    stats = RetornaStats(jogo);

    jogo.gameover = 0;
    jogo.stats = stats;
    jogo.pacman = pac;
    jogo.fantasmas = fants;

    while (scanf("%c%*c", &aux) == 1 && jogo.stats.pacmanComeu != jogo.stats.qtdComidaJogo) {
        jogo = EfetuaJogadaPacman(jogo, aux, jogo.stats);
        fants = IdentificaFantasmasPresentes(jogo);
        jogo.fantasmas = fants;
        jogo = EfetuaJogadaFantasmas(jogo);

        printf("\nEstado do jogo após o movimento '%c':\n", aux);
        if (jogo.gameover) {
            printf("GAME OVER!\n");
            printf("Pontuação final: %d\n", jogo.stats.pacmanComeu);
            exit(1);
        }

        ImprimeMapa(jogo);
        printf("\nPontuação: %d\n", jogo.stats.pacmanComeu);
    }

    printf("Voce venceu!\n");
    printf("\nPontuação final: %d\n", jogo.stats.pacmanComeu);

}

//funcao para realizar as jogadas 
tJogo EfetuaJogadaPacman (tJogo jogo, char acao, tEstatisticas stats) {
    int linha, coluna; char aux;
    tCoordenada coord;
    coord = IdentificaCoordenadaPACMAN(jogo);


    tPacman pac;
    pac.cordPac = coord;

    switch (acao) {
    case 'w':
        if (coord.x - 1 >= 0 && 
            jogo.mapa[coord.x-1][coord.y] != PAREDE) {  
                if (jogo.mapa[coord.x-1][coord.y] == COMIDA) {
                    stats.pacmanComeu++;
                    jogo.stats = stats;
                }

                if (jogo.mapa[coord.x-1][coord.y] == GHOST1 || 
                    jogo.mapa[coord.x-1][coord.y] == GHOST2 ||
                    jogo.mapa[coord.x-1][coord.y] == GHOST3 ||
                    jogo.mapa[coord.x-1][coord.y] == GHOST4) {
                        pac.qtdVida--;
                        jogo.pacman = pac;
                        return jogo;
                    }

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

                if (jogo.mapa[coord.x+1][coord.y] == GHOST1 || 
                    jogo.mapa[coord.x+1][coord.y] == GHOST2 ||
                    jogo.mapa[coord.x+1][coord.y] == GHOST3 ||
                    jogo.mapa[coord.x+1][coord.y] == GHOST4) {
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

                if (jogo.mapa[coord.x][coord.y-1] == GHOST1 || 
                    jogo.mapa[coord.x][coord.y-1] == GHOST2 ||
                    jogo.mapa[coord.x][coord.y-1] == GHOST3 ||
                    jogo.mapa[coord.x][coord.y-1] == GHOST4) {
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

                if (jogo.mapa[coord.x][coord.y+1] == GHOST1 || 
                    jogo.mapa[coord.x][coord.y+1] == GHOST2 ||
                    jogo.mapa[coord.x][coord.y+1] == GHOST3 ||
                    jogo.mapa[coord.x][coord.y+1] == GHOST4) {
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
        printf("\nJogada invalida.\nJogadas validas: a, d, s, w\n");
        return jogo;
        break;
    }
}

tJogo EfetuaJogadaFantasmas (tJogo jogo) {
    tPacman pac;        tFantasmas fant;
    pac = jogo.pacman;  fant = jogo.fantasmas;

    tFantasma1B fb;     tFantasma2P fp;     tFantasma3I fi;     tFantasma4C fc;
    fb = fant.fantB;    fp = fant.fantP;    fi = fant.fantI;    fc = fant.fantC;
    printf("%d AUAUHA", fant.fantB.fantasmaGulao);

    tCoordenada coord;
    if (fb.presenteNoJogo) {
        coord = fb.coordFantB;
                //if (coord.y - jogo.dmov >= 0 || coord.y + jogo.dmov < jogo.nLinhas) {
                    if (jogo.mapa[coord.x][coord.y - jogo.dmov] != PAREDE) {
                        if (jogo.mapa[coord.x][coord.y - jogo.dmov] == PACMAN) {
                            pac.qtdVida--;
                            jogo.pacman = pac;
                            jogo.gameover = 1;
                        }

                        jogo.mapa[coord.x][coord.y-jogo.dmov] = GHOST1;

                        if (fb.fantasmaGulao) {
                            jogo.mapa[coord.x][coord.y] = '*';
                        }
                        
                        printf("%d %d\n\n", coord.x, coord.y - jogo.dmov);
                        if (jogo.mapa[coord.x][coord.y - jogo.dmov] == COMIDA) {
                            fb.fantasmaGulao = 1;
                            printf("%d oaiaiaia", fb.fantasmaGulao);
                            fant.fantB = fb;
                            jogo.fantasmas = fant;
                        }

                        jogo.mapa[coord.x][coord.y] = ' ';

                    } else if (jogo.mapa[coord.x][coord.y - jogo.dmov] == PAREDE) {
                        fb.flag = 0;
                        jogo.dmov *= -1;
                        fant.fantB = fb;
                        jogo.fantasmas = fant;
                        jogo.mapa[coord.x][coord.y - jogo.dmov] = GHOST1;
                        jogo.mapa[coord.x][coord.y] = ' ';
                    }
            //}   
    }
    return jogo;

}


tEstatisticas RetornaStats(tJogo jogo) {
    tEstatisticas s;
    s = jogo.stats;
    return s;
}

// retorna se 1 para a estrutura de cada fantasma 
// caso esteja presente no jogo 
tFantasmas IdentificaFantasmasPresentes(tJogo jogo) {
    tFantasmas fant; tCoordenada coord;

    tFantasma1B f1; coord = f1.coordFantB;
    tFantasma2P f2; coord = f2.coordFantP;
    tFantasma3I f3; coord = f3.coordFantI;
    tFantasma4C f4; coord = f4.coordFantC;

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapa[i][j] ==  GHOST1) {
                f1.presenteNoJogo = 1;
                f1.fantasmaGulao = 9999;
                fant.fantB = f1;
                coord.x = i; coord.y = j;
                f1.coordFantB = coord;
                fant.fantB = f1;
            }

            if (jogo.mapa[i][j] ==  GHOST2) {
                f2.presenteNoJogo = 1;
                f2.flag = 1;
                fant.fantP = f2;
                coord.x = i; coord.y = j;
                f2.coordFantP = coord;
                fant.fantP = f2;
            }

            if (jogo.mapa[i][j] ==  GHOST3) {
                f3.presenteNoJogo = 1;
                f3.flag = 1;
                fant.fantI = f3;
                coord.x = i; coord.y = j;
                f3.coordFantI = coord;
                fant.fantI = f3;
            }

            if (jogo.mapa[i][j] ==  GHOST4) {
                f4.presenteNoJogo = 1;
                f4.flag = 1;
                fant.fantC = f4;
                coord.x = i; coord.y = j;
                f4.coordFantC = coord;
                fant.fantC = f4;
            }
        }
    }

    return fant;
}

// funcao para identificar a linha q o pacman esta no comeco do jogo
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

// funcao para retornar na struct de coordenadas onde esta localizado o fantasma
tCoordenada IdentificaCoordenadaFantasma (tJogo jogo, char s) {
    tCoordenada coord;
    int i, j;

    if (s == GHOST1) {
        for (i = 0; i < jogo.nLinhas; i++) {
            for (j = 0; j < jogo.nColunas; j++) {
                if (jogo.mapa[i][j] == GHOST1) {
                    coord.x = i;
                    coord.y = j;
                    return coord;   
                }
            }
        }
    }

    if (s == GHOST2) {
        for (i = 0; i < jogo.nLinhas; i++) {
            for (j = 0; j < jogo.nColunas; j++) {
                if (jogo.mapa[i][j] == GHOST2) {
                    coord.x = i;
                    coord.y = j;
                    return coord;   
                }
            }
        }
    }

    if (s == GHOST3) {
        for (i = 0; i < jogo.nLinhas; i++) {
            for (j = 0; j < jogo.nColunas; j++) {
                if (jogo.mapa[i][j] == GHOST3) {
                    coord.x = i;
                    coord.y = j;
                    return coord;   
                }
            }
        }
    }

    if (s == GHOST4) {
        for (i = 0; i < jogo.nLinhas; i++) {
            for (j = 0; j < jogo.nColunas; j++) {
                if (jogo.mapa[i][j] == GHOST4) {
                    coord.x = i;
                    coord.y = j;
                    return coord;   
                }
            }
        }
    }
}