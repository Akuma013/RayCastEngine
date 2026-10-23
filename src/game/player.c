#include <math.h>

#include "player.h"
#include "game.h"

float px, py, pdx, pdy, pa, sdx, sdy; //player coordinates
//PLAYER
float degToRad(float a) { return a*M_PI/180.0;}
float FixAng(float a){ if(a>359){ a-=360;} if(a<0){ a+=360;} return a;}

void player_rotate(float amount)
{
    pa += amount;
    pa = FixAng(pa);

    pdx = cos(degToRad(pa));
    pdy = -sin(degToRad(pa));

    sdx = cos(degToRad(pa - 90));
    sdy = -sin(degToRad(pa - 90));
}



void player_move(){
     float dt = frame_time_ms > 60 ? 60 : frame_time_ms;

    float mx = 0;
    float my = 0;
    if(Keys.a == 1){mx -= sdx; my -= sdy;}
    if(Keys.d == 1){mx += sdx; my += sdy;}
    if(Keys.w == 1){mx += pdx; my += pdy;}
    if(Keys.s == 1){mx -= pdx; my -= pdy;}

    float len = sqrtf(mx * mx + my * my);
    if(len < 0.0001f){
        return;
    }
    mx /= len; my /= len;

    float dx = mx * 0.2 * dt;
    float dy = my * 0.2 * dt;


    int xo = (mx < 0) ? -20 : 20;
    int yo = (my < 0) ? -20 : 20;
    int cellX = (int)(px + xo) / 64;
    int cellY = (int)(py + yo) / 64;

    if(mapW[(int)(py / 64) * mapX + cellX] == 0) { px += dx;}
    if(mapW[cellY * mapX + (int)(px / 64)] == 0) { py += dy;}
    
    
}

