#include <GL/freeglut.h>
#include <math.h>

#include "input.h"
#include "../game/game.h"
#include "../game/player.h"
#include "../game/map.h"
#include "../audio/audio.h"

#include "../renderer/renderer.h"
#include "../editor/editor.h"
#include "../level/level.h"
#include "../weapon/weapon.h"

float delta_x;
float delta_y;
float pitch_offs;
float pitch_sens = 1;
float sensitivity = 0.03f;
int centerX = 1920 / 2;
int centerY = 1080 / 2;
 


void SpecialDown(int key, int x, int y){
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) Keys.shift = 1;
}

void SpecialUp(int key, int x, int y){
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) Keys.shift = 0;
}


void ButtonDown(unsigned char key, int x, int y){

    if (key == 'a' || key == 'A'){Keys.a = 1;}
    if (key == 'd' || key == 'D'){Keys.d = 1;}
    if (key == 'w' || key == 'W'){Keys.w = 1;}
    if (key == 's' || key == 'S'){Keys.s = 1;}
    if ((key == 'e' || key == 'E') && sp[0].state == 0){ //open doors
        int xo = 0; if (player.pdx < 0) {xo = -25;} else {xo = 25;}
        int yo = 0; if (player.pdy < 0) {yo = -25;} else {yo = 25;}
        int ipx = player.px/64; int ipx_add_xo = (player.px + xo) / 64;
        int ipy = player.py/64; int ipy_add_yo = (player.py + yo) / 64; 
        if (map.walls[ipy_add_yo * map.width + ipx_add_xo] == 4) {
            map.walls[ipy_add_yo * map.width + ipx_add_xo] = 0;

            // Find the center of the door tile
            float doorX = (ipx_add_xo * 64.0f + 32.0f) / 64.0f;
            float doorY = (ipy_add_yo * 64.0f + 32.0f) / 64.0f;
            updateDoorSound(doorX, doorY);
        }
        
    }

    //quickly set current texture to empty
    if(key == 'q'){currrentTexture = 0;}
    //spacebar to return 2d edit view
    if(key == 32){gameState = -1;}
    
    
    
    glutPostRedisplay();
}

void ButtonUp(unsigned char key, int x, int y){
    if (key == 'a' || key == 'A'){Keys.a = 0;}
    if (key == 'd' || key == 'D'){Keys.d = 0;}
    if (key == 'w' || key == 'W'){Keys.w = 0;}
    if (key == 's' || key == 'S'){Keys.s = 0;}
    glutPostRedisplay();
}


void mouse(int button, int state, int x, int y){
    if(gameState == -1){
        //Add to array
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            // mouse click add, texture
            addTextures(x, y);


            //click over buttons
            // printf("x:%d, y:%d\n", x, y); //for check
            if(x/8 >= 223){
                if(y/8 < 16 + 8 * 0){currrentTexture++; if(currrentTexture > 15){currrentTexture = 0;} } //next texture
                else if(y/8 >= 16 + 8 * 0 && y/8 < 24 + 8 * 0 ) { if(buttonState > 0) {buttonState = 0;} else {buttonState = 1; } printf("Save\n");}
                else if(y/8 >= 16 + 8 * 1 && y/8 < 24 + 8 * 1 ) { if(buttonState > 0) {buttonState = 0;} else {buttonState = 2; } printf("Load\n");}
                else if(y/8 >= 16 + 8 * 2 && y/8 < 24 + 8 * 2 ) { if(buttonState > 0) {buttonState = 0;} else {buttonState = 3; } printf("Player Rotate\n");}
                else if(y/8 >= 16 + 8 * 3 && y/8 < 24 + 8 * 3 ) { if(buttonState > 0) {buttonState = 0;} else {buttonState = 4; } printf("Add Enemy\n");}
                else if(y/8 >= 16 + 8 * 4 && y/8 < 24 + 8 * 4 ) { if(buttonState > 0) {buttonState = 0;} else {buttonState = 5; } printf("View Walls\n");}
                else if(y/8 >= 16 + 8 * 5 && y/8 < 24 + 8 * 5 ) { if(buttonState > 0) {buttonState = 0;} else {buttonState = 6; } printf("View Floors\n");}
                else if(y/8 >= 16 + 8 * 6 && y/8 < 24 + 8 * 6 ) { if(buttonState > 0) {buttonState = 0;} else {buttonState = 7; } printf("View Ceiling\n");}
                else if(y/8 >= 16 + 8 * 7 && y/8 < 24 + 8 * 7 ) { if(buttonState > 0) {buttonState = 0;} else {buttonState = 8; } printf("Play The Game\n");}
            
                if(buttonState == 1){save();} //save
                if(buttonState == 2){load_data(); load_maze();} // load
                if(buttonState == 3){ player.pa += 45; if(player.pa > 359){player.pa -= 360;} player.pdx = cos(degToRad(player.pa)); player.pdy = -sin(degToRad(player.pa)); } //rotate player by 45 degrees
                if(buttonState == 4){
                    int s = numSprite;
                    sp[s].type = 3; sp[s].state = 1; sp[s].map = 2; sp[s].x = numSprite*32; sp[s].y = 1*32; sp[s].z = 20; //add enemy
                    sp[s].r = 255; sp[s].g = 0; sp[s].b = 0;
                    numSprite++;
                    enemyCount++;
                } 
                if(buttonState == 5){currentMap = 0;} // view walls
                if(buttonState == 6){currentMap = 1;} //view floors
                if(buttonState == 7){currentMap = 2;} // view ceiling
                if(buttonState == 8){gameState = 0;} //start the game
            }
        }
        if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        //toggle buttons
        if (x/8 >= 104) {buttonState = 0;}
            
        }


        //right button down
        if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
            //previous texture
            if(x/8 >= 104 && y/8 < 16){currrentTexture--; if(currrentTexture < 0){currrentTexture = 15;} }


            //convert mouse to grid
            int mx = x*64/6/8;
            int my = y*64/6/8;

            //select player
            if(mx + 10 > player.px && mx - 10 < player.px && my + 10 > player.py && my - 10 < player.py){dragItem = 1;}

            //check all sprites if selected
            for (int i = 0; i < numSprite; i++)
            {
                if(mx + 10 > sp[i].x && mx - 10 < sp[i].x && my + 10 > sp[i].y && my - 10 < sp[i].y){dragItem = i + 10; break;}
            }
            
        }
        //right button up
        if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP){dragItem = 0;}
    }else if(gameState == 2){

        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
            alSourcePlay(weaponSoundSource[1]);
            create_bullet();
            draw_bullet();
            
            
        }
    }
    
}

void MouseLook(int x, int y){
    if(gameState == 2){
        if (x == centerX && y == centerY) return;

        delta_x = centerX - x;
        delta_y = centerY - y;
        pitch_offs += delta_y * pitch_sens;
        if(pitch_offs > 300) {pitch_offs = 300;}
        if(pitch_offs < -300) {pitch_offs = -300;}
        player_rotate(delta_x * sensitivity);
        glutWarpPointer(centerX, centerY);
    }
    
    
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MouseMove(int x, int y){
    if(gameState == -1) { //player
        if(dragItem == 1){
            player.px = x * 64/6/8;
            player.py = y * 64/6/8;
        }
        //move sprites
        else if (dragItem > 1)
        {
            sp[dragItem - 10].x = x *64/6/8;
            sp[dragItem - 10].y = y *64/6/8;
        }
        else if(dragItem == 0){
            addTextures(x, y);
        }
        
        glutPostRedisplay();
    }else if(gameState == 2){
        if (x == centerX && y == centerY) return;

        delta_x = centerX - x;
        delta_y = centerY - y;
        pitch_offs += delta_y * pitch_sens;
        if(pitch_offs > 300) {pitch_offs = 300;}
        if(pitch_offs < -300) {pitch_offs = -300;}
        player_rotate(delta_x * sensitivity);
        glutWarpPointer(centerX, centerY);
    }
    
    
}



