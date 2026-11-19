#include <GL/gl.h>
#include <stdlib.h>

#include <stdio.h>
#include <GL/freeglut.h>
#include <math.h>
#include <float.h>
#include <string.h>


#include "raycaster.h"
#include "textures.h"
#include "renderer.h"

#include "../game/game.h"
#include "../game/player.h"
#include "../game/map.h"
#include "../input/input.h"

//public
float rx, ry;
float ra;
int lineOff;
int lineH;
int depth[NUM_RAYS]; 
int rayX[NUM_RAYS], rayY[NUM_RAYS];
//private 
static float colFx[NUM_RAYS], colFy[NUM_RAYS];
static float colInvCos[NUM_RAYS];
static int   wallTop[NUM_RAYS], wallBot[NUM_RAYS];
static float rowDist[RENDER_HEIGHT];             // perpendicular floor distance (texture units)
// static int   rowShade[RENDER_HEIGHT];            // 0..256
static float rowWorld[RENDER_HEIGHT];
// static int   tablesReady = 0;

int horizon;

//CALC DISTANCE OF RAY's PROJECTION (fixes fish eye)
float distance(float ax, float ay, float bx, float by, float ang){
    return cos(degToRad(ang))*(bx-ax) - sin(degToRad(ang))*(by-ay);
}



static void initRowTables(void){
    for (int y = 0; y < RENDER_HEIGHT; y++){
        float dy = fabsf((y + 0.5f) - horizon);                  // distance below the horizon, never 0
        if (dy < 0.5f) dy = 0.5f;
        rowDist[y] = (TEX * (float)RENDER_HEIGHT * 0.5f) / dy;   // = wall projection, so floor meets wall base
        float world = rowDist[y] * mapS / TEX;
        rowWorld[y] = world;
        // float s = 1.0f - world / (3.5f * 64.0f);
        // if (s < 0.35f) s = 0.35f;
        // rowShade[y] = (int)(s * 256.0f);
        // rowShade[y] = shadeAt(world);
    }
    // tablesReady = 1;
}

//RAYS
RayHit castRay(float ra){
    
    int mx, my;
    int mp, dof;
    float vx, vy; //reserve for drawing (to take shortest ray)
    float xo, yo;
    float disH, disV;
    int vmt = 0; 
    int hmt = 0;
    float cos_ra = cos(degToRad(ra));    

    const int maxDof = map.width + map.height;

        //Check Vewrtical Lines
        disV = FLT_MAX;
        dof = 0;
        float Tan = tan(degToRad(ra));
        if(cos_ra > 0.001)//LOOKING RIGHT
        {
            rx = (((int)player.px >> 6) << 6) + 64;
            ry = (player.px - rx) * Tan + player.py;
            xo = 64;
            yo = -xo * Tan;
        }
        else if(cos_ra < -0.001)// LOOKING LEFT
        {
            rx = (((int)player.px >> 6) << 6) - 0.01f;
            ry = (player.px - rx) * Tan + player.py;
            xo = -64;
            yo = -xo * Tan;
        }
        else//LOOKING STRAIGHT UP OR DOWN
        {
            rx = player.px;
            ry = player.py;
            dof = maxDof;
        }
        while(dof < maxDof){
            mx = (int) rx >> 6;
            my = (int) ry >> 6;
            mp = my * map.width + mx;
            if(mx < 0 || mx >= map.width || my < 0 || my >= map.height){
                break; //out of bounds or hit wall
            }
            else if (map.walls[mp] > 0)//HIT A WALL
            {
                vmt = map.walls[mp];
                dof = maxDof;
                disV = distance(player.px, player.py, rx, ry, ra);
            }
            else{
                rx += xo;
                ry += yo;
                dof++;
            }
        }
        vx = rx;
        vy = ry;

        //Check Horizontal Lines
        disH = FLT_MAX;
        dof = 0;
        Tan = 1/Tan;
        float sin_ra = sin(degToRad(ra));
        if(sin_ra > 0.001)//LOOKING UP
        {
            ry = (((int)player.py >> 6) << 6) - 0.01f;
            rx = (player.py - ry) * Tan + player.px;
            yo = -64;
            xo = -yo * Tan;
        }
        
        else if(sin_ra < -0.001)// LOOKING DOWN
        {
            ry = (((int)player.py >> 6) << 6) + 64;
            rx = (player.py - ry) * Tan + player.px;
            yo = 64;
            xo = -yo * Tan;
        }
        else//LOOKING STRAIGHT LEFT OR RIGHT
        {
            rx = player.px;
            ry = player.py;
            dof = maxDof;
        }
        while(dof < maxDof){
            mx = (int) rx >> 6;
            my = (int) ry >> 6;
            mp = my * map.width + mx;
            if(mx < 0 || mx >= map.width || my < 0 || my >= map.height){
                break; //out of bounds or hit wall
            }
            else if (map.walls[mp] > 0)//HIT A WALL
            {
                hmt = map.walls[mp];
                dof = maxDof;
                disH = distance(player.px, player.py, rx, ry, ra);
            }
            else{
                rx += xo;
                ry += yo;
                dof++;
            }
        }

        RayHit hit;
        if (disV < disH)
        {
            hit.tex = vmt;
            hit.side = 0.5;
            hit.wallX = vy;
            rx = vx, ry = vy;
            hit.dist = disV;
            // glColor3f(0, 0.6, 0);
        }else{
            hit.tex = hmt;
            hit.side = 1;
            hit.wallX = rx;
            hit.dist = disH;
            // glColor3f(0, 0.8, 0);
        }
        return hit;
}

void drawRays2D(){  
    // if(!tablesReady){initRowTables();}
    horizon = HALF_H + (int)pitch_offs; 
    initRowTables();


    const float fov = 60.0f;
    const float ray_step = fov / NUM_RAYS;
    const float texPerCell = (float)TEX / mapS;

    ra = FixAng(player.pa + 30);
    for(int r = 0; r < NUM_RAYS ; r++){
        RayHit hit = castRay(ra);
        rayX[r] = (int)rx; rayY[r] = (int)ry;
         // horizontal hit first
 
         float rad   = degToRad(ra);
         float cosCa = cos(degToRad(player.pa - ra));  // ray's and player's angle difference
        
        colFx[r] =  cos(rad) / cosCa;
        colFy[r] = -sin(rad) / cosCa;
        colInvCos[r] = 1.0f / cosCa;  
        

        //DRAW 3D WALLS
        float old_dist = hit.dist;
        hit.dist *= cosCa; // fix fisheye
        if (hit.dist < 1.0f) hit.dist = 1.0f;  
        depth[r] = hit.dist; // save line's depth 

        int lineHFull = (int)(mapS* (float)RENDER_HEIGHT/hit.dist);
        if (lineHFull < 1){                              // no wall hit: nothing to draw
            wallTop[r] = wallBot[r] = horizon;
            ra = FixAng(ra - ray_step);
            continue;
        }
    
        float ty_step = TEX / (float)lineHFull;
        // float ty_off = 0;         
        // if(lineHv > RENDER_HEIGHT) {ty_off = (lineHv - RENDER_HEIGHT) / 2; lineHv = RENDER_HEIGHT;} //cap at 1080 (line Height)
        //line offset
        int top_full = horizon - (lineHFull >> 1); 
        int bot_full = top_full + lineHFull;

        int top = top_full < 0 ? 0 : top_full;
        int bot = bot_full > RENDER_HEIGHT ? RENDER_HEIGHT : bot_full;
        int rows = bot - top;
        wallTop[r] = top;
        wallBot[r] = bot;

        if (rows <= 0) {                             //wall completely off screen
            wallTop[r] = wallBot[r] = horizon;
            ra = FixAng(ra - ray_step);
            continue;
        }
        

        //DRAW WALLS
        int y;
        int tx = (int) (hit.wallX * texPerCell) & TEX_MASK;
        if (hit.side == 1)  {if (ra > 180){tx = TEX - 1 - tx;}}
        else                {if (ra > 90 && ra < 270){tx = TEX - 1 - tx;}}
        
        // float distt = hit.dist; if (distt > 7 * 64){distt = 7 * 64;}
        // int shade = (int)((1.0f - distt / (10.0f * 64)) * 256.0f);
        int shade = shadeAt(old_dist);
        const unsigned char *tex = &All_Textures[hit.tex * TEX * TEX * 3];
        

        unsigned ty_fp   = (unsigned)((top - top_full) * ty_step * 65536.0f);   // 16.16 fixed point
        unsigned step_fp = (unsigned)(ty_step * 65536.0f);
        for (int c = 0; c < COLUMN_W; c++)
        {
            unsigned char *dst = frameBuffer + ((size_t)top * RENDER_WIDTH + (r * COLUMN_W + c)) * 3;
            unsigned ty_fp_local = ty_fp;
            for (y= 0; y < rows; y++)
            {
                int ti = (((ty_fp_local >> 16) & TEX_MASK) * TEX + (int) tx) * 3;
                dst[0] = (tex[ti + 0]*shade) >> 8; //shade
                dst[1] = (tex[ti + 1]*shade) >> 8;
                dst[2] = (tex[ti + 2]*shade) >> 8;
                dst += RENDER_WIDTH * 3;
                ty_fp_local += step_fp;       
            }    
        }

        ra = FixAng(ra - ray_step); // go to next ray
    }


    const float px4 = player.px * texPerCell, py4 = player.py * texPerCell;
    for (int y = 0; y < RENDER_HEIGHT; y++){
        const int isFloor = (y >= horizon);
        const float D     = rowDist[y];
        const float W = rowWorld[y];   
        // const int   shade = rowShade[y];
        
        unsigned char *row = frameBuffer + (size_t)y  * RENDER_WIDTH * 3;

        for (int r = 0; r < NUM_RAYS; r++){
            int shade = shadeAt(W * colInvCos[r]);
            int doFloor = isFloor && (y  >= wallBot[r]);
            int doCeil  = !isFloor && (y <  wallTop[r]); //!!!!!!!!!!!!!!
            if (!doFloor && !doCeil) continue;

            int ix = (int)floorf(px4 + colFx[r] * D);
            int iy = (int)floorf(py4 + colFy[r] * D);
            int cx = ix >> 8, cy = iy >> 8;                           // TEX = 256
            if (cx < 0) cx = 0; else if (cx >= map.width) cx = map.width - 1;
            if (cy < 0) cy = 0; else if (cy >= map.height) cy = map.height - 1;
            // if (cx < 0 || cx >= mapX || cy < 0 || cy >= mapY) continue;   // was reading out of bounds
            int cell = cy * map.width + cx;
            int tofs = ((iy & TEX_MASK) * TEX + (ix & TEX_MASK)) * 3;

           for (int c = 0; c < COLUMN_W; c++)
           {
            int screenX = r * COLUMN_W + c;
            if (doFloor){
                const unsigned char *t = &All_Textures[map.floors[cell] * TEX * TEX * 3 + tofs];
                row[screenX*3+0] = (t[0] * shade) >> 8;
                row[screenX*3+1] = (t[1] * shade) >> 8;
                row[screenX*3+2] = (t[2] * shade) >> 8;
            }
            else if (doCeil && map.ceilings[cell] > 0){
                const unsigned char *t = &All_Textures[map.ceilings[cell] * TEX * TEX * 3 + tofs];
                row[screenX*3+0] = (t[0] * shade) >> 8;
                row[screenX*3+1] = (t[1] * shade) >> 8;
                row[screenX*3+2] = (t[2] * shade) >> 8;
            }
           }
           
        }
    }
}
