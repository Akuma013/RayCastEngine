#ifndef PLAYER_H
#define PLAYER_H

extern float dt;

#define ENEMY_BODY 45


extern float CS, SN;

typedef struct {
    float px, py, pdx, pdy, pa, sdx, sdy;
    float speed;
    int hp;
    int sp;
}Player;

extern Player player;


float degToRad(float a);
float FixAng(float a);
void player_move();
void player_rotate(float amount);
int blockedByEnemy(float x, float y);

#endif