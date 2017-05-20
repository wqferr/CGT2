#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include <GL/glut.h>

#include "invader.h"

#define GRID_MIN_X (INVADER_SIZE/2)
#define GRID_MAX_X (WINDOW_W - GRID_INVADER_W*INVADER_SIZE)

#define INVADER_Y_POS_LIM (WINDOW_H - INVADER_SIZE)
#define INVADER_SHOT_SPEED 2.f

const InvaderType ROW_TYPES[] = {SQUARE, SQUARE, CIRCLE, CIRCLE, TRIANGLE};

uint8_t nInvadersAlive;
bool **invaders;

GLfloat gridX, gridY;
GLfloat invShotX, invShotY;
bool invShot;
bool stop;

int8_t gridMoveDir;


typedef void (*InvaderDrawFunc) (void);

void invader_drawSquare(void) {
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);
    glEnd();
}

void invader_drawCircle(void) {
    int i;
    float r;

    glBegin(GL_LINE_LOOP);
        for (i = 0; i < 200; i++) {
            r = M_PI/100.f * i;
            glVertex2f(cos(r)/2, sin(r)/2);
        }
    glEnd();
}

void invader_drawTriang(void) {
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.f, 0.5f);
    glEnd();
}

// Array de funções para cada TIPO, não linha, da matriz de invaders
const InvaderDrawFunc RENDER_FUNCTIONS[] = {
    NULL,
    &invader_drawSquare,
    &invader_drawCircle,
    &invader_drawTriang
};


void invader_updateShot(int val) {
    (void) val;

    invShotY += INVADER_SHOT_SPEED;
    if (invShotY >= WINDOW_H) {
        invShot = false;
    } else {
        glutTimerFunc(DELAY, &invader_updateShot, 0);
    }
}

void invader_shoot(int r, int c) {
    invShotX = gridX + (c+1.f/2)*INVADER_SIZE;
    invShotY = gridY + (r+1)*INVADER_SIZE;
    invShot = true;
    glutTimerFunc(DELAY, &invader_updateShot, 0);
}

void invader_tryShoot(int val) {
    int i, col;
    
    (void) val;
    col = rand() % GRID_INVADER_W;
    for (i = GRID_INVADER_H-1; i >= 0; i--) {
        if (invaders[i][col]) {
            invader_shoot(i, col);
            break;
        }
    }
    glutTimerFunc(5000, &invader_tryShoot, 0);
}

void invader_spawnAll(void) {
    int i, j;

    srand(time(NULL));
    gridX = INVADER_SIZE;
    gridY = INVADER_SIZE;
    gridMoveDir = 1;
    nInvadersAlive = GRID_INVADER_H * GRID_INVADER_W;
    gluOrtho2D(0, WINDOW_W, WINDOW_H, 0);

    invaders = malloc(GRID_INVADER_H * sizeof(*invaders));
    for (i = 0; i < GRID_INVADER_H; i++) {
        invaders[i] = malloc(GRID_INVADER_W * sizeof(**invaders));
        memset(invaders[i], true, GRID_INVADER_W*sizeof(**invaders));
    }

    stop = false;
    invShot = false;
    invader_update(0);
    invader_tryShoot(0);
}

void invader_destroyAll(void) {
    int i, j;
    for (i = 0; i < GRID_INVADER_H; i++)
        free(invaders[i]);
    free(invaders);
}

void invader_stop(void) {
    stop = true;
}


int invader_furthestRow(void) {
    int i, j;

    for (i = GRID_INVADER_H-1; i >= 0; i--) {
        for (j = 0; j < GRID_INVADER_W; j++) {
            if (invaders[i][j])
                return i;
        }
    }
    return -1;
}

void invader_update(int val) {
    int r;

    if (!stop) {
        (void) val;

        if ((gridMoveDir > 0 && gridX >= GRID_MAX_X)
                || (gridMoveDir < 0 && gridX <= GRID_MIN_X)) {
            gridY += INVADER_SIZE;
            gridMoveDir *= -1;
        }
        gridX += X_SPEED * gridMoveDir;

        glutPostRedisplay();

        if (nInvadersAlive > 0) {
            r = invader_furthestRow()+1;
            if (gridY + r*INVADER_SIZE < INVADER_Y_POS_LIM)
                glutTimerFunc(DELAY, &invader_update, 0);
        }
    }
}

void invader_shotDraw(void) {
    if (invShot) {
        glPushMatrix();
            glTranslatef(invShotX, invShotY, 0.f);
            glScalef(1.f, -1.f, 1.f);
            glColor3f(1.f, 0.f, 0.f);
            glBegin(GL_POLYGON);
                glVertex2f(-1.f, -4.f);
                glVertex2f(1.f, -4.f);
                glVertex2f(1.f, 4.f);
                glVertex2f(-1.f, 4.f);
            glEnd();
            glColor3f(1.f, 1.f, 1.f);
        glPopMatrix();
    }
}

void invader_draw(void) {
    int i, j;

    invader_shotDraw();

    glPushMatrix();
        glTranslatef(gridX, gridY, 0.f);
        glScalef(INVADER_SIZE, INVADER_SIZE, 1.f);

        for (i = 0; i < GRID_INVADER_H; i++) {
            InvaderType rowType = ROW_TYPES[i];
            InvaderDrawFunc render = RENDER_FUNCTIONS[rowType];

            glPushMatrix();
                for (j = 0; j < GRID_INVADER_W; j++) {
                    if (invaders[i][j])
                        render();
                    glTranslatef(1.f, 0.f, 0.f);
                }
            glPopMatrix();
            glTranslatef(0.f, 1.f, 0.f);
        }
    glPopMatrix();
}

InvaderType invader_checkCollision(GLfloat x, GLfloat y) {
    int i, j;
    x -= gridX - INVADER_SIZE/2.f;
    y -= gridY - INVADER_SIZE/2.f;

    j = floor(x/INVADER_SIZE);
    i = floor(y/INVADER_SIZE);

    if (j < 0 || j >= GRID_INVADER_W
            || i < 0 || i >= GRID_INVADER_H)
        return NONE;

    if (invaders[i][j]) {
        invaders[i][j] = false;
        nInvadersAlive--;
        return ROW_TYPES[i];
    }
    return NONE;
}

void invader_shotPosition(GLfloat *x, GLfloat *y) {
    if (invShot) {
        *x = invShotX;
        *y = invShotY;
    } else {
        *x = *y = -1;
    }
}

void invader_destroyShot(void) {
    invShot = false;
}
