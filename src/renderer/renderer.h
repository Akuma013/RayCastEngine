#ifndef RENDERER_H
#define RENDERER_H
#include <GL/freeglut.h>

#define RENDER_WIDTH 960
#define RENDER_HEIGHT 540
void drawMap2D();
void drawMapRays2D();
void drawPlayer2D();
void draw_sky();
void screen(int v);
void init();
void display();
void resize(int w, int h);
void applyVsync(int on);

void render();
void setPixel(int x, int y, int r, int g, int b);

extern int map_scale;
extern unsigned char frameBuffer[1920*1080*3];
extern GLuint screenTex;
extern int vsyncEnabled;

#endif
