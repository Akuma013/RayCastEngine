#ifndef GAME_H
#define GAME_H

#define M_PI 3.14159265358979323846

extern int mapX;  // map width
extern int mapY;  // map height
#define mapS 64 // map cube size
#define MAX_SPRITES 32

//MAP
extern int mapW[35*13]; //walls 
extern int mapF[35*13]; //floors
extern int mapC[35*13]; //ceiling


extern int currentMap; // 0 = walls, 1 = floor, 2 = ceiling
extern int currrentTexture; // texture id number , 0=empty
extern int numSprite; // total number of sprites
extern int enemyCount; //number of enemies
extern int buttonState; // which UI button is pressed
extern int dragItem; // indicates whether we are dragging an item or not
extern int currentLevel; // level that we can save in file

extern float enemySoundTimer;


extern float frame1, frame2, frame_time_ms;
extern int gameState;
extern int timer;
extern float fade;

typedef struct
{
    int type; //static, key, enemy
    int state; //on/off
    int map; //texture to show
    float x, y, z; //position 
    int r, g, b; //color
}sprite;

typedef struct
{
    int w, a, d, s;

}ButtonKeys;


void game_update();
void restore();

extern sprite sp[MAX_SPRITES];
extern ButtonKeys Keys;



#endif