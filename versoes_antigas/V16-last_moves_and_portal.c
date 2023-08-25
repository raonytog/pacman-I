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
    char mapaPac[MAX_LINHA][MAX_COLUNA];
    char mapaFant[MAX_LINHA][MAX_COLUNA];
} tJogo;

//dump de funcoes, explicacoes sobre cada logo abaixo da main
tJogo LeMapa (char *diretorio);
tCoordenada IdentificaCoordenada(tJogo jogo, char s);
void ImprimeMapa (tJogo jogo);
void ImprimeMapaNoDiretorio (tJogo jogo, FILE * fDiretorio);
tJogo InicializaJogo (tJogo jogo, char *diretorio);
void RealizaJogo(tJogo jogo);
tJogo EfetuaJogadaPacman (tJogo jogo, char acao);
int RetornaVidaPACMAN (tJogo jogo);
int RetornaPontuacao (tJogo jogo);
int EhFantasma(tJogo jogo, int l, int c);
void RetornaGameOver (tJogo jogo);
tJogo InicializaFantasmas(tJogo jogo);
tJogo MoveFantasmaB (tJogo jogo);
tJogo MoveFantasmaP (tJogo jogo);
tJogo MoveFantasmaI (tJogo jogo);
tJogo MoveFantasmaC (tJogo jogo);
tJogo AnalisaCoordenadasGameOver(tJogo jogo);
tJogo VerificaPosicaoPacEComida(tJogo jogo);   

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
    mapinha = InicializaJogo(mapinha, diretorio);
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
            if (jogo.mapa[i][j] == PACMAN) {
                jogo.mapa[i][j] = VAZIO;
                jogo.mapaPac[i][j] = PACMAN;
            } else if (jogo.mapa[i][j] == GHOST1 ||
                        jogo.mapa[i][j] == GHOST2 || 
                        jogo.mapa[i][j] == GHOST3 ||
                        jogo.mapa[i][j] == GHOST4) {
                jogo.mapaFant[i][j] = jogo.mapa[i][j];
                jogo.mapa[i][j] = VAZIO;
            }
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

    if (s == PACMAN) {
        for (i = 0; i < jogo.nLinhas; i++) {
            for (j = 0; j < jogo.nColunas; j++) {
                if (jogo.mapaPac[i][j] == s) {
                    coord.x = i;
                    coord.y = j;
                    return coord;   
                }
            }
        }
    } else {
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
}

// imprime o mapa salvo na estrutura do jogo
void ImprimeMapa (tJogo jogo) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {

            if (jogo.mapaPac[i][j] == PACMAN) {
                printf("%c", jogo.mapaPac[i][j]);
            } else if (EhFantasma(jogo, i, j)) {
                printf("%c", jogo.mapaFant[i][j]);
            } else {
                printf("%c", jogo.mapa[i][j]);
            }
        }
        printf("\n");
    }
}

void ImprimeMapaNoDiretorio (tJogo jogo, FILE * fDiretorio) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {

            if (jogo.mapaPac[i][j] == PACMAN) {
                fprintf(fDiretorio, "%c", jogo.mapaPac[i][j]);
            } else if (EhFantasma(jogo, i, j)) {
                fprintf(fDiretorio, "%c", jogo.mapaFant[i][j]);
            } else {
                fprintf(fDiretorio, "%c", jogo.mapa[i][j]);
            }
        }
        fprintf(fDiretorio, "\n");
    }
}

// inicializa o jogo, dando as informacoes de linha e coluna inicial na saida
// inicializa as variaveis do jogo, tal como as estatisticas
// A IMPLEMENTAR saida de arquivos
tJogo InicializaJogo (tJogo jogo, char *diretorio) {
    FILE * fInicializacao = NULL;
    char dirInicializacao[DIR_TAM_MAX];
    sprintf(dirInicializacao, "%s/saida/inicializacao.txt", diretorio);
    fInicializacao = fopen(dirInicializacao, "w");

    if (!fInicializacao) {
        printf("ERRO ao tentar abrir o arquivo de saida/inicializacao!\n");
        exit(1);
    }

    ImprimeMapaNoDiretorio(jogo, fInicializacao);
    tCoordenada coord;
    coord = IdentificaCoordenada(jogo, PACMAN);
    fprintf(fInicializacao, "Pac-Man comecara o jogo na linha %d e coluna %d\n", coord.x + 1, coord.y + 1);

    fclose(fInicializacao);

    jogo.gameover = 0;       jogo.qtdComida = 0;
    jogo.deltaMovF1 = -1;    jogo.deltaMovF2 = -1;
    jogo.deltaMovF3 = 1;     jogo.deltaMovF4 = 1;
    jogo.deltaMovP = 1;

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if(jogo.mapa[i][j] == COMIDA) jogo.qtdComida++;
        }
    }

    tEstatisticas status;
    status.nColisoesComParede = 0;      status.nMovimentosSemPontos = 0;
    status.nMovimentoBaixo = 0;         status.nMovimentosCima = 0;
    status.nMovimentoEsquerda = 0;      status.nMovimentoDireita = 0;
    status.pacmanComeu = 0;             jogo.stats = status;

    tPacman pac;
    pac.qtdVida = 1;
    pac.cordPac = coord;
    jogo.pacman = pac;
    return jogo;
}

// retorna true se encontra um fantasma na coordenada indicada
int EhFantasma(tJogo jogo, int l, int c) {
    if (jogo.mapaFant[l][c] == GHOST1 ||
        jogo.mapaFant[l][c] == GHOST2 ||
        jogo.mapaFant[l][c] == GHOST3 ||
        jogo.mapaFant[l][c] == GHOST4) {
            return 1;
    }

    return 0;
}

// realiza as jogadas do pacman de acordo com a tecla pressionada
tJogo EfetuaJogadaPacman (tJogo jogo, char acao) {
    tCoordenada coord;  coord = IdentificaCoordenada(jogo, PACMAN);
    int x = coord.x;    int y = coord.y;

    tPacman pac;            pac = jogo.pacman;
    tEstatisticas stats;    stats = jogo.stats;

    switch (acao) {
    case 'w':
        if (x - 1 >= 0 && jogo.mapa[x-1][y] != PAREDE) {  
            jogo.mapaPac[x][y] = VAZIO;
            jogo.mapaPac[x-1][y] = PACMAN;
            coord.x = x-1;
            coord.y = y;
            pac.cordPac = coord;
            jogo.pacman = pac;
        }
        break;

    case 's':
        if (x + 1 < jogo.nLinhas && jogo.mapa[x+1][y] != PAREDE) {  
            jogo.mapaPac[x][y] = VAZIO;
            jogo.mapaPac[x+1][y] = PACMAN;
            coord.x = x+1;
            coord.y = y;
            pac.cordPac = coord;
            jogo.pacman = pac;
        }
        break; 

    case 'a':
        if (y - 1 >= 0 && jogo.mapa[x][y-1] != PAREDE) {
            jogo.mapaPac[x][y] = VAZIO;
            jogo.mapaPac[x][y-1] = PACMAN;
            coord.x = x;
            coord.y = y-1;
            pac.cordPac = coord;
            jogo.pacman = pac;
        }
        break; 

    case 'd':
        if (y + 1 < jogo.nColunas && jogo.mapa[x][y+1] != PAREDE) {
            jogo.mapaPac[x][y] = VAZIO;
            jogo.mapaPac[x][y+1] = PACMAN;
            coord.x = x;
            coord.y = y+1;
            pac.cordPac = coord;
            jogo.pacman = pac;
        }
        break; 

    default:
        break;
    }
    return jogo;
}

// indica quais fantasmas existem no mapa
tJogo InicializaFantasmas(tJogo jogo) {

    tFantasma1B fb;         tFantasma2P fp;
    fb = jogo.fb;            fp = jogo.fp;
    tCoordenada coordB;     tCoordenada coordP;

    tFantasma3I fi;         tFantasma4C fc;
    fi = jogo.fi;           fc = jogo.fc;
    tCoordenada coordI;     tCoordenada coordC;

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapaFant[i][j] ==  GHOST1) {
                fb.presenteNoJogo = 1;
                fb.fantasmaGulao = 0;
                coordB.x = i;   coordB.y = j;
                fb.coordFantB = coordB;
                jogo.fb = fb;
            }

            if (jogo.mapaFant[i][j] ==  GHOST2) {
                fp.presenteNoJogo = 1;
                coordP.x = i;
                coordP.y = j;
                fp.coordFantP = coordP;
                fp.fantasmaGulao = 0;
                jogo.fp = fp;
            }

            if (jogo.mapaFant[i][j] ==  GHOST3) {
                fi.presenteNoJogo = 1;
                coordI.x = i;
                coordI.y = j;
                fi.coordFantI = coordI;
                fi.fantasmaGulao = 0;
                jogo.fi = fi;
            }

            if (jogo.mapaFant[i][j] ==  GHOST4) {
                fc.presenteNoJogo = 1;
                coordC.x = i;
                coordC.y = j;
                fc.coordFantC = coordC;
                fc.fantasmaGulao = 0;
                jogo.fc = fc;
            }
        }
    }

    return jogo;
}

tJogo MoveFantasmaB (tJogo jogo) {
    tFantasma1B fb;
    fb = jogo.fb;

    tCoordenada coordb;
    coordb = fb.coordFantB;
    int x = coordb.x;
    int y = coordb.y;
    int dy = jogo.deltaMovF1;
    
    if (fb.presenteNoJogo) {
        if (jogo.mapa[x][y+dy] != PAREDE) {
            jogo.mapaFant[x][y+dy] = GHOST1;
            jogo.mapaFant[x][y] = VAZIO;
        } else {
            dy *= -1;
            jogo.mapaFant[x][y+dy] = GHOST1;
            jogo.mapaFant[x][y] = VAZIO;
        }
    }

    coordb.x = x;
    coordb.y = y + dy;
    fb.coordFantB = coordb;
    jogo.fb = fb;
    jogo.deltaMovF1 = dy;
    return jogo;
}

tJogo MoveFantasmaP (tJogo jogo) {
    tFantasma2P fp;
    fp = jogo.fp;

    tCoordenada coordp;
    coordp = fp.coordFantP;
    int x = coordp.x;
    int y = coordp.y;
    int dy = jogo.deltaMovF2;
    
    if (fp.presenteNoJogo) {
        if (jogo.mapa[x+dy][y] != PAREDE) {
            jogo.mapaFant[x+dy][y] = GHOST2;
            jogo.mapaFant[x][y] = VAZIO;
        } else {
            dy *= -1;
            jogo.mapaFant[x+dy][y] = GHOST2;
            jogo.mapaFant[x][y] = VAZIO;
        }
    }

    coordp.x = x+dy;
    coordp.y = y;
    fp.coordFantP = coordp;
    jogo.fp = fp;
    jogo.deltaMovF2 = dy;
    return jogo;
}

tJogo MoveFantasmaI (tJogo jogo) {
    tFantasma3I fi;
    fi = jogo.fi;

    tCoordenada coordi;
    coordi = fi.coordFantI;
    int x = coordi.x;
    int y = coordi.y;
    int dy = jogo.deltaMovF3;
    
    if (fi.presenteNoJogo) {
        if (jogo.mapa[x+dy][y] != PAREDE) {
            jogo.mapaFant[x+dy][y] = GHOST3;
            jogo.mapaFant[x][y] = VAZIO;
        } else {
            dy *= -1;
            jogo.mapaFant[x+dy][y] = GHOST3;
            jogo.mapaFant[x][y] = VAZIO;
        }
    }

    coordi.x = x+dy;
    coordi.y = y;
    fi.coordFantI = coordi;
    jogo.fi = fi;
    jogo.deltaMovF3 = dy;
    return jogo;
}

tJogo MoveFantasmaC (tJogo jogo) {
    tFantasma4C fc;
    fc = jogo.fc;

    tCoordenada coordc;
    coordc = fc.coordFantC;
    int x = coordc.x;
    int y = coordc.y;
    int dy = jogo.deltaMovF4;
    
    if (fc.presenteNoJogo) {
        if (jogo.mapa[x][y+dy] != PAREDE) {
            jogo.mapaFant[x][y+dy] = GHOST4;
            jogo.mapaFant[x][y] = VAZIO;
        } else {
            dy *= -1;
            jogo.mapaFant[x][y+dy] = GHOST4;
            jogo.mapaFant[x][y] = VAZIO;
        }
    }

    coordc.x = x;
    coordc.y = y + dy;
    fc.coordFantC = coordc;
    jogo.fc = fc;
    jogo.deltaMovF4 = dy;
    return jogo;
}

tJogo AnalisaCoordenadasGameOver(tJogo jogo) {
    tPacman pac;            tCoordenada cpac;
    pac = jogo.pacman;      cpac = pac.cordPac;

    tFantasma1B fb;         tCoordenada cfb;
    fb = jogo.fb;           cfb = fb.coordFantB;

    tFantasma2P fp;         tCoordenada cfp;
    fp = jogo.fp;           cfp = fp.coordFantP;

    tFantasma3I fi;         tCoordenada cfi;
    fi = jogo.fi;           cfi = fi.coordFantI;
    
    tFantasma4C fc;         tCoordenada cfc;
    fc = jogo.fc;           cfc = fc.coordFantC;

    if (cpac.x == cfb.x && cpac.y == cfb.y || 
        cpac.x == cfp.x && cpac.y == cfp.y ||
        cpac.x == cfi.x && cpac.y == cfi.y ||
        cpac.x == cfc.x && cpac.y == cfc.y) {
            pac.qtdVida--;
            jogo.pacman = pac;
    }

    return jogo;
}

tJogo VerificaPosicaoPacEComida(tJogo jogo) {
    tPacman pac;            tCoordenada cpac;
    pac = jogo.pacman;      cpac = pac.cordPac;

    tEstatisticas estatistica;
    estatistica = jogo.stats;

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if(jogo.mapa[i][j] == COMIDA && jogo.mapaPac[i][j] == PACMAN) {
                jogo.mapa[i][j] = VAZIO;
                estatistica.pacmanComeu++;
            }
        }
    }
    jogo.stats = estatistica;
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
    tEstatisticas status;   status = jogo.stats; 
    int nMov = 0;
    jogo = InicializaFantasmas(jogo);
    char acao;

    while (scanf("%c%*c", &acao) == 1) {
        printf("Estado do jogo apos o movimento '%c':\n", acao);
        jogo = EfetuaJogadaPacman(jogo, acao); nMov++; 

        jogo = MoveFantasmaB(jogo);     
        jogo = MoveFantasmaP(jogo);
        jogo = MoveFantasmaI(jogo);
        jogo = MoveFantasmaC(jogo);

        if (nMov == jogo.qtdJogadas) {
            RetornaGameOver(jogo);
            exit(1);
        }

        jogo = AnalisaCoordenadasGameOver(jogo);
        if (!RetornaVidaPACMAN(jogo)) {
            RetornaGameOver(jogo);
            exit(1);
        }

        jogo = VerificaPosicaoPacEComida(jogo);

        ImprimeMapa(jogo);
        printf("Pontuacao: %d\n\n", RetornaPontuacao(jogo));

        if (RetornaPontuacao(jogo) == jogo.qtdComida) break;
    }
            
    printf("Voce venceu!");
    printf("\nPontuação final: %d\n", RetornaPontuacao(jogo));
}
