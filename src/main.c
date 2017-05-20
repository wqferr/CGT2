#include <GL/glut.h>

#include "invader.h"

#include <stdio.h>

#define PLAYER_SPEED 1.5f
#define SHOT_SPEED 6.f

#define PLAYER_RIGHT_KEY GLUT_KEY_RIGHT
#define PLAYER_LEFT_KEY GLUT_KEY_LEFT
#define PLAYER_SHOOT_KEY ' '

GLfloat playerX;
const GLfloat playerY = WINDOW_H - INVADER_SIZE;

bool shot, playerShooting;
GLfloat shotX, shotY;
bool playerMoveR, playerMoveL;

int lives;

void player_draw(void) {
    glPushMatrix();
        glTranslatef(playerX, playerY, 0);
        glScalef(INVADER_SIZE, -INVADER_SIZE, 1.f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(-1.f, 0.3f);
            glVertex2f(-1.f, -0.5f);
            glVertex2f(1.f, -0.5f);
            glVertex2f(1.f, 0.3f);
            glVertex2f(0.15f, 0.3f);
            glVertex2f(0.15f, .75f);
            glVertex2f(-0.15f, .75f);
            glVertex2f(-0.15f, 0.3f);
        glEnd();
    glPopMatrix();
}

void shot_draw(void) {
    if (shot) {
        glPushMatrix();
            glTranslatef(shotX, shotY, 0);
            glBegin(GL_POLYGON);
                glVertex2f(-1.f, -4.f);
                glVertex2f(1.f, -4.f);
                glVertex2f(1.f, 4.f);
                glVertex2f(-1.f, 4.f);
            glEnd();
        glPopMatrix();
    }
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.f, 1.f, 1.f);
    player_draw();
    shot_draw();
    invader_draw();

    glutSwapBuffers();
}

void specialKeyPress(int key, int x, int y) {
    switch (key) {
        case PLAYER_LEFT_KEY:
            playerMoveL = true;
            break;
        case PLAYER_RIGHT_KEY:
            playerMoveR = true;
            break;
    }
}

void specialKeyUp(int key, int x, int y) {
    switch (key) {
        case PLAYER_LEFT_KEY:
            playerMoveL = false;
            break;
        case PLAYER_RIGHT_KEY:
            playerMoveR = false;
            break;
    }
}

void keyPress(unsigned char key, int x, int y) {
    if (key == PLAYER_SHOOT_KEY && !shot)
        playerShooting = true;
}

void updateShot(int val) {
    (void) val;
    shotY -= SHOT_SPEED;
    if (invader_checkCollision(shotX, shotY) || shotY <= 0) {
        shot = false;
    } else {
        glutTimerFunc(DELAY, &updateShot, 0);
    }
}

void updatePlayer(int val) {
    GLfloat invShotX, invShotY;

    (void) val;
    
    playerX += PLAYER_SPEED * (playerMoveR-playerMoveL);
    if (playerShooting) {
        playerShooting = false;
        shot = true;
        shotX = playerX;
        shotY = playerY;

        glutTimerFunc(DELAY, &updateShot, 0);
    }

    invader_shotPosition(&invShotX, &invShotY);
    if (invShotX > 0) {
        invShotX -= playerX;
        invShotY -= playerY;

        invShotX /= INVADER_SIZE;
        invShotY /= INVADER_SIZE;

        if (abs(invShotX) <= 1 && invShotY <= 0.3f && invShotY >= -0.5f) {
            lives--;
            invader_destroyShot();
        }
    }

    if (lives > 0) {
        glutTimerFunc(DELAY, &updatePlayer, 0);
    } else {
        invader_stop();
    }
}

void start(int *argc, char *argv[]) {
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    glutInitWindowPosition(
            (glutGet(GLUT_SCREEN_WIDTH)-WINDOW_W) / 2,
            (glutGet(GLUT_SCREEN_HEIGHT)-WINDOW_H) / 2);

    glutCreateWindow("Space Invaders");
    glutDisplayFunc(&render);
    glutTimerFunc(DELAY, &updatePlayer, 0);
    glutKeyboardFunc(&keyPress);
    glutSpecialFunc(&specialKeyPress);
    glutSpecialUpFunc(&specialKeyUp);

    lives = 3;
    shot = false;
    playerX = WINDOW_W / 2.f;
    invader_spawnAll();
    glutMainLoop();

    invader_destroyAll();
}

int main(int argc, char *argv[]) {
    start(&argc, argv);
    return 0;
}

