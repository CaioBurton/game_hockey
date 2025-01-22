#include <GL/glut.h>
#include <stdio.h>
#include <cstdlib>
#include <cmath>

struct Particula {
    GLfloat x, y;    // Posi��o
    GLfloat velX, velY; // Velocidade
    GLfloat r, g, b; // Cor
    bool ativa;      // Ativa ou n�o
};

#define MAX_PARTICULAS 100
Particula particulas[MAX_PARTICULAS];


// Configura��es gerais
GLfloat angle = 45;
GLfloat fAspect;

// Dimens�es da mesa
const int mesaWidth = 200;
const int mesaHeight = 400;
const int maxPontos = 5; // Pontua��o m�xima para vit�ria

// Posi��o dos mallets
GLfloat malletPlayerX = 0.0;
GLfloat malletPlayerY = -80.0;
GLfloat malletCompX = 0.0;
GLfloat malletCompY = 80.0;

// Posi��o e velocidade do puck
GLfloat puckX = 0.0;
GLfloat puckY = 0.0;
GLfloat puckVelX = 0.5;
GLfloat puckVelY = 0.5;

// Tamanho dos objetos
GLfloat tamanhoMallet = 15.0;
GLfloat tamanhoPuck = 10.0;

// Pontua��o
int pontosPlayer = 0;
int pontosComp = 0;

// Vari�vel de dificuldade
int dificuldade = 1; // 1: F�cil, 2: M�dio, 3: Dif�cil

bool isPaused = false; // Vari�vel global para controlar o estado de pausa

// Vari�vel para controlar a exibi��o da mensagem de gol
bool mostrarMensagemGol = false;

// Vari�vel para controlar a exibi��o da mensagem de vit�ria/derrota
bool mostrarMensagemFim = false;
char mensagemFim[50];

// Fun��es de inicializa��o
void init(void);
void desenhaArena(void);
void desenhaGols(void);
void desenhaMallet(GLfloat x, GLfloat y);
void desenhaPuck(GLfloat x, GLfloat y);
void display(void);
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);
void atualiza(int value);
void verificaColisoes(void);
void movimentoMalletComp(void);
void resetarJogo(void);
void verificarVitoria(void);
void desenhaPlacar(void);
void menuPrincipal(int op);
void dificuldadeMenu(int op);
void criarMenu(void);
void movimentaMouse(int x, int y);
void configurarMaterialBrilho(GLfloat r, GLfloat g, GLfloat b);
void inicializarParticulas(GLfloat origemX, GLfloat origemY);
void atualizarParticulas();
void desenharParticulas();


// Programa Principal
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Glow Hockey");
    init();
    criarMenu(); // Cria o menu interativo
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(movimentaMouse);
	glutTimerFunc(16, atualiza, 0); 
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

// Inicializa configura��es
void init(void) {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f); // Fundo azul escuro
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // Configura��o de ilumina��o para efeito de "glow"
    GLfloat luzAmbiente[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat luzDifusa[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat luzEspecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat posicaoLuz[] = { 0.0, 0.0, 100.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Configura��o do material para brilho
    GLfloat matEspecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat brilho[] = { 50.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, matEspecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, brilho);

    glEnable(GL_COLOR_MATERIAL);
}

// Desenha um mallet
void desenhaMallet(GLfloat x, GLfloat y, bool isPlayer) {
    if (isPlayer) {
        configurarMaterialBrilho(0.0, 0.0, 1.0); // Azul para o jogador
    } else {
        configurarMaterialBrilho(1.0, 0.0, 0.0); // Vermelho para o advers�rio
    }
    glPushMatrix();
    glTranslatef(x, y, 0);
    glutSolidSphere(tamanhoMallet, 20, 20);
    glPopMatrix();
}

void movimentaMouse(int x, int y) {
	if (isPaused) return; // Bloqueia o movimento se o jogo estiver pausado
    // Obter a largura e altura da janela atual
    int larguraJanela = glutGet(GLUT_WINDOW_WIDTH);
    int alturaJanela = glutGet(GLUT_WINDOW_HEIGHT);

    // Mapear as coordenadas do mouse para o espa�o do jogo
    GLfloat novoX = ((GLfloat)x / larguraJanela) * mesaWidth - mesaWidth / 2;
    GLfloat novoY = ((GLfloat)(alturaJanela - y) / alturaJanela) * mesaHeight - mesaHeight / 2;

    // Restringir o movimento dentro dos limites da mesa
    if (novoX > -mesaWidth / 2 + tamanhoMallet && novoX < mesaWidth / 2 - tamanhoMallet) {
        malletPlayerX = novoX;
    }
    if (novoY > -mesaHeight / 2 + tamanhoMallet && novoY < 0 - tamanhoMallet) {
        malletPlayerY = novoY; // Permitir apenas no campo inferior (jogador)
    }

    glutPostRedisplay(); // Atualizar a tela
}

// Fun��o para desenhar o puck (bola central) com cor laranja
void desenhaPuck(GLfloat x, GLfloat y) {
    configurarMaterialBrilho(1.0, 1.0, 0.0); // Amarelo para o puck
    glPushMatrix();
    glTranslatef(x, y, 0);
    glutSolidSphere(tamanhoPuck, 20, 20);
    glPopMatrix();
}

// Desenha a arena de h�quei
void desenhaArena(void) {
	glDisable(GL_LIGHTING);
	glLineWidth(4.0); // Define a espessura da linha
	
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0, 0, -tamanhoPuck);
    glScalef(mesaWidth, mesaHeight, 1);
    glutSolidCube(1);
    glPopMatrix();

    // Define as dimens�es das paredes
    float paredeProfundidade = 5.0f; // Define a profundidade das paredes (z)
    float paredeEspessura = 3.0f;    // Define a espessura extra das paredes (x ou y)

    // Desenha paredes da arena
    glColor3f(0.0, 1.0, 0.0); // Verde (Esquerda)
    glPushMatrix();
    glTranslatef(-mesaWidth / 2 - 1, 0, 0); // Esquerda
    glScalef(paredeEspessura, mesaHeight + 2, paredeProfundidade); // Espessura aumentada
    glutSolidCube(1);
    glPopMatrix();

    glColor3f(1.0, 1.0, 0.0); // Amarelo (Direita)
    glPushMatrix();
    glTranslatef(mesaWidth / 2 + 1, 0, 0); // Direita
    glScalef(paredeEspessura, mesaHeight + 2, paredeProfundidade); // Espessura aumentada
    glutSolidCube(1);
    glPopMatrix();
    
    glColor3f(1.0, 0.0, 0.0); // Vermelho (Inferior)
    glPushMatrix();
    glTranslatef(0, -mesaHeight / 2 - 1, 0); // Inferior
    glScalef(mesaWidth + 2, paredeEspessura, paredeProfundidade); // Espessura aumentada
    glutSolidCube(1);
    glPopMatrix();
    
    glColor3f(0.0, 0.0, 1.0); // Azul (Superior)
    glPushMatrix();
    glTranslatef(0, mesaHeight / 2 + 1, 0); // Superior
    glScalef(mesaWidth + 2, paredeEspessura, paredeProfundidade); // Espessura aumentada
    glutSolidCube(1);
    glPopMatrix();
    
    // Desenha a linha central
    glColor3f(1.0, 1.0, 1.0); // Cor da linha central
    glBegin(GL_LINES);
    glVertex3f(-mesaWidth / 2, 0, 0);
    glVertex3f(mesaWidth / 2, 0, 0);
    glEnd();
	
	// Desenha o c�rculo central
    glColor3f(1.0, 1.0, 1.0); // Branco
    float raio = 40.0f; // Define o raio do c�rculo
    int num_segments = 100; // Define o n�mero de segmentos para aproximar o c�rculo
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * 3.1415926f * i / num_segments; // �ngulo atual
        float x = raio * cosf(theta); // Coordenada X
        float y = raio * sinf(theta); // Coordenada Y
        glVertex3f(x, y, 0.0f); // Define o v�rtice
    }
    glEnd();
    
    // Desenha as grandes �reas em forma de meia circunfer�ncia
    glColor3f(1.0, 1.0, 1.0); // Branco
    float areaRaio = 50.0f; // Raio da meia circunfer�ncia
    int area_segments = 50; // N�mero de segmentos para a meia circunfer�ncia

    // Meia circunfer�ncia na extremidade inferior
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= area_segments; i++) {
        float theta = 3.1415926f * i / area_segments; // �ngulo varia de 0 a pi
        float x = areaRaio * cosf(theta);
        float y = -mesaHeight / 2 + areaRaio * sinf(theta); // Posi��o ajustada
        glVertex3f(x, y, 0.0f);
    }
    glEnd();

    // Meia circunfer�ncia na extremidade superior
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= area_segments; i++) {
        float theta = 3.1415926f * i / area_segments; // �ngulo varia de 0 a pi
        float x = areaRaio * cosf(theta);
        float y = mesaHeight / 2 - areaRaio * sinf(theta); // Posi��o ajustada
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
	
	// Restaura a largura da linha (opcional)
	glLineWidth(1.0); // Retorna � largura padr�o

	glEnable(GL_LIGHTING);
}

// Desenha os gols
void desenhaGols(void) {
    glColor3f(0.0, 0.0, 0.0); // Cor dos gols

    // Gol inferior
    glPushMatrix();
    glTranslatef(0, -mesaHeight / 2, 0);
    glScalef(mesaWidth / 2, 2, 5); // Aumenta a profundidade no eixo Z
    glutSolidCube(1);
    glPopMatrix();

    // Gol superior
    glPushMatrix();
    glTranslatef(0, mesaHeight / 2, 0);
    glScalef(mesaWidth / 2, 2, 7); // Aumenta a profundidade no eixo Z
    glutSolidCube(1);
    glPopMatrix();
}

// Fun��o para desativar a mensagem de gol ap�s alguns segundos
void esconderMensagemGol(int value) {
    mostrarMensagemGol = false;
    glutPostRedisplay();
}

// Fun��o para desenhar texto na tela
void desenharTexto(float x, float y, float z, const char *texto) {
    glRasterPos3f(x, y, z);
    while (*texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *texto);
        texto++;
    }
}

void desenharTextoCentralizadoNaTela(const char *texto, float r, float g, float b) {
	glDisable(GL_LIGHTING);
    int larguraJanela = glutGet(GLUT_WINDOW_WIDTH);
    int alturaJanela = glutGet(GLUT_WINDOW_HEIGHT);

    // Calcula a largura do texto em pixels
    int larguraTexto = 0;
    const char *c = texto;
    while (*c) {
        larguraTexto += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
        c++;
    }

    // Calcula a posi��o central
    int x = (larguraJanela - larguraTexto) / 2;
    int y = alturaJanela / 2;

    // Configura proje��o ortogr�fica para desenhar na tela
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, larguraJanela, 0, alturaJanela);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Define a cor do texto
    glColor3f(r, g, b);

    // Define a posi��o do texto
    glRasterPos2i(x, y);

    // Desenha o texto
    while (*texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *texto);
        texto++;
    }

    // Restaura as configura��es anteriores
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, -380.0, 200.0, 0.0, 10.0, 0.0, 0.0, 1.0, 0.0);

    desenhaArena();
    desenhaGols();
    desenhaMallet(malletPlayerX, malletPlayerY, true);
    desenhaMallet(malletCompX, malletCompY, false);
    desenhaPuck(puckX, puckY);

    desenhaPlacar();

    if (isPaused && mostrarMensagemFim) {
        if (pontosPlayer == 5)
            desenharTextoCentralizadoNaTela(mensagemFim, 0.0, 1.0, 0.0);
        else
            desenharTextoCentralizadoNaTela(mensagemFim, 1.0, 0.0, 0.0);
    } else if (isPaused) {
        desenharTextoCentralizadoNaTela("Jogo Pausado", 0.0, 1.0, 0.0); // Verde para pausa
    }

    if (mostrarMensagemGol && (pontosPlayer != 5 || pontosComp != 5)) {
        desenharTextoCentralizadoNaTela("GOALL!!!", 1.0, 1.0, 0.0);
    }

    // Desenhar part�culas
    desenharParticulas();

    glutSwapBuffers();
}



// Atualiza a posi��o do puck e verifica colis�es
void atualiza(int value) {
    if (isPaused) return; // N�o atualiza o jogo se estiver pausado

    // Movimenta o puck
    puckX += puckVelX;
    puckY += puckVelY;

    // Verifica colis�es
    verificaColisoes();

    // Movimenta o mallet do computador
    movimentoMalletComp();

    // Verifica se houve uma vit�ria
    verificarVitoria();

    // Atualizar part�culas
    atualizarParticulas();

    glutPostRedisplay();
    glutTimerFunc(16, atualiza, 0); // Chama a fun��o novamente em 16ms
}

void verificaColisoes(void) {
    // Define o raio da �rea do gol
    GLfloat raioGol = 50.0; // Metade do di�metro da �rea de gol

    // Colis�o com as bordas laterais da mesa
    if (puckX - tamanhoPuck <= -mesaWidth / 2) {
        puckX = -mesaWidth / 2 + tamanhoPuck; // Ajuste para n�o atravessar
        puckVelX = -puckVelX;
    }
    if (puckX + tamanhoPuck >= mesaWidth / 2) {
        puckX = mesaWidth / 2 - tamanhoPuck; // Ajuste para n�o atravessar
        puckVelX = -puckVelX;
    }

    // Verifica��o de gol no gol inferior (do jogador)
    if (puckY - tamanhoPuck <= -mesaHeight / 2) {
        if (sqrt(puckX * puckX) <= raioGol) {  // Verifica se o puck est� dentro da �rea circular do gol
            pontosComp++;
            if (pontosComp != maxPontos) {
                resetarJogo();
            }
        } else {
            puckY = -mesaHeight / 2 + tamanhoPuck; // Rebater o puck na parede inferior
            puckVelY = -puckVelY;
        }
    }

    // Verifica��o de gol no gol superior (do computador)
    if (puckY + tamanhoPuck >= mesaHeight / 2) {
        if (sqrt(puckX * puckX) <= raioGol) {  // Verifica se o puck est� dentro da �rea circular do gol
            pontosPlayer++;
            if (pontosPlayer != maxPontos) {
                resetarJogo();
            }
        } else {
            puckY = mesaHeight / 2 - tamanhoPuck; // Rebater o puck na parede superior
            puckVelY = -puckVelY;
        }
    }

    // Colis�o com o mallet do jogador
    GLfloat dxPlayer = puckX - malletPlayerX;
    GLfloat dyPlayer = puckY - malletPlayerY;
    GLfloat distPlayer2 = dxPlayer * dxPlayer + dyPlayer * dyPlayer;
    GLfloat somaRaio2 = (tamanhoMallet + tamanhoPuck) * (tamanhoMallet + tamanhoPuck);

    if (distPlayer2 <= somaRaio2) {
        GLfloat distPlayer = sqrt(distPlayer2);
        GLfloat overlap = (tamanhoMallet + tamanhoPuck) - distPlayer;
        GLfloat dirX = dxPlayer / distPlayer;
        GLfloat dirY = dyPlayer / distPlayer;

        malletPlayerX -= overlap * dirX * 0.5;
        malletPlayerY -= overlap * dirY * 0.5;

        puckVelX = dxPlayer * 0.2;
        puckVelY = dyPlayer * 0.2;
    }

    // Colis�o com o mallet do computador
    GLfloat dxComp = puckX - malletCompX;
    GLfloat dyComp = puckY - malletCompY;
    GLfloat distComp2 = dxComp * dxComp + dyComp * dyComp;

    if (distComp2 <= somaRaio2) {
        GLfloat distComp = sqrt(distComp2);
        GLfloat overlap = (tamanhoMallet + tamanhoPuck) - distComp;
        GLfloat dirX = dxComp / distComp;
        GLfloat dirY = dyComp / distComp;

        malletCompX -= overlap * dirX * 0.5;
        malletCompY -= overlap * dirY * 0.5;

        puckVelX = dxComp * 0.2;
        puckVelY = dyComp * 0.2;
    }
}


void movimentoMalletComp(void) {
    GLfloat vel = 0.3; // Velocidade base
    if (dificuldade == 2) vel = 0.5; // M�dio
    if (dificuldade == 3) vel = 0.7; // Dif�cil

    // Centro do gol do jogador (alvo da IA)
    GLfloat alvoX = 0.0;
    GLfloat alvoY = -mesaHeight / 2;

    // Vetor dire��o do puck ao gol
    GLfloat dirPuckGolX = alvoX - puckX;
    GLfloat dirPuckGolY = alvoY - puckY;
    GLfloat magPuckGol = sqrt(dirPuckGolX * dirPuckGolX + dirPuckGolY * dirPuckGolY);
    dirPuckGolX /= magPuckGol; // Normalizar dire��o
    dirPuckGolY /= magPuckGol;

    // Posi��o de intercepta��o: um pouco antes de empurrar o puck
    GLfloat interceptX = puckX - dirPuckGolX * tamanhoMallet * 1.5;
    GLfloat interceptY = puckY - dirPuckGolY * tamanhoMallet * 1.5;

    // Mover o mallet para a posi��o de intercepta��o
    if (malletCompX < interceptX) malletCompX += vel;
    if (malletCompX > interceptX) malletCompX -= vel;
    if (malletCompY < interceptY && malletCompY + vel <= mesaHeight / 2 - tamanhoMallet) {
        malletCompY += vel;
    }
    if (malletCompY > interceptY && malletCompY - vel > 0 + tamanhoMallet) {
        malletCompY -= vel;
    }

    // Garantir que a IA n�o fique presa nas bordas laterais
    if (malletCompX <= -mesaWidth / 2 + tamanhoMallet) malletCompX += vel * 2;
    if (malletCompX >= mesaWidth / 2 - tamanhoMallet) malletCompX -= vel * 2;
}


// Reseta a posi��o do puck e dos mallets ap�s um ponto
void resetarJogo(void) {
    puckX = 0.0;
    puckY = 0.0;
    puckVelX = (rand() % 2 == 0 ? 0.3 : -0.3);
    puckVelY = (rand() % 2 == 0 ? 0.3 : -0.3);
    malletPlayerX = 0.0;
    malletPlayerY = -80.0;
    malletCompX = 0.0;
    malletCompY = 80.0;

    // Exibir mensagem de gol
    mostrarMensagemGol = true;

    // Inicializar part�culas no centro do campo
    inicializarParticulas(0.0, 0.0);

    glutTimerFunc(2000, esconderMensagemGol, 0); // Remove a mensagem ap�s 2 segundos
}


// Verifica se houve uma vit�ria ou derrota
void verificarVitoria(void) {
    if (pontosPlayer >= maxPontos) {
        sprintf(mensagemFim, "Voce venceu!");
        mostrarMensagemFim = true;
        isPaused = true;
    } else if (pontosComp >= maxPontos) {
        sprintf(mensagemFim, "Voce perdeu!");
        mostrarMensagemFim = true;
        isPaused = true;
    }
}

// Ajusta o tamanho da janela
void reshape(GLsizei w, GLsizei h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    fAspect = (GLfloat)w / (GLfloat)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angle, fAspect, 0.1, 1000); // Ajustar o campo de vis�o e a profundidade
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Controle de teclado para mover o mallet do jogador e alternar entre as c�meras
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            if (malletPlayerY + 3.0 <= mesaHeight / 2 - tamanhoMallet) malletPlayerY += 2.5;
            break;
        case 's':
            if (malletPlayerY - 3.0 >= -mesaHeight / 2 + tamanhoMallet) malletPlayerY -= 2.5;
            break;
        case 'a':
            if (malletPlayerX - 3.0 >= -mesaWidth / 2 + tamanhoMallet) malletPlayerX -= 2.5;
            break;
        case 'd':
            if (malletPlayerX + 3.0 <= mesaWidth / 2 - tamanhoMallet) malletPlayerX += 2.5;
            break;
        case 'p': // Pausa e despausa
            isPaused = !isPaused;
            if (!isPaused) {
                glutTimerFunc(16, atualiza, 0); // Retoma 
            }
            break;
        case 27:
            exit(0);
    }
    glutPostRedisplay();
}

// Desenha o placar na tela
void desenhaPlacar(void) {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    int larguraJanela = glutGet(GLUT_WINDOW_WIDTH);
    int alturaJanela = glutGet(GLUT_WINDOW_HEIGHT);

    char placar[50];
    sprintf(placar, "Jogador: %d  |  Computador: %d", pontosPlayer, pontosComp);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, larguraJanela, 0, alturaJanela);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Desenha o fundo do placar
    glColor4f(0.0, 0.0, 0.0, 0.5);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int larguraPlacar = 300;
    int alturaPlacar = 50;
    int x = (larguraJanela - larguraPlacar) / 2; // Centro da janela
    int y = alturaJanela - alturaPlacar - 70;    // Posi��o do placar no topo da tela

    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + larguraPlacar, y);
    glVertex2f(x + larguraPlacar, y + alturaPlacar);
    glVertex2f(x, y + alturaPlacar);
    glEnd();
    
    glDisable(GL_BLEND);

    // Calcular a largura total do texto para centraliz�-lo
    int larguraTexto = 0;
    for (char *c = placar; *c != '\0'; c++) {
        larguraTexto += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    }

    // Centralizar o texto dentro da �rea preta do placar
    int textoX = x + (larguraPlacar - larguraTexto) / 2;
    int textoY = y + (alturaPlacar / 2) - 5;  // Posiciona o texto no meio verticalmente

    glColor3f(1.0, 1.0, 1.0);  // Cor do texto branca
    glRasterPos2i(textoX, textoY);

    // Renderiza o texto caractere por caractere
    for (char *c = placar; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}


// Cria o menu principal e o de dificuldade
void criarMenu(void) {
    int menuPrincipalId = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Iniciar Jogo", 1);
    glutAddMenuEntry("Sair", 2);

    int dificuldadeMenuId = glutCreateMenu(dificuldadeMenu);
    glutAddMenuEntry("F�cil", 1);
    glutAddMenuEntry("M�dio", 2);
    glutAddMenuEntry("Dif�cil", 3);

    glutSetMenu(menuPrincipalId);
    glutAddSubMenu("Escolher Dificuldade", dificuldadeMenuId);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Menu principal
void menuPrincipal(int op) {
    switch (op) {
        case 1: // Iniciar Jogo
            resetarJogo();
            pontosPlayer = 0;
            pontosComp = 0;
            glutTimerFunc(16, atualiza, 0); // Inicia a atualiza��o do jogo
            break;
        case 2: // Sair
            exit(0);
            break;
    }
}

// Menu de dificuldade
void dificuldadeMenu(int op) {
    dificuldade = op; // Define a dificuldade com base na escolha
}

void configurarMaterialBrilho(GLfloat r, GLfloat g, GLfloat b) {
    GLfloat ambiente[] = { r * 0.2f, g * 0.2f, b * 0.2f, 1.0f };
    GLfloat difusa[] = { r, g, b, 1.0f };
    GLfloat especular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat brilho = 50.0f; // Ajuste o valor para o n�vel de brilho desejado

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambiente);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, difusa);
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT, GL_SHININESS, brilho);
    
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(r, g, b);
}

void inicializarParticulas(GLfloat origemX, GLfloat origemY) {
    GLfloat escalaVelocidade = 2.0f; // Fator para aumentar a velocidade das part�culas

    for (int i = 0; i < MAX_PARTICULAS; i++) {
        particulas[i].x = origemX;
        particulas[i].y = origemY;
        particulas[i].velX = ((rand() % 100) - 50) / 50.0f * escalaVelocidade; // Velocidade X escalada
        particulas[i].velY = ((rand() % 100) - 50) / 50.0f * escalaVelocidade; // Velocidade Y escalada
        particulas[i].r = (rand() % 256) / 255.0f; // Cor R aleat�ria
        particulas[i].g = (rand() % 256) / 255.0f; // Cor G aleat�ria
        particulas[i].b = (rand() % 256) / 255.0f; // Cor B aleat�ria
        particulas[i].ativa = true;
    }
}


void atualizarParticulas() {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (particulas[i].ativa) {
            particulas[i].x += particulas[i].velX;
            particulas[i].y += particulas[i].velY;

            // Desativa a part�cula quando sai do limite
            if (fabs(particulas[i].x) > mesaWidth / 2 || fabs(particulas[i].y) > mesaHeight / 2) {
                particulas[i].ativa = false;
            }
        }
    }
}

void desenharParticulas() {
    for (int i = 0; i < MAX_PARTICULAS; i++) {
        if (particulas[i].ativa) {
            glColor3f(particulas[i].r, particulas[i].g, particulas[i].b);
            glPushMatrix();
            glTranslatef(particulas[i].x, particulas[i].y, 0);
            glutSolidSphere(2, 10, 10); // Part�cula pequena
            glPopMatrix();
        }
    }
}