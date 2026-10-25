#ifndef EDITOR_H
#define EDITOR_H

void addTextures(int x, int y);
void save();
void load_data();
// void load_map();
void mouse(int button, int state, int x, int y);
void MouseMove(int x, int y);
void drawSquareFromArray(int v, int posX, int posY, int *array, int black);
void drawCurrentTexture(int v, int posX, int posY);
void mapEditor();



#endif