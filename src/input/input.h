#ifndef INPUT_H
#define INPUT_H

extern float delta_x;
extern float delta_y;
extern float pitch_offs;
extern float pitch_sens;

extern float sensitivity;
extern int centerX;
extern int centerY;

void SpecialDown(int key, int x, int y);
void SpecialUp(int key, int x, int y);

void ButtonDown(unsigned char key, int x, int y);
void ButtonUp(unsigned char key, int x, int y);

void MouseLook(int x, int y);
void mouse(int button, int state, int x, int y);
void MouseMove(int x, int y);




#endif