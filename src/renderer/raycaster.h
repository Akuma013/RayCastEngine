#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "renderer.h"
#define NUM_RAYS   (RENDER_WIDTH )          // pick your quality divisor
#define COLUMN_W   (RENDER_WIDTH / NUM_RAYS)   // pixels each ray covers
#define HALF_H   (RENDER_HEIGHT / 2)

#define SHADE_FALLOFF 192.0f   // distance where shade would hit 0
#define SHADE_MIN     0.02f

extern int depth[NUM_RAYS];
extern float rx, ry;
extern float ra;
extern int rayX[NUM_RAYS];
extern int rayY[NUM_RAYS];
extern int lineOff;
extern int lineH;

float distance(float ax, float ay, float bx, float by, float ang);
void drawRays2D();
// void draw_floor2D();


typedef struct {
    float dist;   // corrected (fisheye-fixed) distance to the hit
    int tex;      // which texture id was hit (hmt)
    float wallX;  // rx or ry, whichever is used for texture x-coord
    float side;     // 0 = vertical wall hit, 1 = horizontal wall hit (drives shade)
} RayHit;

RayHit castRay(float ra);

#endif