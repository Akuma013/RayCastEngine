#include <GL/gl.h>
#include <stdlib.h>

#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <float.h>
#include <string.h>


#include "raycaster.h"
#include "textures.h"
#include "renderer.h"

#include "../game/game.h"
#include "../game/player.h"

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
static int   tablesReady = 0;



//CALC DISTANCE OF RAY's PROJECTION (fixes fish eye)
float distance(float ax, float ay, float bx, float by, float ang){
    return cos(degToRad(ang))*(bx-ax) - sin(degToRad(ang))*(by-ay);
}



static inline int shadeAt(float worldDist){
    float s = 1.0f - worldDist / SHADE_FALLOFF;
    if (s < SHADE_MIN) s = SHADE_MIN;
    s *= s;
    return (int)(s * 256.0f);
}

static void initRowTables(void){
    for (int y = 0; y < RENDER_HEIGHT; y++){
        float dy = (y + 0.5f) - HALF_H;                  // distance below the horizon, never 0
        if (dy < 0.5f) dy = 0.5f;
        rowDist[y] = (TEX * (float)RENDER_HEIGHT * 0.5f) / dy;   // = wall projection, so floor meets wall base
        float world = rowDist[y] * mapS / TEX;
        rowWorld[y] = world;
        // float s = 1.0f - world / (3.5f * 64.0f);
        // if (s < 0.35f) s = 0.35f;
        // rowShade[y] = (int)(s * 256.0f);
        // rowShade[y] = shadeAt(world);
    }
    tablesReady = 1;
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

    const int maxDof = mapX + mapY;

        //Check Vewrtical Lines
        disV = FLT_MAX;
        dof = 0;
        float Tan = tan(degToRad(ra));
        if(cos_ra > 0.001)//LOOKING RIGHT
        {
            rx = (((int)px >> 6) << 6) + 64;
            ry = (px - rx) * Tan + py;
            xo = 64;
            yo = -xo * Tan;
        }
        else if(cos_ra < -0.001)// LOOKING LEFT
        {
            rx = (((int)px >> 6) << 6) - 0.01f;
            ry = (px - rx) * Tan + py;
            xo = -64;
            yo = -xo * Tan;
        }
        else//LOOKING STRAIGHT UP OR DOWN
        {
            rx = px;
            ry = py;
            dof = maxDof;
        }
        while(dof < maxDof){
            mx = (int) rx >> 6;
            my = (int) ry >> 6;
            mp = my * mapX + mx;
            if(mx < 0 || mx >= mapX || my < 0 || my >= mapY){
                break; //out of bounds or hit wall
            }
            else if (mapW[mp] > 0)//HIT A WALL
            {
                vmt = mapW[mp];
                dof = maxDof;
                disV = distance(px, py, rx, ry, ra);
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
            ry = (((int)py >> 6) << 6) - 0.01f;
            rx = (py - ry) * Tan + px;
            yo = -64;
            xo = -yo * Tan;
        }
        
        else if(sin_ra < -0.001)// LOOKING DOWN
        {
            ry = (((int)py >> 6) << 6) + 64;
            rx = (py - ry) * Tan + px;
            yo = 64;
            xo = -yo * Tan;
        }
        else//LOOKING STRAIGHT LEFT OR RIGHT
        {
            rx = px;
            ry = py;
            dof = maxDof;
        }
        while(dof < maxDof){
            mx = (int) rx >> 6;
            my = (int) ry >> 6;
            mp = my * mapX + mx;
            if(mx < 0 || mx >= mapX || my < 0 || my >= mapY){
                break; //out of bounds or hit wall
            }
            else if (mapW[mp] > 0)//HIT A WALL
            {
                hmt = mapW[mp];
                dof = maxDof;
                disH = distance(px, py, rx, ry, ra);
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
    if(!tablesReady){initRowTables();}

    const float fov = 60.0f;
    const float ray_step = fov / NUM_RAYS;
    const float texPerCell = (float)TEX / mapS;

    ra = FixAng(pa + 30);
    for(int r = 0; r < NUM_RAYS ; r++){
        RayHit hit = castRay(ra);
        rayX[r] = (int)rx; rayY[r] = (int)ry;
         // horizontal hit first
 
         float rad   = degToRad(ra);
         float cosCa = cos(degToRad(pa - ra)); 
        
        colFx[r] =  cos(rad) / cosCa;
        colFy[r] = -sin(rad) / cosCa;
        colInvCos[r] = 1.0f / cosCa;  
        

        //DRAW 3D WALLS
         // ray's and player's angle difference
        float old_dist = hit.dist;
        hit.dist *= cosCa; // fix fisheye
        if (hit.dist < 1.0f) hit.dist = 1.0f;  
        depth[r] = hit.dist; // save line's depth 

        int lineHFull = (int)(mapS* (float)RENDER_HEIGHT/hit.dist);
        if (lineHFull < 1){                              // no wall hit: nothing to draw
            wallTop[r] = wallBot[r] = HALF_H;
            ra = FixAng(ra - ray_step);
            continue;
        }
    
        float ty_step = TEX / (float)lineHFull;
        float ty_off = 0;         
        int   lineHv  = lineHFull;
        if(lineHv > RENDER_HEIGHT) {ty_off = (lineHv - RENDER_HEIGHT) / 2; lineHv = RENDER_HEIGHT;} //cap at 1080 (line Height)
        int top = HALF_H - (lineHv >> 1); //line offset
        lineH = lineHv; lineOff = top;
        wallTop[r] = top; wallBot[r] = top + lineHv;
        

        //DRAW WALLS
        int y;
        float ty = ty_off * ty_step; // hmt * 32;
        int tx = (int) (hit.wallX * texPerCell) & TEX_MASK;
        if (hit.side == 1)  {if (ra > 180){tx = TEX - 1 - tx;}}
        else                {if (ra > 90 && ra < 270){tx = TEX - 1 - tx;}}
        
        // float distt = hit.dist; if (distt > 7 * 64){distt = 7 * 64;}
        // int shade = (int)((1.0f - distt / (10.0f * 64)) * 256.0f);
        int shade = shadeAt(old_dist);
        const unsigned char *tex = &All_Textures[hit.tex * TEX * TEX * 3];
        

        unsigned ty_fp   = (unsigned)(ty_off * ty_step * 65536.0f);   // 16.16 fixed point
        unsigned step_fp = (unsigned)(ty_step * 65536.0f);
        for (int c = 0; c < COLUMN_W; c++)
        {
            unsigned char *dst = frameBuffer + ((size_t)top * RENDER_WIDTH + (r * COLUMN_W + c)) * 3;
            unsigned ty_fp_local = ty_fp;
            for (y= 0; y < lineHv; y++)
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


    const float px4 = px * texPerCell, py4 = py * texPerCell;
    for (int y = HALF_H; y < RENDER_HEIGHT; y++){
        const float D     = rowDist[y];

        const float W = rowWorld[y];   
        // const int   shade = rowShade[y];
        
        const int   yc    = RENDER_HEIGHT - 1 - y;                    // mirrored ceiling row
        unsigned char *fl = frameBuffer + (size_t)y  * RENDER_WIDTH * 3;
        unsigned char *ce = frameBuffer + (size_t)yc * RENDER_WIDTH * 3;

        for (int r = 0; r < NUM_RAYS; r++){
            int shade = shadeAt(W * colInvCos[r]);
            int doFloor = (y  >= wallBot[r]);
            int doCeil  = (yc <  wallTop[r]);
            if (!doFloor && !doCeil) continue;

            int ix = (int)floorf(px4 + colFx[r] * D);
            int iy = (int)floorf(py4 + colFy[r] * D);
            int cx = ix >> 8, cy = iy >> 8;                           // TEX = 256
            if (cx < 0) cx = 0; else if (cx >= mapX) cx = mapX - 1;
            if (cy < 0) cy = 0; else if (cy >= mapY) cy = mapY - 1;
            // if (cx < 0 || cx >= mapX || cy < 0 || cy >= mapY) continue;   // was reading out of bounds
            int cell = cy * mapX + cx;
            int tofs = ((iy & TEX_MASK) * TEX + (ix & TEX_MASK)) * 3;

           for (int c = 0; c < COLUMN_W; c++)
           {
            int screenX = r * COLUMN_W + c;
            if (doFloor){
                const unsigned char *t = &All_Textures[mapF[cell] * TEX * TEX * 3 + tofs];
                fl[screenX*3+0] = (t[0] * shade) >> 8;
                fl[screenX*3+1] = (t[1] * shade) >> 8;
                fl[screenX*3+2] = (t[2] * shade) >> 8;
            }
            if (doCeil && mapC[cell] > 0){
                const unsigned char *t = &All_Textures[mapC[cell] * TEX * TEX * 3 + tofs];
                ce[screenX*3+0] = (t[0] * shade) >> 8;
                ce[screenX*3+1] = (t[1] * shade) >> 8;
                ce[screenX*3+2] = (t[2] * shade) >> 8;
            }
            // just for check
            // if(doCeil){
            //     if ( mapC[cell] > 0){
            //     const unsigned char *t = &All_Textures[mapC[cell] * TEX * TEX * 3 + tofs];
            //     ce[screenX*3+0] = (t[0] * shade) >> 8;
            //     ce[screenX*3+1] = (t[1] * shade) >> 8;
            //     ce[screenX*3+2] = (t[2] * shade) >> 8;
            //     }else{
            //         ce[screenX*3+0] = 255; ce[screenX*3+1] = 0; ce[screenX*3+2] = 255;
            //     }
                
            // }
           }
           
        }
    }
}
