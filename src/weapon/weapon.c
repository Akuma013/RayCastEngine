#include <math.h>
#include <stdlib.h>

#include "weapon.h"
#include "../game/game.h"
#include "../game/player.h"
#include "../game/map.h"
#include "../audio/audio.h"
#include "../renderer/renderer.h"

int bullet_count = 0;

Bullet bullet[MAX_BULLETS];


void draw_bullet(){
    
    int center_x = RENDER_WIDTH >> 1;
    int center_y = RENDER_HEIGHT >> 1;

    int x, y;
    for (int bullet_id = 0; bullet_id < bullet_count; bullet_id++)
    {
        if(!bullet[bullet_id].is_active) {continue;}
        float relative_x = bullet[bullet_id].bx - player.px;
        float relative_y = bullet[bullet_id].by - player.py;

        float depth = relative_x * CS - relative_y * SN; if(depth <= 0.01f) {continue;}
        float screen_offset = relative_x * SN + relative_y * CS;

        float focal_length = RENDER_WIDTH * 0.9f;
        float height_z =  0.5;

        float screen_X = center_x + screen_offset * focal_length / depth;
        float screen_Y = center_y + height_z * focal_length / depth;

        int scale = (int)(BULLET_SIZE * focal_length * BULLET_RENDER_FACTOR / depth);
        if(scale < 1){scale = 1;} if(scale > RENDER_WIDTH) {scale = RENDER_WIDTH;}

        int x_start = screen_X - (scale >> 1);
        int x_end = screen_X + (scale >> 1);
        // float tx_step = (BULLET_SIZE /(float)depth);
        // float ty_step = (BULLET_SIZE / (float)depth);

        int y_start = screen_Y;
        int y_end = screen_Y - scale;
        
        if (x_start < 0) x_start = 0;
        if (x_end > RENDER_WIDTH) x_end = RENDER_WIDTH;

        if (y_start >= RENDER_HEIGHT) y_start = RENDER_HEIGHT - 1;
        if (y_end < 0) y_end = 0;

        if (x_start >= x_end || y_start < y_end) continue;

        unsigned char *row = &frameBuffer[((int)y_start) * RENDER_WIDTH * 3];
        
        for (y = y_start; y > y_end; y--, row -= RENDER_WIDTH * 3)
        {
            unsigned char *dst = row + x_start * 3;
            for (x = x_start; x < x_end; x++, dst+= 3)
            {
                dst[0] = 255;
                dst[1] = 255;
                dst[2] = 0;
            }
            
        }
    }
    

}


int create_bullet(){
    if(bullet_count < MAX_BULLETS){

        bullet[bullet_count].is_active = 1;

        bullet[bullet_count].bx = player.px;
        bullet[bullet_count].by = player.py;
        bullet[bullet_count].ba = FixAng(player.pa);
        bullet[bullet_count].speed = 3;
            
        bullet[bullet_count].bdx = cos(degToRad(bullet[bullet_count].ba));
        bullet[bullet_count].bdy = -sin(degToRad(bullet[bullet_count].ba));
    
        bullet_count++;

        return 1;
    }
    printf("OUT OF BULLETS!!!\n");
    return 0;
}

void bullet_update(int blt_indx){
    //!(bullet[blt_indx].bx < 0 || bullet[blt_indx].bx > mapS * map.width || bullet[blt_indx].by > mapS * map.height || bullet[blt_indx].by < 0)
    int mx = bullet[blt_indx].bx / 64;
    int my = bullet[blt_indx].by / 64;
    if(!(map.walls[my*map.width + mx] > 0))
    {
        bullet[blt_indx].bx += bullet[blt_indx].bdx * dt;
        bullet[blt_indx].by += bullet[blt_indx].bdy * dt;
    } else{
        bullet[blt_indx].is_active = 0;
    }
}

int check_bullet_collision(int blt_indx, int enm_indx){
    if(fabs(bullet[blt_indx].bx - sp[enm_indx].x) <= 5 && fabs(bullet[blt_indx].by - sp[enm_indx].y) <= 16){
        sp[enm_indx].state = 0;
        bullet[blt_indx].is_active = 0;
        enemyDeadSound(enm_indx);
        return 1;
    }
    return 0;
}