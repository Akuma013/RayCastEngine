#ifndef PLAYER_H
#define PLAYER_H

extern float px, py, pdx, pdy, pa, sdx, sdy; //player coordinates


float degToRad(float a);
float FixAng(float a);
void player_move();
void player_rotate(float amount);

#endif