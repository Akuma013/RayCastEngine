#include <GL/glut.h>

#include "input.h"
#include "../game/game.h"
#include "../game/player.h"
#include "../audio/audio.h"

#include "../renderer/renderer.h"

float delta;
float sensitivity = 0.03f;
int centerX = 1920 / 2;
int centerY = 1080 / 2;



void ButtonDown(unsigned char key, int x, int y){
    if (key == 'a'){Keys.a = 1;}
    if (key == 'd'){Keys.d = 1;}
    if (key == 'w'){Keys.w = 1;}
    if (key == 's'){Keys.s = 1;}
    if (key == 'e' && sp[0].state == 0){ //open doors
        int xo = 0; if (pdx < 0) {xo = -25;} else {xo = 25;}
        int yo = 0; if (pdy < 0) {yo = -25;} else {yo = 25;}
        int ipx = px/64; int ipx_add_xo = (px + xo) / 64;
        int ipy = py/64; int ipy_add_yo = (py + yo) / 64; 
        if (mapW[ipy_add_yo * mapX + ipx_add_xo] == 4) {
            mapW[ipy_add_yo * mapX + ipx_add_xo] = 0;

            // Find the center of the door tile
            float doorX = (ipx_add_xo * 64.0f + 32.0f) / 64.0f;
            float doorY = (ipy_add_yo * 64.0f + 32.0f) / 64.0f;
            alSource3f(
                openDoorSource,
                AL_POSITION,
                doorX,
                doorY,
                0.0f
            );

            alSourcePlay(openDoorSource);
        }
        
    }

    //quickly set current texture to empty
    if(key == 'q'){currrentTexture = 0;}
    //spacebar to return 2d edit view
    if(key == 32){gameState = -1;}
    
    
    
    glutPostRedisplay();
}

void ButtonUp(unsigned char key, int x, int y){
    if (key == 'a'){Keys.a = 0;}
    if (key == 'd'){Keys.d = 0;}
    if (key == 'w'){Keys.w = 0;}
    if (key == 's'){Keys.s = 0;}
    glutPostRedisplay();
}

void MouseLook(int x, int y){
    if(gameState == 2){
        if (x == centerX && y == centerY) return;

        delta = centerX - x;
        player_rotate(delta * sensitivity);
        glutWarpPointer(centerX, centerY);
    }
    
    
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}



