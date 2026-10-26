#include <math.h>

#include "game.h"
#include "player.h"
#include "map.h"

#include "../audio/audio.h"
#include "../editor/editor.h"
#include "../renderer/renderer.h"
#include "../renderer/sprites.h"
#include "../level/level.h"





// int mapX = 35;  // map width
// int mapY = 13;  // map height

//MAP
// int mapW[35*13]; //walls 
// int mapF[35*13]; //floors
// int mapC[35*13]; //ceiling

sprite sp[MAX_SPRITES];
ButtonKeys Keys;


int currentMap = 0; // 0 = walls, 1 = floor, 2 = ceiling
int currrentTexture = 0; // texture id number , 0=empty
int numSprite = 0; // total number of sprites
int enemyCount = 0; //number of enemies
int buttonState = 0; // which UI button is pressed
int dragItem = 0; // indicates whether we are dragging an item or not
int currentLevel = 1; // level that we can save in file


float frame1, frame2, frame_time_ms;
int gameState;
int timer;
float fade;

float enemySoundTimer = 0;

void game_shutdown(void){
    destroy_map(&map);
}


void restore(){
    px=96; 
    py=(map.height - 1) * mapS - mapS/2; 
    pa=90;
    pdx=cos(degToRad(pa)); 
    pdy=-sin(degToRad(pa)); 
    map.walls[map.height * map.width - 2 * map.width + 1] = 0;
    map.walls[2 * map.width - 2] = 0;
    map.floors[2 * map.width - 2] = 15;
}

void game_update(){
    //2d edit
    if(gameState == -1){ glutSetCursor(GLUT_CURSOR_INHERIT);mapEditor();}
    //3d game
    else{
        if (gameState == 0) { 
            //generate random maze
            load_data();
            generate_maze();
            load_maze();
            spawn_enemies();
            restore();
            
            loadEnemySounds();
            fade = 0; timer = 0; gameState = 1;
        }
        else if (gameState == 1) { //init game
            screen(1);
            timer +=1 * frame_time_ms; 
            if (timer > 3000){fade = 0;timer = 0; gameState = 2;}    
        }
        

        else if (gameState == 2)
        {
            //buttons
            glutSetCursor(GLUT_CURSOR_NONE);
            player_move();
            sprite_update();
            render();


            if (map.floors[((int)px >> 6 ) + ((int) py >> 6) * map.width] == 15){
                alSourcePlay(winSource);
                fade = 0; timer = 0; gameState = 3;
            }
            
            
        }
        else if (gameState == 3) { //won screen
            
            screen(2);
            
            timer +=1 * frame_time_ms; 
            if (timer > 2000){
                fade = 0;timer = 0; gameState = 0;
                if(currentLevel == 3){
                    gameState = -1;
                    currentLevel = 1;
                }else{
                    currentLevel++;
                    gameState = 0;
                }
                
            }    
        }
        else if (gameState == 4) { //lose screen
            //reset player position
            restore();
            // px=96; 
            // py=(map.height - 1) * 64 - 32; 
            screen(3);
            timer +=1 * frame_time_ms; 
            if (timer > 5000){
                fade = 0;timer = 0; gameState = 0;
                currentLevel = 1;
            }    
        }
    } 
}