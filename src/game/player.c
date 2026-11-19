#include <math.h>

#include "player.h"
#include "game.h"
#include "map.h"

Player player = {
    .px = 0,
    .py = 0,
    .speed = 0.2f,
    .hp = 100
};
float dt; 
float CS, SN;

//PLAYER
float degToRad(float a) { return a*M_PI/180.0;}
float FixAng(float a){ if(a>359){ a-=360;} if(a<0){ a+=360;} return a;}



int blockedByEnemy(float x, float y){
    for (int i = 0; i < numSprite; i++){
        if (sp[i].type != 3 || sp[i].state != 1) continue;

        float newDx = x - sp[i].x,  newDy = y - sp[i].y;   // distance after the move
        float oldDx = player.px - sp[i].x, oldDy = player.py - sp[i].y;  // distance now

        float newDist = newDx*newDx + newDy*newDy;
        float oldDist = oldDx*oldDx + oldDy*oldDy;

        if (newDist < ENEMY_BODY*ENEMY_BODY && newDist < oldDist) return 1;
    }
    return 0;
}


void player_rotate(float amount)
{
    player.pa += amount;
    player.pa = FixAng(player.pa);

    player.pdx = cos(degToRad(player.pa));
    player.pdy = -sin(degToRad(player.pa));

    player.sdx = cos(degToRad(player.pa - 90));
    player.sdy = -sin(degToRad(player.pa - 90));
}



void player_move(){
    dt = frame_time_ms > 60 ? 60 : frame_time_ms;

    float mx = 0;
    float my = 0;
    if(Keys.a == 1){mx -= player.sdx; my -= player.sdy;}
    if(Keys.d == 1){mx += player.sdx; my += player.sdy;}
    if(Keys.w == 1){mx += player.pdx; my += player.pdy;}
    if(Keys.s == 1){mx -= player.pdx; my -= player.pdy;}

    float len = sqrtf(mx * mx + my * my);
    if(len < 0.0001f){
        return;
    }
    mx /= len; my /= len;

    
    player.speed = Keys.shift ? 0.27f : 0.2f;
    float dx = mx * player.speed * dt;
    float dy = my * player.speed * dt;


    int xo = (mx < 0) ? -15 : 15;
    int yo = (my < 0) ? -15 : 15;
    int cellX = (int)(player.px + xo) / 64;
    int cellY = (int)(player.py + yo) / 64;

    if(map.walls[(int)(player.py / 64) * map.width + cellX] == 0 && !blockedByEnemy(player.px + dx, player.py)) { player.px += dx;}
    if(map.walls[cellY * map.width + (int)(player.px / 64)] == 0 && !blockedByEnemy(player.px, player.py + dy)) { player.py += dy;}
    
    
}

