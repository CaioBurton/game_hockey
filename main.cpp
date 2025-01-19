#include <GL/glut.h>
#include <stdio.h>
#include <cstdlib>
//#include "stb_image.h"
#include <cmath>
#define NUM_FLOCOS 100

// Configurações gerais
GLfloat angle = 45;
GLfloat fAspect;

// Dimensões da mesa
const int mesaWidth = 200;
const int mesaHeight = 400;
const int maxPontos = 5; // Pontuação máxima para vitória

// Posição dos mallets
GLfloat malletPlayerX = 0.0;
GLfloat malletPlayerY = -80.0;
GLfloat malletCompX = 0.0;
GLfloat malletCompY = 80.0;

// Posição e velocidade do puck
GLfloat puckX = 0.0;
GLfloat puckY = 0.0;
GLfloat puckVelX = 0.5;
GLfloat puckVelY = 0.5;

// Tamanho dos objetos
GLfloat tamanhoMallet = 10.0;
GLfloat tamanhoPuck = 5.0;

// Pontuação
int pontosPlayer = 0;
int pontosComp = 0;

// Variáveis de controle de câmera
int cameraMode = 1; // 0: Padrão, 1: Campo do jogador, 2: Campo do computador

// Variável de dificuldade
int dificuldade = 1; // 1: Fácil, 2: Médio, 3: Difícil

bool isPaused = false; // Variável global para controlar o estado de pausa

// Funções de inicialização
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
void configurarCamera(void);
void desenhaPlacar(void);
void menuPrincipal(int op);
void dificuldadeMenu(int op);
void criarMenu(void);
void movimentaMouse(int x, int y);

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

// Inicializa configurações
void init(void) {
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f); // Fundo azul escuro
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // Configuração de iluminação para efeito de "glow"
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

    // Configuração do material para brilho
    GLfloat matEspecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat brilho[] = { 50.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, matEspecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, brilho);

    glEnable(GL_COLOR_MATERIAL);
}

// Desenha um mallet
void desenhaMallet(GLfloat x, GLfloat y, bool isPlayer) {
    if (isPlayer) {
        glColor3f(0.0, 0.0, 1.0); // Azul para o jogador
    } else {
        glColor3f(1.0, 0.0, 0.0); // Vermelho para o adversário
    }
    glPushMatrix();
    glTranslatef(x, y, 0);
    glutSolidSphere(tamanhoMallet, 20, 20);
    glPopMatrix();
}

void movimentaMouse(int x, int y) {
    // Obter a largura e altura da janela atual
    int larguraJanela = glutGet(GLUT_WINDOW_WIDTH);
    int alturaJanela = glutGet(GLUT_WINDOW_HEIGHT);

    // Mapear as coordenadas do mouse para o espaço do jogo
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

// Função para desenhar o puck (bola central) com cor laranja
void desenhaPuck(GLfloat x, GLfloat y) {
    glColor3f(1.0, 1.0, 0.0); // Amarelo
    glPushMatrix();
    glTranslatef(x, y, 0);
    glutSolidSphere(tamanhoPuck, 20, 20);
    glPopMatrix();
}

// Desenha a arena de hóquei
void desenhaArena(void) {
	glDisable(GL_LIGHTING);
	
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0, 0, -tamanhoPuck);
    glScalef(mesaWidth, mesaHeight, 1);
    glutSolidCube(1);
    glPopMatrix();

    // Desenha paredes da arena
	glColor3f(0.0, 1.0, 0.0); // Verde

    glPushMatrix();
    glTranslatef(-mesaWidth / 2 - 1, 0, 0); // Esquerda
    glScalef(2, mesaHeight + 2, 2);
    glutSolidCube(1);
    glPopMatrix();

	glColor3f(1.0, 1.0, 0.0); // Amarelo

    glPushMatrix();
    glTranslatef(mesaWidth / 2 + 1, 0, 0); // Direita
    glScalef(2, mesaHeight + 2, 2);
    glutSolidCube(1);
    glPopMatrix();
    
    glColor3f(1.0, 0.0, 0.0); // Vermelho

    glPushMatrix();
    glTranslatef(0, -mesaHeight / 2 - 1, 0); // Inferior
    glScalef(mesaWidth + 2, 2, 2);
    glutSolidCube(1);
    glPopMatrix();
    
    glColor3f(0.0, 0.0, 1.0); // Azul

    glPushMatrix();
    glTranslatef(0, mesaHeight / 2 + 1, 0); // Superior
    glScalef(mesaWidth + 2, 2, 2);
    glutSolidCube(1);
    glPopMatrix();

	glEnable(GL_LIGHTING);
}


// Desenha os gols
void desenhaGols(void) {
    glColor3f(0.0, 0.0, 0.0); // Cor dos gols

    // Gol inferior
    glPushMatrix();
    glTranslatef(0, -mesaHeight / 2 , 0);
    glScalef(mesaWidth / 3, 2, 2);
    glutSolidCube(1);
    glPopMatrix();

    // Gol superior
    glPushMatrix();
    glTranslatef(0, mesaHeight / 2 , 0);
    glScalef(mesaWidth / 3, 2, 2);
    glutSolidCube(1);
    glPopMatrix();
}

// Desenha um mallet
// void desenhaMallet(GLfloat x, GLfloat y) {
//     glEnable(GL_LIGHTING); // Ativar iluminação
//     glColor3f(1.0, 0.0, 0.0); // Vermelho para o mallet do jogador
//     glPushMatrix();
//     glTranslatef(x, y, 0);
//     glutSolidSphere(tamanhoMallet, 20, 20);
//     glPopMatrix();
//     glDisable(GL_LIGHTING); // Desativar iluminação
// }
// 
// Desenha o puck
// void desenhaPuck(GLfloat x, GLfloat y) {
//     glEnable(GL_LIGHTING); // Ativar iluminação
//     glColor3f(1.0, 1.0, 0.0); // Amarelo para o puck
//     glPushMatrix();
//     glTranslatef(x, y, 0);
//     glutSolidSphere(tamanhoPuck, 20, 20);
//     glPopMatrix();
//     glDisable(GL_LIGHTING); // Desativar iluminação
// }



// Configura a câmera de acordo com o modo selecionado
void configurarCamera(void) {
    switch (cameraMode) {
        case 0: // Padrão
            gluLookAt(0.0, 0.0, 500.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
            break;
        case 1: // Isométrica do jogador
            gluLookAt(0.0, -380.0, 200.0, 0.0, 10.0, 0.0, 0.0, 1.0, 0.0);
            break;
        case 2: // Isométrica do computador
            gluLookAt(0.0, 380.0, 200.0, 0.0, 50.0, 0.0, 0.0, -1.0, 0.0);
            break;
        case 3: // Câmera que segue o jogador
            gluLookAt(malletPlayerX, malletPlayerY - 50, 200.0, malletPlayerX, malletPlayerY, 0.0, 0.0, 1.0, 0.0);
            break;
    }
}


// Função para desenhar texto na tela
void desenharTexto(float x, float y, float z, const char *texto) {
    glRasterPos3f(x, y, z);
    while (*texto) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *texto);
        texto++;
    }
}

// Desenha os flocos de neve
// void desenharFlocosDeNeve() {
//     glColor3f(1.0, 1.0, 1.0);
//     for (int i = 0; i < NUM_FLOCOS; i++) {
//         glPushMatrix();
//         glTranslatef(flocos[i].x, flocos[i].y, flocos[i].z);
//         glutSolidSphere(1, 10, 10);
//         glPopMatrix();
//     }
// }
// 
// Atualizar a função display para exibir o texto de pausa
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    configurarCamera();

    desenhaArena();
    desenhaGols();

    // Desenha a linha central
    glColor3f(1.0, 1.0, 1.0); // Cor da linha central
    glBegin(GL_LINES);
    glVertex3f(-mesaWidth / 2, 0, 0);
    glVertex3f(mesaWidth / 2, 0, 0);
    glEnd();

    desenhaMallet(malletPlayerX, malletPlayerY, true);  // Mallet do jogador
    desenhaMallet(malletCompX, malletCompY, false);    // Mallet do adversário
    desenhaPuck(puckX, puckY);                         // Puck (bola central)

    desenhaPlacar();

    // Desenhar texto informativo de pausa
    if (isPaused) {
        desenharTexto(-10, 0, 0, "Jogo Pausado");
    }

    glutSwapBuffers();
}

// Atualiza a posição dos flocos de neve
// void atualizarFlocosDeNeve() {
//     for (int i = 0; i < NUM_FLOCOS; i++) {
//         flocos[i].y -= flocos[i].velocidade;
//         if (flocos[i].y < -200) {
//             flocos[i].y = 200;
//         }
//     }
// }


// Atualiza a posição do puck e verifica colisões
void atualiza(int value) {
	
    if (isPaused) return; // Não atualiza o jogo se estiver pausado

    // Movimenta o puck
    puckX += puckVelX;
    puckY += puckVelY;

    // Verifica colisões
    verificaColisoes();
    
    // Movimenta o mallet do computador
    movimentoMalletComp();

    // Verifica se houve uma vitória
    verificarVitoria();
    
	// Atualiza a posição dos flocos de neve
//    atualizarFlocosDeNeve();

    glutPostRedisplay();
    glutTimerFunc(16, atualiza, 0); // Chama a função novamente em 16ms
}

void verificaColisoes(void) {
    // Colisão com as bordas da mesa
    if (puckX - tamanhoPuck <= -mesaWidth / 2) {
        puckX = -mesaWidth / 2 + tamanhoPuck; // Ajuste para não atravessar
        puckVelX = -puckVelX;
    }
    if (puckX + tamanhoPuck >= mesaWidth / 2) {
        puckX = mesaWidth / 2 - tamanhoPuck; // Ajuste para não atravessar
        puckVelX = -puckVelX;
    }

    // Verificação de gol
    if (puckY - tamanhoPuck <= -mesaHeight / 2) {
        if (abs(puckX) <= mesaWidth / 6) {
            pontosComp++;
            resetarJogo();
        } else {
            puckY = -mesaHeight / 2 + tamanhoPuck; // Ajuste para não atravessar
            puckVelY = -puckVelY;
        }
    }

    if (puckY + tamanhoPuck >= mesaHeight / 2) {
        if (abs(puckX) <= mesaWidth / 6) {
            pontosPlayer++;
            resetarJogo();
        } else {
            puckY = mesaHeight / 2 - tamanhoPuck; // Ajuste para não atravessar
            puckVelY = -puckVelY;
        }
    }

    // Colisão com o mallet do jogador
    GLfloat dxPlayer = puckX - malletPlayerX;
    GLfloat dyPlayer = puckY - malletPlayerY;
    GLfloat distPlayer2 = dxPlayer * dxPlayer + dyPlayer * dyPlayer; // a norma entre os centros deve ser maior que a soma dos raios
    GLfloat somaRaio2 = (tamanhoMallet + tamanhoPuck) * (tamanhoMallet + tamanhoPuck);

    if (distPlayer2 <= somaRaio2) {
        GLfloat distPlayer = sqrt(distPlayer2);
        GLfloat overlap = (tamanhoMallet + tamanhoPuck) - distPlayer;
        GLfloat dirX = dxPlayer / distPlayer;
        GLfloat dirY = dyPlayer / distPlayer;

        malletPlayerX -= overlap * dirX * 0.5; // Move o mallet do jogador para fora da colisão
        malletPlayerY -= overlap * dirY * 0.5; // 

        puckVelX = dxPlayer * 0.2;
        puckVelY = dyPlayer * 0.2;
    }

    // Colisão com o mallet do computador
    GLfloat dxComp = puckX - malletCompX;
    GLfloat dyComp = puckY - malletCompY;
    GLfloat distComp2 = dxComp * dxComp + dyComp * dyComp; // a norma entre os centros deve ser maior que a soma dos raios

    if (distComp2 <= somaRaio2) {
        GLfloat distComp = sqrt(distComp2);
        GLfloat overlap = (tamanhoMallet + tamanhoPuck) - distComp;
        GLfloat dirX = dxComp / distComp;
        GLfloat dirY = dyComp / distComp;

        malletCompX -= overlap * dirX * 0.5; // Move o mallet da CPU para fora da colisão
        malletCompY -= overlap * dirY * 0.5; // esse overlap garante que o CPU não fique sobrepondo o disco na movimentação

        puckVelX = dxComp * 0.2;
        puckVelY = dyComp * 0.2;
    }
}



void movimentoMalletComp(void) {
    GLfloat vel = 0.2; // Velocidade base
    if (dificuldade == 2) vel = 0.4; // Médio
    if (dificuldade == 3) vel = 0.6; // Difícil

    GLfloat dirX = puckX - malletCompX;
    GLfloat dirY = puckY - malletCompY;
    GLfloat dist = sqrt(dirX * dirX + dirY * dirY);

    // Movimenta o mallet da CPU somente se estiver longe o suficiente do puck
    if (dist > tamanhoMallet + tamanhoPuck + 5.0) {
        if (puckX > malletCompX) malletCompX += vel;
        if (puckX < malletCompX) malletCompX -= vel;
        if (puckY > malletCompY && malletCompY + vel <= mesaHeight / 2 - tamanhoMallet) {
            malletCompY += vel; // Apenas se estiver dentro do campo superior
        }
        if (puckY < malletCompY && malletCompY - vel > 0 + tamanhoMallet) {
            malletCompY -= vel; // Apenas se estiver dentro do campo superior
        }
    }
}


// Reseta a posição do puck e dos mallets após um ponto
void resetarJogo(void) {
    puckX = 0.0;
    puckY = 0.0;
    puckVelX = (rand() % 2 == 0 ? 0.3 : -0.3);
    puckVelY = (rand() % 2 == 0 ? 0.3 : -0.3);
    malletPlayerX = 0.0;
    malletPlayerY = -80.0;
    malletCompX = 0.0;
    malletCompY = 80.0;
}

// Verifica se houve uma vitória ou derrota
void verificarVitoria(void) {
    if (pontosPlayer >= maxPontos) {
        printf("Você venceu!\n");
        exit(0);
    } else if (pontosComp >= maxPontos) {
        printf("Você perdeu!\n");
        exit(0);
    }
}

// Ajusta o tamanho da janela
void reshape(GLsizei w, GLsizei h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    fAspect = (GLfloat)w / (GLfloat)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angle, fAspect, 0.1, 1000); // Ajustar o campo de visão e a profundidade
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Controle de teclado para mover o mallet do jogador e alternar entre as câmeras
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
        case 'c':
            cameraMode = (cameraMode + 1) % 4;
            break;
        case 'C':
            cameraMode = (cameraMode + 2) % 4; // Alterna para a câmera anterior
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
    char placar[50];
    sprintf(placar, "Player: %d  Computer: %d", pontosPlayer, pontosComp);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2i(10, 570);

    for (char *c = placar; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Cria o menu principal e o de dificuldade
void criarMenu(void) {
    int menuPrincipalId = glutCreateMenu(menuPrincipal);
    glutAddMenuEntry("Iniciar Jogo", 1);
    glutAddMenuEntry("Sair", 2);

    int dificuldadeMenuId = glutCreateMenu(dificuldadeMenu);
    glutAddMenuEntry("Fácil", 1);
    glutAddMenuEntry("Médio", 2);
    glutAddMenuEntry("Difícil", 3);

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
            glutTimerFunc(16, atualiza, 0); // Inicia a atualização do jogo
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


// 
// struct FlocoDeNeve {
//     float x, y, z;
//     float velocidade;
// };
// 
// FlocoDeNeve flocos[NUM_FLOCOS];
// 
// Inicializa os flocos de neve
// void inicializarFlocosDeNeve() {
//     for (int i = 0; i < NUM_FLOCOS; i++) {
//         flocos[i].x = (rand() % 200) - 100;
//         flocos[i].y = (rand() % 400) - 200;
//         flocos[i].z = 0;
//         flocos[i].velocidade = (rand() % 5 + 1) / 10.0;
//     }
// }
// }
