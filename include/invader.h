
#ifndef INVADER_H
#define INVADER_H

#include <GL/gl.h>
#include <stdbool.h>

#define DELAY (1000/60)
#define WINDOW_W 400
#define WINDOW_H 400

#define INVADER_SIZE 30

#define GRID_INVADER_W 6
#define GRID_INVADER_H 5

#define X_SPEED 1.f

typedef enum {
    NONE,
    SQUARE,
    CIRCLE,
    TRIANGLE,
} InvaderType;

extern bool **invaders;

void invader_spawnAll(void);
void invader_destroyAll(void);
void invader_stop(void);

void invader_update(int val);

void invader_draw(void);

InvaderType invader_checkCollision(GLfloat x, GLfloat y);

void invader_shotPosition(GLfloat *x, GLfloat *y);
void invader_destroyShot(void);


#endif
