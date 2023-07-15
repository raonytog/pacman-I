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

typedef struct {
    int qtdComida;
    int qtdColisoes;
    int qtdUsos;
    char charW;
} tMovW;

typedef struct {
    int qtdComida;
    int qtdColisoes;
    int qtdUsos;
    char charS;
} tMovS;

typedef struct {
    int qtdComida;
    int qtdColisoes;
    int qtdUsos;
    char charA;
} tMovA;

typedef struct {
    int qtdComida;
    int qtdColisoes;
    int qtdUsos;
    char charD;
} tMovD;

typedef struct {
    int qtdComida;
    int qtdColisoes;
    int qtdUsos;
    char charGen;
} tMovimentos;

//estrutura geral do jogo
typedef struct {
    tEstatisticas stats;
    tFantasma1B fb;
    tFantasma2P fp;
    tFantasma3I fi;
    tFantasma4C fc;
    tPacman pacman;
    tMovW mw;
    tMovS ms;
    tMovA ma;
    tMovD md;
    int deltaMovF1;     // qtd de avancos do fantasma 1 (B)
    int deltaMovF2;     // qtd de avancos do fantasma 2 (P)
    int deltaMovF3;     // qtd de avancos do fantasma 3 (I)
    int deltaMovF4;     // qtd de avancos do fantasma 4 (C)
    int deltaMovP;      // qtd de avancos do pacman
    int gameover;       // define se eh ou nao fim de jogo
    int qtdComida;      // diz qnts comidas tem inicialmente no mapa
    int qtdJogadas;     // diz qnts jogadas o pacman pode ter na partida
    int nLinhas;        // num de linhas presentes na matriz do mapa
    int nColunas;       // num de colunas presentes na matriz do mapa
    int flagResumo;     // flag para dizer acao q vai executar no resumo
    char mapa[MAX_LINHA][MAX_COLUNA];
    char mapaPac[MAX_LINHA][MAX_COLUNA];
    char mapaFant[MAX_LINHA][MAX_COLUNA];
    int mapaTrilha[MAX_LINHA][MAX_COLUNA];
    int idJogadaTrilha;
} tJogo;

//dump de funcoes, explicacoes sobre cada logo abaixo da main
tJogo LeMapa (char *diretorio);
tJogo InicializaVariaveis(tJogo jogo);
tJogo InicializaQtdComida(tJogo jogo);
tJogo InicializaEstatisticas(tJogo jogo);
tJogo InicializaPacman (tJogo jogo, tCoordenada coord);
tJogo InicializaJogo (tJogo jogo, char *diretorio);
tJogo InicializaFantasmas(tJogo jogo);
tJogo MoveFantasmaB (tJogo jogo);
tJogo MoveFantasmaP (tJogo jogo);
tJogo MoveFantasmaI (tJogo jogo);
tJogo MoveFantasmaC (tJogo jogo);
tJogo EfetuaJogadaPacman (tJogo jogo, char acao, int nMov); 
tJogo AnalisaCoordenadasGameOver(tJogo jogo);
tJogo VerificaPosicaoPacEComida(tJogo jogo, char acao);   
tJogo InicializaTrilha (tJogo jogo);
tJogo EfetuaTrilha (tJogo jogo, int nthJogada);
tJogo EfetuaResumo (tJogo jogo, char * diretorio, int nMov, char acao);
tJogo InicializaTMovimentos(tJogo jogo);
tCoordenada IdentificaCoordenada(tJogo jogo, char s);
tCoordenada EncontraOutroPortal(tJogo jogo, int x, int y);
void ImprimeMapa (tJogo jogo);
void ImprimeMapaNoDiretorio (tJogo jogo, FILE * fDiretorio);
void RealizaJogo(tJogo jogo, char * diretorio);
void SaidaArquivoEstatisticas(tJogo jogo, char * diretorio, int qtdMov);
void RetornaTrilha(tJogo jogo, char * diretorio);
void RetornaRanking(tJogo jogo, char * diretorio);
int CriterioUso (tMovimentos m, tMovimentos n);
int CriterioColisao(tMovimentos m, tMovimentos n);
int CriteriosRanking (tMovimentos m, tMovimentos n); 
int RetornaVidaPACMAN (tJogo jogo);
int RetornaPontuacao (tJogo jogo);
int CriterioAlfabetico (tMovimentos m, tMovimentos n);
int EhFantasma(tJogo jogo, int l, int c);


int main (int argc, char * argv[]) {
    if (argc <= 1) {
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 1;
    }

    char diretorio[DIR_TAM_MAX];
    sprintf(diretorio, "%s", argv[1]);

    // realiza o jogo
    tJogo mapinha;
    tEstatisticas status;
    mapinha = LeMapa(diretorio);
    mapinha = InicializaJogo(mapinha, diretorio);
    RealizaJogo(mapinha, diretorio);
    return 0;
}

// inicializa a matriz da trilha com -1
tJogo InicializaTrilha (tJogo jogo) {
    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            jogo.mapaTrilha[i][j] = -1;
        }
    }
    return jogo;
}

// preenche o arquivo de trilha ao longo do jogo
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

// retorna o arquvio da trila
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

    // armazerna as informacoes lidas no arquvio de mapa.txt
    fscanf(fMapa, "%d", &jogo.nLinhas);
    fscanf(fMapa, "%d", &jogo.nColunas);
    fscanf(fMapa, "%d", &jogo.qtdJogadas);

    // preenche o mapa do geral, dos fantasmas e do pacman inicilamente
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
    if (s == PACMAN) {  //se for pacman
        for (i = 0; i < jogo.nLinhas; i++) {
            for (j = 0; j < jogo.nColunas; j++) {
                if (jogo.mapaPac[i][j] == s) {
                    coord.x = i;
                    coord.y = j;
                    return coord;   
                }
            }
        }
    } else {    // qlqr outro objeto no mapa, como fantasmas, comida...
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

// imprime o mapa completo, independete das matrizes separadas
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

// mesma coisa do imprime mapa, mas em um diretorio, se necessario
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

// encontra a localizacao do portal em que o pacman nao esta sobreposto
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

    if (jogo.flagResumo == 1) { // comeu comida
        fprintf(fResumo, "Movimento %d (%c) pegou comida\n", nMov, acao);
    } else if (jogo.flagResumo == 2) {  //colidiu parede
        fprintf(fResumo, "Movimento %d (%c) colidiu na parede\n", nMov, acao);
    } else if (jogo.flagResumo == 3) {  // morre p fantasma
        fprintf(fResumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", nMov, acao);
    } else if (jogo.flagResumo == 4) {  //colisao e morte 
        fprintf(fResumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", nMov, acao);
        fprintf(fResumo, "Movimento %d (%c) colidiu na parede\n", nMov, acao);
    }

    jogo.flagResumo = 0;
    fclose(fResumo);
    return jogo;
}

// realiza as jogadas do pacman de acordo com a tecla pressionada
tJogo EfetuaJogadaPacman (tJogo jogo, char acao, int nMov) {
    tCoordenada coord;      coord = IdentificaCoordenada(jogo, PACMAN);
    int x = coord.x;        int y = coord.y;

    tCoordenada antigapac;

    tPacman pac;            pac = jogo.pacman;
    tEstatisticas stats;    stats = jogo.stats;

    tMovA mova;             mova = jogo.ma;
    tMovD movd;             movd = jogo.md;
    tMovS movs;             movs = jogo.ms;
    tMovW movw;             movw = jogo.mw;

    tCoordenada coordPortal;

    switch (acao) {
    case 'w':
        stats.nMovimentoCima++;
        movw.qtdUsos++;
        if (x - 1 >= 0) {

            // se bateu na parede e esta em cima do portal
            if (jogo.mapa[x][y] == PORTAL && jogo.mapa[x-1][y] == PAREDE) {
                jogo = EfetuaTrilha(jogo, nMov);
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                break;
            }

            else if (jogo.mapa[x-1][y] == PAREDE) {     // se bateu na parede
                stats.nColisoesComParede++;
                jogo.flagResumo = 2;
                movw.qtdColisoes++;
            
            } else if (jogo.mapa[x-1][y] == PORTAL) {   // se bateu no portal
                jogo = EfetuaTrilha(jogo, nMov);
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x-1, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;

            } else {     // se for vazio ou comida
                jogo.mapaPac[x][y] = VAZIO;
                jogo.mapaPac[x-1][y] = PACMAN;
                coord.x = x-1;
                coord.y = y;
            }
        }
        break;

    case 's':
        stats.nMovimentoBaixo++;
        movs.qtdUsos++;
        if (x + 1 < jogo.nLinhas) {

            // se bateu na parede e esta em cima do portal
            if (jogo.mapa[x][y] == PORTAL && jogo.mapa[x+1][y] == PAREDE) {
                jogo = EfetuaTrilha(jogo, nMov);
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                break;

            } else if (jogo.mapa[x+1][y] == PAREDE) {   // se bateu na parede
                stats.nColisoesComParede++;
                jogo.flagResumo = 2;
                movs.qtdColisoes++;

            } else if  (jogo.mapa[x+1][y] == PORTAL) {  // se bateu no portal
                jogo = EfetuaTrilha(jogo, nMov);
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x+1, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;

            } else {     // se for vazio ou comida
                jogo.mapaPac[x][y] = VAZIO;
                jogo.mapaPac[x+1][y] = PACMAN;
                coord.x = x+1;
                coord.y = y;
            }
        }
        break; 

    case 'a':
        stats.nMovimentoEsquerda++;
        mova.qtdUsos++;
        if (y - 1 >= 0) {

            // se bateu na parede e esta em cima do portal
            if (jogo.mapa[x][y] == PORTAL && jogo.mapa[x][y-1] == PAREDE) {
                jogo = EfetuaTrilha(jogo, nMov);
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                break;

            } else if (jogo.mapa[x][y-1] == PAREDE) {    // se bateu na parede
                stats.nColisoesComParede++;
                jogo.flagResumo = 2;
                mova.qtdColisoes++;

            } else if  (jogo.mapa[x][y-1] == PORTAL) {  // se bateu no portal
                jogo = EfetuaTrilha(jogo, nMov);
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y-1);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;

            } else {    // se for vazio ou comida
                jogo.mapaPac[x][y] = VAZIO;
                jogo.mapaPac[x][y-1] = PACMAN;
                coord.x = x;
                coord.y = y-1;
            }
        }
        break; 

    case 'd':
        stats.nMovimentoDireita++;
        movd.qtdUsos++;
        if (y + 1 < jogo.nColunas) {

            // se bateu na parede e esta em cima do portal
            if (jogo.mapa[x][y] == PORTAL && jogo.mapa[x][y+1] == PAREDE) {
                jogo = EfetuaTrilha(jogo, nMov);
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;
                break;
            
            } else if (jogo.mapa[x][y+1] == PAREDE) {   // se bateu na parede
                stats.nColisoesComParede++;
                jogo.flagResumo = 2;
                movd.qtdColisoes++;

            } else if  (jogo.mapa[x][y+1] == PORTAL) {  // se bateu no portal
                jogo = EfetuaTrilha(jogo, nMov);
                jogo.mapaPac[x][y] = VAZIO;
                coordPortal = EncontraOutroPortal(jogo, x, y+1);
                jogo.mapaPac[coordPortal.x][coordPortal.y] = PACMAN;
                coord.x = coordPortal.x;    coord.y = coordPortal.y;

            } else {    // se for vazio ou comida
                jogo.mapaPac[x][y] = VAZIO;
                jogo.mapaPac[x][y+1] = PACMAN;
                coord.x = x;
                coord.y = y+1;
            }
        }
        break; 
    }


    jogo.ma = mova;     jogo.md = movd;
    jogo.ms = movs;     jogo.mw = movw;

    jogo = EfetuaTrilha(jogo, nMov);
    jogo.stats = stats;
    antigapac.x = x;
    antigapac.y = y;
    pac.coordAntiga = antigapac;
    pac.cordPac = coord;
    jogo.pacman = pac;
    return jogo;
}

// indica fantasmas presentes e suas coordenadas iniciais
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
    tPacman pac;            tCoordenada cpac;           tCoordenada antigapac;
    pac = jogo.pacman;      cpac = pac.cordPac;         antigapac = pac.coordAntiga;

    tFantasma1B fb;         tCoordenada cfb;            tCoordenada antigafb;
    fb = jogo.fb;           cfb = fb.coordFantB;        antigafb = fb.coordAntiga;

    tFantasma2P fp;         tCoordenada cfp;            tCoordenada antigafp;
    fp = jogo.fp;           cfp = fp.coordFantP;        antigafp = fp.coordAntiga;

    tFantasma3I fi;         tCoordenada cfi;            tCoordenada antigafi;
    fi = jogo.fi;           cfi = fi.coordFantI;        antigafi = fi.coordAntiga;

    tFantasma4C fc;         tCoordenada cfc;            tCoordenada antigafc;
    fc = jogo.fc;           cfc = fc.coordFantC;        antigafc = fc.coordAntiga;

        //se se sobreporem
    if (cpac.x == cfb.x && cpac.y == cfb.y || 
        cpac.x == cfp.x && cpac.y == cfp.y ||
        cpac.x == cfi.x && cpac.y == cfi.y ||
        cpac.x == cfc.x && cpac.y == cfc.y || 
        
        //se andarem em direcoes opostas qnd cara-a-cara
        cpac.x == antigafb.x && cpac.y == antigafb.y   &&   antigapac.x == cfb.x && antigapac.y == cfb.y ||
        cpac.x == antigafp.x && cpac.y == antigafp.y   &&   antigapac.x == cfp.x && antigapac.y == cfp.y ||
        cpac.x == antigafi.x && cpac.y == antigafi.y   &&   antigapac.x == cfi.x && antigapac.y == cfi.y ||
        cpac.x == antigafc.x && cpac.y == antigafc.y   &&   antigapac.x == cfc.x && antigapac.y == cfc.y ) {
            jogo.mapaPac[cpac.x][cpac.y] = VAZIO;

            if (jogo.flagResumo == 2) {
                jogo.flagResumo = 4;
            } else jogo.flagResumo = 3;
            pac.qtdVida--;
            jogo.pacman = pac;
    }
    return jogo;
}

// retorna estatisticas se o pacman comeu ou nao a comida no movimento
tJogo VerificaPosicaoPacEComida(tJogo jogo, char acao) {
    tPacman pac;            tCoordenada cpac;
    pac = jogo.pacman;      cpac = pac.cordPac;

    tMovA mova;             mova = jogo.ma;
    tMovD movd;             movd = jogo.md;
    tMovS movs;             movs = jogo.ms;
    tMovW movw;             movw = jogo.mw;

    tEstatisticas estatistica;
    estatistica = jogo.stats;

    int i, j;
    for (i = 0; i < jogo.nLinhas; i++) {
        for (j = 0; j < jogo.nColunas; j++) {
            if(jogo.mapa[i][j] == COMIDA && jogo.mapaPac[i][j] == PACMAN) {
                jogo.mapa[i][j] = VAZIO;
                estatistica.pacmanComeu++;
                if (acao == 'w') {
                    movw.qtdComida++;
                } else if (acao == 's') {
                    movs.qtdComida++;
                } else if (acao == 'a') {
                    mova.qtdComida++;
                } else if (acao == 'd') {
                    movd.qtdComida++;
                }

                jogo.flagResumo = 1;
            } else if (jogo.mapa[i][j] == VAZIO && jogo.mapaPac[i][j] == PACMAN) {
                estatistica.nMovimentosSemPontos++;
            }
        }
    }

    jogo.ma = mova;     jogo.md = movd;
    jogo.ms = movs;     jogo.mw = movw;
    jogo.stats = estatistica;
    return jogo;
}

// retorna um inteiro para manipulacao e checagem se o pacman esta vivo ou nao
int RetornaVidaPACMAN (tJogo jogo) {
    tPacman pac;    pac = jogo.pacman;
    return pac.qtdVida;
}

// retorna quantos pontos o pacman realizou ate o momento
int RetornaPontuacao (tJogo jogo) {
    tEstatisticas s;    s = jogo.stats;
    return s.pacmanComeu;
}

// joga as estatisticas para o arquivo de estatisticas na saida
void SaidaArquivoEstatisticas(tJogo jogo, char *diretorio, int qtdMov) {
    tEstatisticas stats;     stats = jogo.stats;
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

int CriterioAlfabetico (tMovimentos m, tMovimentos n) {
    return m.charGen < n.charGen;
}

int CriterioUso (tMovimentos m, tMovimentos n) {
    if (m.qtdUsos > n.qtdUsos) {
        return 1;
    } else if (m.qtdUsos == n.qtdUsos) {
        return CriterioAlfabetico(m, n);
    }
    return 0;
}

int CriterioColisao(tMovimentos m, tMovimentos n) {
    if (m.qtdColisoes < n.qtdColisoes) {
        return 1;
    } else if (m.qtdColisoes == n.qtdColisoes) {
        return CriterioUso(m, n);
    }
    return 0;
}

int CriteriosRanking (tMovimentos m, tMovimentos n) {
    if (m.qtdComida > n.qtdComida) {
        return 1;
    } else if (m.qtdComida == n.qtdComida) {
        return CriterioColisao(m, n);
    }
    return 0;
}

// retorna o arquvio de ranking 
void RetornaRanking(tJogo jogo, char * diretorio) {
    char dirRanking[DIR_TAM_MAX];
    sprintf(dirRanking, "%s/saida/ranking.txt", diretorio);

    FILE * fRanking = NULL;
    fRanking = fopen(dirRanking, "w");

    if (!fRanking) {
        printf("Erro ao abrir arquivo de ranking");
        exit(1);
    }

    tMovA mova;             mova = jogo.ma;
    tMovD movd;             movd = jogo.md;
    tMovS movs;             movs = jogo.ms;
    tMovW movw;             movw = jogo.mw;

    tMovimentos mov[4];     tMovimentos aux;

    mov[0].charGen = 'a';                           mov[1].charGen = 's';
    mov[0].qtdColisoes = mova.qtdColisoes;          mov[1].qtdColisoes = movs.qtdColisoes;
    mov[0].qtdComida = mova.qtdComida;              mov[1].qtdComida = movs.qtdComida;
    mov[0].qtdUsos = mova.qtdUsos;                  mov[1].qtdUsos = movs.qtdUsos;

    mov[2].charGen = 'd';                           mov[3].charGen = 'w';
    mov[2].qtdColisoes = movd.qtdColisoes;          mov[3].qtdColisoes = movw.qtdColisoes; 
    mov[2].qtdComida = movd.qtdComida;              mov[3].qtdComida = movw.qtdComida;
    mov[2].qtdUsos = movd.qtdUsos;                  mov[3].qtdUsos = movw.qtdUsos;

    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = i+1; j < 4; j++) {
            if(CriteriosRanking(mov[i], mov[j])) {
                aux = mov[j];
                mov[j] = mov[i];
                mov[i] = aux;
            }
       }
    }

    for (i = 3; i >= 0; i--) {
        fprintf(fRanking, "%c,%d,%d,%d\n", mov[i].charGen, mov[i].qtdComida, mov[i].qtdColisoes, mov[i].qtdUsos);
    }
    fclose(fRanking);
}

// inicializa a contagem das structs dos movimentos
tJogo InicializaTMovimentos(tJogo jogo) {
    tMovA mova;                 tMovD movd;
    mova.qtdColisoes = 0;       movd.qtdColisoes = 0;
    mova.qtdComida = 0;         movd.qtdComida = 0;
    mova.qtdUsos = 0;           movd.qtdUsos = 0;
    mova.charA = 'a';           movd.charD = 'd';
    jogo.ma = mova;             jogo.md = movd;

    tMovS movs;                 tMovW movw;
    movs.qtdColisoes = 0;       movw.qtdColisoes = 0;
    movs.qtdComida = 0;         movw.qtdComida = 0;
    movs.qtdUsos = 0;           movw.qtdUsos = 0;
    movs.charS = 's';           movw.charW = 'w';
    jogo.ms = movs;             jogo.mw = movw;
    return jogo;
}

// funcao principal, realiza todo o jogo por aqui
void RealizaJogo(tJogo jogo, char * diretorio) {
    int nMov = 0;
    jogo = InicializaFantasmas(jogo);
    char acao;

    while (scanf("%c%*c", &acao) == 1) {
        jogo.flagResumo = 0;

        printf("Estado do jogo apos o movimento '%c':\n", acao);
        jogo = EfetuaJogadaPacman(jogo, acao, nMov);  nMov++;

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

            SaidaArquivoEstatisticas(jogo, diretorio, nMov);
            RetornaTrilha(jogo, diretorio);
            RetornaRanking(jogo, diretorio);

            jogo = EfetuaResumo(jogo, diretorio, nMov, acao);
            exit(1);
        }

        jogo = VerificaPosicaoPacEComida(jogo, acao);

        ImprimeMapa(jogo);
        printf("Pontuacao: %d\n\n", RetornaPontuacao(jogo));

        if (jogo.flagResumo != 0) {
            jogo = EfetuaResumo(jogo, diretorio, nMov, acao);
        }

        if (nMov == jogo.qtdJogadas) {
            printf("Game over!\n");
            printf("Pontuacao final: %d\n", RetornaPontuacao(jogo));
            jogo = EfetuaTrilha(jogo, nMov);

            SaidaArquivoEstatisticas(jogo, diretorio, nMov);
            RetornaTrilha(jogo, diretorio);
            RetornaRanking(jogo, diretorio);

            exit(1);
            break;
        }

        jogo = EfetuaTrilha(jogo, nMov);
        if (RetornaPontuacao(jogo) == jogo.qtdComida) break;
    }


    printf("Voce venceu!\n");
    printf("Pontuacao final: %d\n", RetornaPontuacao(jogo));
    SaidaArquivoEstatisticas(jogo, diretorio, nMov);
    RetornaTrilha(jogo, diretorio);
    RetornaRanking(jogo, diretorio);
    jogo = EfetuaResumo(jogo, diretorio, nMov, acao);
}