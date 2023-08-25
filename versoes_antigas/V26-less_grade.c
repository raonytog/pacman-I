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
    int nMovimentoCima;
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
    tCoordenada coordAntiga;
    tCoordenada cordPac;
    int qtdVida;
} tPacman;

// estrutura para manipular o fantasma de de acordo:
//  * se comeu comida;
//  * se esta no jogo;
//  * qtd de vida;
//  * coordenadas;
typedef struct {
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordAntiga;
    tCoordenada coordFantB;
} tFantasma1B;

// mesma do tFantasma1B
typedef struct {
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordAntiga;
    tCoordenada coordFantP;
} tFantasma2P;

// mesma do tFantasma1B
typedef struct {
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordAntiga;
    tCoordenada coordFantI;
} tFantasma3I;

// mesma do tFantasma1B
typedef struct {
    int fantasmaGulao;
    int presenteNoJogo;
    int qtdVida;
    tCoordenada coordAntiga;
    tCoordenada coordFantC;
} tFantasma4C;

//estrutura geral do jogo
typedef struct {
    tEstatisticas stats;
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
    int flagResumo;
    char mapa[MAX_LINHA][MAX_COLUNA];
    char mapaPac[MAX_LINHA][MAX_COLUNA];
    char mapaFant[MAX_LINHA][MAX_COLUNA];
    int mapaTrilha[MAX_LINHA][MAX_COLUNA];
    int idJogadaTrilha;
} tJogo;

//dump de funcoes, explicacoes sobre cada logo abaixo da main
tJogo LeMapa (char *diretorio);
tCoordenada IdentificaCoordenada(tJogo jogo, char s);
void ImprimeMapa (tJogo jogo);
void ImprimeMapaNoDiretorio (tJogo jogo, FILE * fDiretorio);
tJogo InicializaJogo (tJogo jogo, char *diretorio);
tJogo InicializaVariaveis(tJogo jogo);
tJogo InicializaQtdComida(tJogo jogo);
tJogo InicializaEstatisticas(tJogo jogo);
tJogo InicializaPacman (tJogo jogo, tCoordenada coord);
void RealizaJogo(tJogo jogo, char * diretorio);
tJogo EfetuaJogadaPacman (tJogo jogo, char acao);
int RetornaVidaPACMAN (tJogo jogo);
int RetornaPontuacao (tJogo jogo);
int EhFantasma(tJogo jogo, int l, int c);
tJogo InicializaFantasmas(tJogo jogo);
tJogo MoveFantasmaB (tJogo jogo);
tJogo MoveFantasmaP (tJogo jogo);
tJogo MoveFantasmaI (tJogo jogo);
tJogo MoveFantasmaC (tJogo jogo);
tJogo AnalisaCoordenadasGameOver(tJogo jogo);
tJogo VerificaPosicaoPacEComida(tJogo jogo);   
tCoordenada EncontraOutroPortal(tJogo jogo, int x, int y);
void SaidaArquivoEstatisticas(tJogo jogo, char * diretorio, int qtdMov);

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
    RealizaJogo(mapinha, diretorio);
    
    return 0;
}

tJogo EfetuaTrilha (tJogo jogo, int nthJogada) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapaPac[i][j] == PACMAN) {
                jogo.mapaTrilha[i][j] = nthJogada;
            } 
        }
    }
    return jogo;
}

tJogo InicializaTrilha (tJogo jogo) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            jogo.mapaTrilha[i][j] = -1;
        }
    }
    return jogo;
}

void RetornaTrilha(tJogo jogo, char * diretorio) {
    char trilhaDir[DIR_TAM_MAX];
    sprintf(trilhaDir, "%s/saida/trilha.txt", diretorio);

    FILE * fTrilha = NULL;
    fTrilha = fopen(trilhaDir, "w");
    
    if (!fTrilha) {
        printf("Diretorio para saida da trilha nao valido");
        exit(1);
    }

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapaTrilha[i][j] == -1) {
                fprintf(fTrilha, "# ");
            } else {
                fprintf(fTrilha, "%d ", jogo.mapaTrilha[i][j]);
            }
        }
        fprintf(fTrilha, "\n");
    }
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

    //comea a trilha
    jogo = InicializaTrilha(jogo);
    jogo = EfetuaTrilha(jogo, 0);
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

// imprime o mapa completo, tal como os fantasmas, o pacman, os portais, as comidas e as paredes
// que estao separadas por matrizes diferentes
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

// mesma coisa do imprime mapa, mas em um diretorio
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

// inicializa as variaveis uteis ao desenrolar do jogo
tJogo InicializaVariaveis(tJogo jogo) {
    jogo.gameover = 0;       jogo.qtdComida = 0;
    jogo.deltaMovF1 = -1;    jogo.deltaMovF2 = -1;
    jogo.deltaMovF3 = 1;     jogo.deltaMovF4 = 1;
    jogo.deltaMovP = 1;      jogo.flagResumo = 0;
    return jogo;
}

// inicializa a variavel para contagem da qtd decomida no mapa
tJogo InicializaQtdComida(tJogo jogo) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if(jogo.mapa[i][j] == COMIDA) jogo.qtdComida++;
        }
    }
    return jogo;
}

// inicializa as variaveis de estatisticas para 0
tJogo InicializaEstatisticas(tJogo jogo) {
    tEstatisticas status;
    status.nColisoesComParede = 0;      status.nMovimentosSemPontos = 0;
    status.nMovimentoBaixo = 0;         status.nMovimentoCima = 0;
    status.nMovimentoEsquerda = 0;      status.nMovimentoDireita = 0;
    status.pacmanComeu = 0;
    jogo.stats = status;
    return jogo;
}

// inicializa as variaveis gerais do pacman no inicio do jogo
tJogo InicializaPacman (tJogo jogo, tCoordenada coord) {
    tPacman pac;    pac.qtdVida = 1;    pac.cordPac = coord;    jogo.pacman = pac;
    return jogo;
}

// inicializa o jogo, gera arquivo de saida de inicializacao e configura as variaveis com os valores
// que devem conter para o inicio do jogo
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

    jogo = InicializaVariaveis(jogo);
    jogo = InicializaQtdComida(jogo);
    jogo = InicializaEstatisticas(jogo);
    jogo = InicializaPacman(jogo, coord);
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

tCoordenada EncontraOutroPortal(tJogo jogo, int x, int y) {
    tCoordenada coord;

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if (jogo.mapa[i][j] == PORTAL && (i != x || j != y) ) {
                coord.x = i;
                coord.y = j;
            }
        }
    }
    return coord;
}

tJogo EfetuaResumo (tJogo jogo, char * diretorio, int nMov, char acao) {
    char dirResumo[DIR_TAM_MAX];
    sprintf(dirResumo, "%s/saida/resumo.txt", diretorio);
    
    FILE * fResumo;
    fResumo = fopen(dirResumo, "a+");
    if (!fResumo) {
        printf("Nao abriu o resumo");
        exit(1);
    }

    switch (jogo.flagResumo) {
    case 1:
        fprintf(fResumo, "Movimento %d (%c) pegou comida\n", nMov, acao);
        jogo.flagResumo = 0;
        break;

    case 2:
        fprintf(fResumo, "Movimento %d (%c) colidiu na parede\n", nMov, acao);
        jogo.flagResumo = 0;
        break;

    case 3:
        fprintf(fResumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", nMov, acao);
        jogo.flagResumo = 0;
        break;
    
    default:
        break;
    }

    fclose(fResumo);
    return jogo;
}

// realiza as jogadas do pacman de acordo com a tecla pressionada
tJogo EfetuaJogadaPacman (tJogo jogo, char acao) {
    tCoordenada coord;      coord = IdentificaCoordenada(jogo, PACMAN);
    int x = coord.x;        int y = coord.y;

    tPacman pac;            pac = jogo.pacman;
    tEstatisticas stats;    stats = jogo.stats;

    tCoordenada coordPortal;

    switch (acao) {
    case 'w':
        stats.nMovimentoCima++;
        if (x - 1 >= 0) {

            if (jogo.mapa[x][y] == PORTAL && jogo.mapa[x-1][y] == PAREDE) {
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                break;
            }

            else if (jogo.mapa[x-1][y] == PAREDE) { 
                stats.nColisoesComParede++;
                jogo.flagResumo = 1;
            
            } else if (jogo.mapa[x-1][y] == PORTAL) {
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x-1, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;

            } else {
                jogo.mapaPac[x][y] = VAZIO;
                jogo.mapaPac[x-1][y] = PACMAN;
                coord.x = x-1;
                coord.y = y;
            }
        }
        break;

    case 's':
        stats.nMovimentoBaixo++;
        if (x + 1 < jogo.nLinhas) {

            if (jogo.mapa[x][y] == PORTAL && jogo.mapa[x+1][y] == PAREDE) {
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                break;

            } else if (jogo.mapa[x+1][y] == PAREDE) { 
                stats.nColisoesComParede++;
                jogo.flagResumo = 1;

            } else if  (jogo.mapa[x+1][y] == PORTAL) {
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x+1, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;

            } else {
                jogo.mapaPac[x][y] = VAZIO;
                jogo.mapaPac[x+1][y] = PACMAN;
                coord.x = x+1;
                coord.y = y;
            }
        }
        break; 

    case 'a':
        stats.nMovimentoEsquerda++;
        if (y - 1 >= 0) {

            if (jogo.mapa[x][y] == PORTAL && jogo.mapa[x][y-1] == PAREDE) {
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                break;

            } else if (jogo.mapa[x][y-1] == PAREDE) {
                stats.nColisoesComParede++;

            } else if  (jogo.mapa[x][y-1] == PORTAL) {
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y-1);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                jogo.flagResumo = 1;

            } else {
                jogo.mapaPac[x][y] = VAZIO;
                jogo.mapaPac[x][y-1] = PACMAN;
                coord.x = x;
                coord.y = y-1;
            }
        }
        break; 

    case 'd':
        stats.nMovimentoDireita++;
        if (y + 1 < jogo.nColunas) {

            if (jogo.mapa[x][y] == PORTAL && jogo.mapa[x][y+1] == PAREDE) {
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                break;
            
            } else if (jogo.mapa[x][y+1] == PAREDE) {
                stats.nColisoesComParede++;
                jogo.flagResumo = 1;

            } else if  (jogo.mapa[x][y+1] == PORTAL) {
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y+1);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;

            } else {
                jogo.mapaPac[x][y] = VAZIO;
                jogo.mapaPac[x][y+1] = PACMAN;
                coord.x = x;
                coord.y = y+1;
            }
        }
        break; 
    }

    jogo.stats = stats;
    pac.cordPac = coord;
    jogo.pacman = pac;
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

// move fantasmaB e armazena sua nova coordenada
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

    tCoordenada antiga;
    antiga.x = x;   antiga.y = y;
    fb.coordAntiga = antiga;

    coordb.x = x;   coordb.y = y + dy;
    fb.coordFantB = coordb;

    jogo.fb = fb;
    jogo.deltaMovF1 = dy;
    return jogo;
}

// move fantasmaP e armazena sua nova coordenada
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

    tCoordenada antiga;
    antiga.x = x;   antiga.y = y;
    fp.coordAntiga = antiga;

    coordp.x = x + dy;   coordp.y = y;
    fp.coordFantP = coordp;
    
    jogo.fp = fp;
    jogo.deltaMovF2 = dy;
    return jogo;
}

// move fantasmaI e armazena sua nova coordenada
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

    tCoordenada antiga;
    antiga.x = x;   antiga.y = y;
    fi.coordAntiga = antiga;

    coordi.x = x + dy;   coordi.y = y;
    fi.coordFantI = coordi;
    
    jogo.fi = fi;
    jogo.deltaMovF3 = dy;
    return jogo;
}

// move fantasmaC e armazena sua nova coordenada
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

    tCoordenada antiga;
    antiga.x = x;   antiga.y = y;
    fc.coordAntiga = antiga;

    coordc.x = x;   coordc.y = y+dy;
    fc.coordFantC = coordc;
    
    jogo.fc = fc;
    jogo.deltaMovF4 = dy;
    return jogo;
}

// verifica se o pacman e o fantasma estao na mesma casa ou andaram em direcoes opostas
tJogo AnalisaCoordenadasGameOver(tJogo jogo) {
    tPacman pac;            tCoordenada cpac;
    pac = jogo.pacman;      cpac = pac.cordPac;
    // tCoordenada antigapac;  antigapac = pac.coordAntiga;

    tFantasma1B fb;         tCoordenada cfb;
    fb = jogo.fb;           cfb = fb.coordFantB;
    // tCoordenada antigafb;   antigafb = fb.coordAntiga;

    tFantasma2P fp;         tCoordenada cfp;
    fp = jogo.fp;           cfp = fp.coordFantP;
    // tCoordenada antigafp;   antigafp = fp.coordAntiga;

    tFantasma3I fi;         tCoordenada cfi;
    fi = jogo.fi;           cfi = fi.coordFantI;
    // tCoordenada antigafi;   antigafi = fi.coordAntiga;
    
    tFantasma4C fc;         tCoordenada cfc;
    fc = jogo.fc;           cfc = fc.coordFantC;
    // tCoordenada antigafc;   antigafc = fc.coordAntiga;

    if (cpac.x == cfb.x && cpac.y == cfb.y || 
        cpac.x == cfp.x && cpac.y == cfp.y ||
        cpac.x == cfi.x && cpac.y == cfi.y ||
        cpac.x == cfc.x && cpac.y == cfc.y) {
            jogo.mapaPac[cpac.x][cpac.y] = VAZIO;
            pac.qtdVida--;
            jogo.flagResumo = 3;
            jogo.pacman = pac;
    }

    return jogo;
}

// retorna estatisticas se o pacman comeu ou nao a comida no movimento
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
            } else if (jogo.mapa[i][j] == VAZIO && jogo.mapaPac[i][j] == PACMAN) {
                estatistica.nMovimentosSemPontos++;
            }
        }
    }
    jogo.flagResumo = 2;
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

// joga as estatisticas para o arquivo de estatisticas na saida
void SaidaArquivoEstatisticas(tJogo jogo, char *diretorio, int qtdMov) {
    tEstatisticas stats;
    stats = jogo.stats;
    char dirEstatisticas[DIR_TAM_MAX];
    sprintf(dirEstatisticas, "%s/saida/estatisticas.txt", diretorio);

    FILE *fEstatistica = NULL;
    fEstatistica = fopen(dirEstatisticas, "w");

    if (!fEstatistica) {
        printf("Arquivo %s nao foi aberto.", dirEstatisticas);
        exit(1);
    }

    fprintf(fEstatistica, "Numero de movimentos: %d\n", qtdMov);
    fprintf(fEstatistica, "Numero de movimentos sem pontuar: %d\n", qtdMov - stats.pacmanComeu);
    fprintf(fEstatistica, "Numero de colisoes com parede: %d\n", stats.nColisoesComParede);
    fprintf(fEstatistica, "Numero de movimentos para baixo: %d\n", stats.nMovimentoBaixo);
    fprintf(fEstatistica, "Numero de movimentos para cima: %d\n", stats.nMovimentoCima);
    fprintf(fEstatistica, "Numero de movimentos para esquerda: %d\n", stats.nMovimentoEsquerda);
    fprintf(fEstatistica, "Numero de movimentos para direita: %d\n", stats.nMovimentoDireita);

    fclose(fEstatistica);
}

// funcao principal, realiza todo o jogo por aqui
void RealizaJogo(tJogo jogo, char * diretorio) {
    tEstatisticas status;   status = jogo.stats; 
    int nMov = 0, flag = 0;
    jogo = InicializaFantasmas(jogo);
    char acao;

    while (scanf("%c%*c", &acao) == 1) {

        printf("Estado do jogo apos o movimento '%c':\n", acao);
        jogo = EfetuaJogadaPacman(jogo, acao);  nMov++; 
        jogo = EfetuaTrilha(jogo, nMov);

        jogo = MoveFantasmaB(jogo);     
        jogo = MoveFantasmaP(jogo);
        jogo = MoveFantasmaI(jogo);
        jogo = MoveFantasmaC(jogo);

        jogo = AnalisaCoordenadasGameOver(jogo);
        if (!RetornaVidaPACMAN(jogo)) {
            ImprimeMapa(jogo);
            printf("Pontuacao: %d\n\n", RetornaPontuacao(jogo));

            printf("Game over!\n");
            printf("Pontuacao final: %d\n", RetornaPontuacao(jogo));

            if (jogo.flagResumo) {
                jogo = EfetuaResumo(jogo, diretorio, nMov, acao);
                jogo.flagResumo = 0;
                printf("aq1");
            }

            SaidaArquivoEstatisticas(jogo, diretorio, nMov);
            RetornaTrilha(jogo, diretorio);
            exit(1);
        }

        jogo = VerificaPosicaoPacEComida(jogo);
        if (jogo.flagResumo) {
                jogo = EfetuaResumo(jogo, diretorio, nMov, acao);
                jogo.flagResumo = 0;
        }

        ImprimeMapa(jogo);
        printf("Pontuacao: %d\n\n", RetornaPontuacao(jogo));

        if (nMov == jogo.qtdJogadas) {
            printf("Game over!\n");
            printf("Pontuacao final: %d\n", RetornaPontuacao(jogo));
            SaidaArquivoEstatisticas(jogo, diretorio, nMov);
            RetornaTrilha(jogo, diretorio);
            exit(1);
            break;
        }

        if (RetornaPontuacao(jogo) == jogo.qtdComida) break;
    }


    printf("Voce venceu!\n");
    printf("Pontuacao final: %d\n", RetornaPontuacao(jogo));
    SaidaArquivoEstatisticas(jogo, diretorio, nMov);
    RetornaTrilha(jogo, diretorio);
}
