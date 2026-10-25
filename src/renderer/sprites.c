#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "sprites.h"

#include "../game/game.h"
#include "../game/player.h"
#include "textures.h"
#include "raycaster.h"

#include "../audio/audio.h"


void sprite_update(){
    int x;
    // if picked up dont draw the sprite
    if (sp[0].state == 1 &&
        px < sp[0].x + 30 && px > sp[0].x - 30 &&
        py < sp[0].y + 30 && py > sp[0].y - 30)
    {
        printf("PICKUP!\n");

        alSourcePlay(pickupSource);

        sp[0].state = 0;
    }


    for(x = 0; x < numSprite; x++){
        
        if(sp[x].type != 3){continue;} //skip if not enemy

        
        // enemy kills the player
        if (px < sp[x].x +30 && px > sp[x].x -30 && py < sp[x].y +30 && py > sp[x].y -30){

            alSourcePlay(diedSource);
            gameState = 4;
            
        }  

        //add variation to enemy position and speed
        float ran  = (float) rand() / (float) (RAND_MAX / 0.1);

        int spx = (int)sp[x].x >> 6, spy = (int)sp[x].y >> 6; //normal grid position
        int spx_add = ((int)sp[x].x + 15) >> 6 , spy_add = ((int)sp[x].y + 15) >> 6; //normal grid pos + offset
        int spx_sub = ((int)sp[x].x - 15) >> 6 , spy_sub = ((int)sp[x].y - 15) >> 6; //normal grid pos - offset

        if(sp[x].x > px && mapW[spy*mapX + spx_sub] == 0){sp[x].x -= ran*frame_time_ms;}
        if(sp[x].x < px && mapW[spy*mapX + spx_add] == 0){sp[x].x += ran*frame_time_ms;}
        if(sp[x].y > py && mapW[spy_sub*mapX + spx] == 0){sp[x].y -= ran*frame_time_ms;}
        if(sp[x].y < py && mapW[spy_add*mapX + spx] == 0){sp[x].y += ran*frame_time_ms;} 


        
    }
}

void drawSprite(){



    int x,y, s;

    for (s = 0; s < numSprite; s++)
    {

        float sx = sp[s].x - px; // temp float vars
        float sy = sp[s].y - py;
        float sz = sp[s].z; 

        float CS = cos(degToRad(pa)), SN = sin(degToRad(pa)); //rotate around origin
        float a = sy * CS + sx * SN;
        float b = sx * CS - sy * SN;
        sx = a, sy = b;
        const float focalLength = 0.9f * RENDER_WIDTH;  //was 1728.0f
        sx = (sx * focalLength /sy) + (RENDER_WIDTH/2); //convert to screen x, y   prev  120,80
        sy = (sz * focalLength /sy) + (RENDER_HEIGHT/2); //previous magic number 108.0

        
        int scale = SPRITE_HEIGHT*RENDER_HEIGHT/b; //80
        if(scale < 0){scale = 0;} if(scale > RENDER_HEIGHT){scale = RENDER_HEIGHT;}  // was render width

        float t_x = 0, t_y = SPRITE_HEIGHT - 1;
        float t_x_step = (float)(SPRITE_WIDTH - 0.5f) / (float) scale;
        float t_y_step = (float)SPRITE_HEIGHT / (float)scale;

        for ( x = sx - scale/2; x < sx + scale/2 ; x++){
            t_y = 31;
            for (y = 0; y < scale; y++)
            {
                if (sp[s].state == 1 && x > 0 && x < RENDER_WIDTH && b > 0 && b < depth[x /COLUMN_W]){
                    int pixel = ((int)t_y*SPRITE_WIDTH + (int) t_x ) * 3 + (sp[s].map * SPRITE_HEIGHT*SPRITE_WIDTH*3);
                    int red = sprites[pixel + 0];
                    int green = sprites[pixel + 1];
                    int blue = sprites[pixel + 2];
                    if (!(red == 255 && green == 0 && blue == 255))
                    {
                        setPixel(x, sy - y, red, green, blue);
                    }
                    
                    t_y -= t_y_step;
                    if (t_y < 0){t_y = 0;}
                    
                }
            }
            t_x += t_x_step;
            
        }
    }
    
    
}



