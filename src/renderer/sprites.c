#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/freeglut.h>

#include <math.h>

#include "sprites.h"

#include "../game/game.h"
#include "../game/player.h"
#include "../game/map.h"
#include "textures.h"
#include "raycaster.h"

#include "../audio/audio.h"

#include "../weapon/weapon.h"
#include "textures.h"


static inline void sort_sprites(int* order,float* sprite_depth, int order_size){
    for (int i = 1; i < order_size; i++){
    int key = order[i];
    int j = i - 1;
    while (j >= 0 && sprite_depth[order[j]] < sprite_depth[key]){
        order[j + 1] = order[j];
        j--;
    }
    order[j + 1] = key;
    }
}


void sprite_update(){
    int x;
    // if picked up dont draw the sprite
    for (x = 0; x < numSprite; x++)
    {
        if(sp[x].type != 1 || sp[x].state != 1){continue;}

        if (player.px < sp[x].x + 30 && player.px > sp[x].x - 30 &&
            player.py < sp[x].y + 30 && player.py > sp[x].y - 30){
            
            printf("PICKUP!\n");
            updatePickupSound(x);

            sp[x].state = 0;
        }
    }
    
    for (int blt = 0; blt < bullet_count; blt++)
    {
        if(bullet[blt].is_active){
            bullet_update(blt);
        }
    }
    

    for(x = 0; x < numSprite; x++){
        
        if(sp[x].type != 3 || sp[x].state != 1){continue;} //skip if not enemy

        
        // enemy hurts the player
        if (player.px < sp[x].x +38 && player.px > sp[x].x - 38 && player.py < sp[x].y +38 && player.py > sp[x].y - 38){

            
            // alSourcePlay(pickupSource);
            // player.hp -= 10;
            // printf("%d\n", player.hp);
            // printf("HHHHIIIITTTT\n");

            // if(player.hp <= 0){
            //     gameState = 4;
            // }
        
            // alSourcePlay(diedSource);
            // gameState = 4;
            
        }  
        //bullet kills enemy
        for (int blt = 0; blt < bullet_count; blt++)
        {
            if(bullet[blt].is_active){
                check_bullet_collision(blt, x);
            }
        }
        if(sp[x].state == 0){continue;}

        float ex = player.px - sp[x].x, ey = player.py - sp[x].y;
        if (ex*ex + ey*ey > ENEMY_BODY*ENEMY_BODY){
            //add variation to enemy position and speed
            // float ran  = (float) rand() / (float) (RAND_MAX / 0.1);
            float ran = ENEMY_SPEED * (0.8f + 0.4f * ((float)rand() / RAND_MAX));

            int spx = (int)sp[x].x >> 6, spy = (int)sp[x].y >> 6; //normal grid position
            int spx_add = ((int)sp[x].x + 15) >> 6 , spy_add = ((int)sp[x].y + 15) >> 6; //normal grid pos + offset
            int spx_sub = ((int)sp[x].x - 15) >> 6 , spy_sub = ((int)sp[x].y - 15) >> 6; //normal grid pos - offset

            if(sp[x].x > player.px && map.walls[spy*map.width + spx_sub] == 0){sp[x].x -= ran*frame_time_ms;}
            if(sp[x].x < player.px && map.walls[spy*map.width + spx_add] == 0){sp[x].x += ran*frame_time_ms;}
            if(sp[x].y > player.py && map.walls[spy_sub*map.width + spx] == 0){sp[x].y -= ran*frame_time_ms;}
            if(sp[x].y < player.py && map.walls[spy_add*map.width + spx] == 0){sp[x].y += ran*frame_time_ms;} 
        }


        
    }
}


void drawSprite(){

    int x,y, s;
    int scale;
    int t_y;
    float t_x_step, t_y_step;
    
    CS = cos(degToRad(player.pa)), SN = sin(degToRad(player.pa)); //rotate around origin

    static int order[MAX_SPRITES];
    static float relative_x[MAX_SPRITES];
    static float relative_y[MAX_SPRITES];
    static float camera_depth[MAX_SPRITES];

    int n = 0;

    for (s = 0; s < numSprite; s++){
        if (sp[s].state != 1) continue;
        relative_x[s] = sp[s].x - player.px;
        relative_y[s] = sp[s].y - player.py;
        camera_depth[s] = relative_x[s] * CS - relative_y[s] * SN;          // same formula as cameraDepth
        if (camera_depth[s] <= 0.01f) continue;
        order[n] = s;
        n++;
    }

    sort_sprites(order, camera_depth, n);





    for (int i = 0; i < n; i++)
    {
        s = order[i];
        float heightZ = sp[s].z; 

        float dist = sqrtf(relative_x[s] * relative_x[s] + relative_y[s] * relative_y[s]);
        int shade = shadeAt(dist);
        
        float cameraX = relative_y[s] * CS + relative_x[s] * SN;

        const float focalLength = 0.9f * RENDER_WIDTH;  //was 1728.0f //previous magic number 108.0

        float screenX = (cameraX * focalLength /camera_depth[s]) + (RENDER_WIDTH >> 1); //convert to screen x, y  
        float screenY = (heightZ * focalLength /camera_depth[s]) + horizon; 

        
        
        if(sp[s].type != 3){
            scale = SPRITE_HEIGHT*RENDER_HEIGHT/camera_depth[s]; 
            if(scale < 1){continue;} if(scale > RENDER_WIDTH){scale = RENDER_WIDTH;}  // was render width

            t_x_step = (SPRITE_WIDTH - 0.5f) / scale;
            t_y_step = (float)SPRITE_HEIGHT / scale;


            int yStart = (screenY >= RENDER_HEIGHT) ? (int)screenY - RENDER_HEIGHT + 1 : 0;
            int yEnd = scale; if(yEnd > (int)screenY + 1) {yEnd = (int)screenY + 1;}

            int xs = (int)(screenX - (scale >> 1));
            int xa = xs < 1 ? 1 : xs;
            int xb = (xs + scale > RENDER_WIDTH) ? RENDER_WIDTH : xs + scale;

            static int colOff[RENDER_WIDTH];

            for (x = xa; x < xb; x++){
                colOff[x] = (camera_depth[s] < depth[x / COLUMN_W]) 
                ? ((int)((x - xs) * t_x_step) * 3 + sp[s].map * SPRITE_WIDTH * SPRITE_HEIGHT * 3) : -1;                 
            }
            
            unsigned char *row = &frameBuffer[((int)screenY - yStart) * RENDER_WIDTH  * 3];

            for (y = yStart; y < yEnd; y++, row -= RENDER_WIDTH *  3){
                
                t_y = (SPRITE_HEIGHT - 1) - (int)(y * t_y_step);
                if (t_y < 0) t_y = 0;
                const unsigned char *trow = &sprites[t_y * SPRITE_WIDTH * 3];

                unsigned char *dst = row + xa * 3;
                for (x = xa; x < xb; x++, dst+= 3)
                {
                    int c = colOff[x];
                    if (c < 0){continue;}
                    const unsigned char *src = trow + c;
                    if (src[0] == 255 && src[1] == 0 && src[2] == 255) {continue;}
                    dst[0] = (src[0] * shade) >> 8;
                    dst[1] = (src[1] * shade) >> 8;
                    dst[2] = (src[2] * shade) >> 8;                
                }
                
            }
        }
        else{

            scale = ENEMY_HEIGHT*RENDER_HEIGHT/camera_depth[s]; //80
            if(scale < 1){continue;} if(scale > RENDER_WIDTH){scale = RENDER_WIDTH;}  // was render width

            t_x_step = (float)(ENEMY_WIDTH - 0.5f) / (float) scale;
            t_y_step = (float)ENEMY_HEIGHT / (float)scale;
            
            int yStart = (screenY >= RENDER_HEIGHT) ? (int)screenY - RENDER_HEIGHT + 1 : 0;
            int yEnd = scale; if(yEnd > (int)screenY + 1) {yEnd = (int)screenY + 1;}
           
            int xs = (int)(screenX - (scale >> 1));
            int xa = xs < 1 ? 1 : xs;
            int xb = (xs + scale > RENDER_WIDTH) ? RENDER_WIDTH : xs + scale;

            static int colOff[RENDER_WIDTH];

            for (x = xa; x < xb; x++){
                colOff[x] = (camera_depth[s] < depth[x / COLUMN_W]) 
                ? ((int)((x - xs) * t_x_step) * 3 + txt_map * ENEMY_WIDTH * ENEMY_HEIGHT * 3) : -1;                 
            }
        
            unsigned char *row = &frameBuffer[((int)screenY - yStart) * RENDER_WIDTH  * 3];

            for (y = yStart; y < yEnd; y++, row -= RENDER_WIDTH * 3){

                t_y = (ENEMY_HEIGHT - 1) - (int)(y * t_y_step);
                if (t_y < 0) t_y = 0;
                const unsigned char *trow = &goofillet[t_y * ENEMY_WIDTH * 3];

                // int col = (int)t_x * 3 + txt_map * ENEMY_HEIGHT * ENEMY_WIDTH * 3;
                
                unsigned char *dst = row + xa * 3;
                for (x = xa; x < xb; x++, dst+= 3)
                {
                    int c = colOff[x];
                    if (c < 0){continue;}
                    const unsigned char *src = trow + c;
                    if (src[0] == 255 && src[1] == 0 && src[2] == 255) {continue;}
                    dst[0] = (src[0] * shade) >> 8;
                    dst[1] = (src[1] * shade) >> 8;
                    dst[2] = (src[2] * shade) >> 8;                  
                }
                
            }
        }
    }
    
    
}




// void drawSprite(){

//     int x,y, s;
//     int scale;
//     float t_x, t_y;
//     float t_x_step, t_y_step;
//     int pixel;
//     int red, green, blue;
    
//     float CS = cos(degToRad(player.pa)), SN = sin(degToRad(player.pa)); //rotate around origin

//     if(frame_timer <= 0){
//             txt_map++;
//             txt_map %= 4;
//             frame_timer = 100.0f;
//     }
//     for (s = 0; s < numSprite; s++)
//     {
//         if(sp[s].state != 1){continue;}

//         float relativeX = sp[s].x - player.px; // temp float vars
//         float relativeY = sp[s].y - player.py;
//         float heightZ = sp[s].z; 

//         float dist = sqrtf(relativeX * relativeX + relativeY * relativeY);
//         int shade = shadeAt(dist);
        
//         float cameraX = relativeY * CS + relativeX * SN;
//         float cameraDepth = relativeX * CS - relativeY * SN;

        
//         if (cameraDepth <= 0.01f) continue;

//         const float focalLength = 0.9f * RENDER_WIDTH;  //was 1728.0f

//         float screenX = (cameraX * focalLength /cameraDepth) + (RENDER_WIDTH/2); //convert to screen x, y   prev  120,80
//         float screenY = (heightZ * focalLength /cameraDepth) + (RENDER_HEIGHT/2); //previous magic number 108.0

        
        
//         if(sp[s].type != 3){
//             scale = SPRITE_HEIGHT*RENDER_HEIGHT/cameraDepth; //80
//             if(scale < 1){continue;} if(scale > RENDER_WIDTH){scale = RENDER_WIDTH;}  // was render width

//             t_x = 0, t_y = SPRITE_HEIGHT - 1;
//             t_x_step = (float)(SPRITE_WIDTH - 0.5f) / (float) scale;
//             t_y_step = (float)SPRITE_HEIGHT / (float)scale;


//             int yStart = (screenY >= RENDER_HEIGHT) ? (int)screenY - RENDER_HEIGHT + 1 : 0;
//             int yEnd = scale; if(yEnd > (int)screenY + 1) {yEnd = (int)screenY + 1;}

//             static int rowOff[RENDER_WIDTH + 1];

//             for (y = yStart; y < yEnd; y++)
//             {
//                 t_y = (SPRITE_HEIGHT - 1) - (int)(y * t_y_step);
//                 if (t_y < 0) t_y = 0;
//                 rowOff[y] = t_y * SPRITE_WIDTH * 3;
//             }
            
            
//             for ( x = screenX - scale/2; x < screenX + scale/2 ; x++){
//                 if (x <= 0 || x >= RENDER_WIDTH || cameraDepth <= 0 || cameraDepth >= depth[x /COLUMN_W]){
//                     t_x += t_x_step;   // keep the texture column aligned
//                     continue;
//                 }
//                 int col = (int)t_x * 3 + sp[s].map * SPRITE_HEIGHT * SPRITE_WIDTH * 3;
//                 unsigned char *dst = &frameBuffer[(((int)screenY - yStart) * RENDER_WIDTH + x) * 3];

//                 for (y = yStart; y < yEnd; y++, dst -= RENDER_WIDTH *  3)
//                 {
//                     // pixel = ((int)t_y*SPRITE_WIDTH + (int) t_x ) * 3 + (sp[s].map * SPRITE_HEIGHT*SPRITE_WIDTH*3);
//                     unsigned char *src = &sprites[rowOff[y] + col];
//                     if (src[0] == 255 && src[1] == 0 && src[2] == 255) {continue;}
//                     dst[0] = (src[0] * shade) >> 8;
//                     dst[1] = (src[1] * shade) >> 8;
//                     dst[2] = (src[2] * shade) >> 8;                
//                 }
//                 t_x += t_x_step;
                
//             }
//         }
//         else{

//             scale = ENEMY_HEIGHT*RENDER_HEIGHT/cameraDepth; //80
//             if(scale < 1){continue;} if(scale > RENDER_WIDTH){scale = RENDER_WIDTH;}  // was render width

//             t_x = 0, t_y = ENEMY_HEIGHT - 1;
//             t_x_step = (float)(ENEMY_WIDTH - 0.5f) / (float) scale;
//             t_y_step = (float)ENEMY_HEIGHT / (float)scale;
            
//             int yStart = (screenY >= RENDER_HEIGHT) ? (int)screenY - RENDER_HEIGHT + 1 : 0;
//             int yEnd = scale; if(yEnd > (int)screenY + 1) {yEnd = (int)screenY + 1;}

//             static int rowOff[RENDER_WIDTH + 1];

            

//              for (y = yStart; y < yEnd; y++)
//             {
//                 t_y = (ENEMY_HEIGHT - 1) - (int)(y * t_y_step);
//                 if (t_y < 0) t_y = 0;
//                 rowOff[y] = t_y * ENEMY_WIDTH * 3;
//             }
//             for ( x = screenX - scale/2; x < screenX + scale/2 ; x++){

//                  if (x <= 0 || x >= RENDER_WIDTH || cameraDepth <= 0 || cameraDepth >= depth[x /COLUMN_W]){
//                     t_x += t_x_step;   // keep the texture column aligned
//                     continue;
//                 }

//                 int col = (int)t_x * 3 + txt_map * ENEMY_HEIGHT * ENEMY_WIDTH * 3;
//                 unsigned char *dst = &frameBuffer[(((int)screenY - yStart) * RENDER_WIDTH + x) * 3];
                
//                 for (y = yStart; y < yEnd; y++, dst -= RENDER_WIDTH * 3)
//                 {
//                     unsigned char *src = &goofillet[rowOff[y] + col];
//                     if (src[0] == 255 && src[1] == 0 && src[2] == 255) {continue;}
//                     dst[0] = (src[0] * shade) >> 8;
//                     dst[1] = (src[1] * shade) >> 8;
//                     dst[2] = (src[2] * shade) >> 8;                  
//                 }
//                 t_x += t_x_step;
                
//             }
//         }
//     }
    
    
// }







