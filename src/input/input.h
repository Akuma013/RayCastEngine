#ifndef INPUT_H
#define INPUT_H

extern float delta;
extern float sensitivity;
extern int centerX;
extern int centerY;

void ButtonDown(unsigned char key, int x, int y);
void ButtonUp(unsigned char key, int x, int y);

void MouseLook(int x, int y);
void mouse(int button, int state, int x, int y);
void MouseMove(int x, int y);

#endif