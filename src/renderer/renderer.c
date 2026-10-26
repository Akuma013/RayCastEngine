#include <GL/freeglut.h>
#include <math.h>
#include <float.h>
#include <string.h>

#include "renderer.h"

#ifdef _WIN32
    #include <windows.h>
    typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALEXTPROC)(int);
#else
    #include <GL/glx.h>
    //typedef int (*PFNGLXSWAPINTERVALEXTPROC)(Display*, GLXDrawable, int);
#endif

#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

#include "../game/game.h"
#include "../game/player.h"
#include "../game/map.h"

#include "../audio/audio.h"
#include "../editor/editor.h"
#include "sprites.h"
#include "raycaster.h"

#include "textures.h"
#include "../game/map.h"

//VSYNC
int vsyncEnabled = 1;

void applyVsync(int on){
#ifdef _WIN32
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT =
        (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) wglSwapIntervalEXT(on);
#else
    PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT =
        (PFNGLXSWAPINTERVALEXTPROC) glXGetProcAddress((const GLubyte*)"glXSwapIntervalEXT");
    if (glXSwapIntervalEXT){
        Display *dpy = glXGetCurrentDisplay();
        GLXDrawable drawable = glXGetCurrentDrawable();
        glXSwapIntervalEXT(dpy, drawable, on);
    }
#endif
}


unsigned char frameBuffer[1920*1080*3];
GLuint screenTex;
int map_scale = 8;


void setPixel(int x, int y, int r, int g, int b){
    if (x < 0 || x >= RENDER_WIDTH || y < 0 || y >= RENDER_HEIGHT) return;
    int index = (y * RENDER_WIDTH + x) * 3;
    frameBuffer[index + 0] = r;
    frameBuffer[index + 1] = g;
    frameBuffer[index + 2] = b;
    
    
}

void drawMapRays2D(){
        // Draw Rays
        glLineWidth(2);
        glBegin(GL_LINES);
        glColor3f(0,1, 0);
        for (int i = 0; i < RENDER_WIDTH; i++)
        {
            glVertex2i(px /map_scale, py /map_scale);
            glVertex2i(rayX[i]/map_scale, rayY[i]/map_scale);
        }
        
        glEnd();//draw 2d rays
}
void drawMap2D(){
    int x, y, xo, yo;
    
    for (y = 0; y < map.height; y++){
        for (x = 0; x < map.width; x++){
            if(map.walls[y*map.width + x] > 0)
                glColor3f(1,1,1);
            else
                glColor3f(0,0,0);
            
            xo = mapS / map_scale * x, yo = mapS / map_scale * y;
            glBegin(GL_QUADS);
            glVertex2i(xo        + 1, yo        + 1);
            glVertex2i(xo        + 1, yo + mapS / map_scale - 1);
            glVertex2i(xo + mapS / map_scale - 1, yo + mapS / map_scale - 1);
            glVertex2i(xo + mapS / map_scale - 1, yo        + 1);
            glEnd();
            
        }
        
    }


    
}

 
void drawPlayer2D(){
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS); 
    glVertex2i(px / map_scale,py / map_scale);
    glEnd();

    // glLineWidth(3);
    // glBegin(GL_LINES);
    // glVertex2i(px, py);
    // glVertex2i(px + pdx*20, py + pdy*20);
    // glEnd();
}

//DRAW SKY
void draw_sky(){
    int x, y;
    float qw = (float)RENDER_WIDTH / 120.0f;
    float qh = (float)RENDER_HEIGHT / 80.0f;
    glBegin(GL_QUADS); 
    for (y = 0; y < 40; y++) // 40
    {
        for (x = 0; x < 120; x++) //120
        {
            int xo = (int) pa * 2 - x; if (xo < 0 ){xo += 120;} xo %= 120; //120

            int pixel = (y*120 +  xo ) * 3; //120
            int red = sky[pixel + 0];
            int green = sky[pixel + 1];
            int blue = sky[pixel + 2];
            
            
            for (int py_ = (int)(y * qh); py_ < (int)(y * qh + qh); py_++)
            {
                for (int px_ = (int)(x * qw); px_ < (int)(x * qw + qw); px_++)
                {
                    setPixel(px_, py_, red, green, blue);
                }
            }
        }   
    }
    glEnd();
    
}




//DRAW SCREEN
void screen(int v){
    int x, y;
    int *T;
    if (v == 1){T = title;}
    if (v == 2){T = won;}
    if (v == 3){T = lost;}
    
    //glPointSize(8);
    float qw = 1920.0 / 120.0;  // quad width  = 16
    float qh = 1080.0 / 80.0;   // quad height = 13.5 
    glBegin(GL_QUADS); 
    for (y = 0; y < 80; y++) //80
    {
        for (x = 0; x < 120; x++) //120
        {
            int pixel = (y*120 + x) * 3; //120
            int red = T[pixel + 0] * fade;
            int green = T[pixel + 1] * fade;
            int blue = T[pixel + 2] * fade;
            
            
            glColor3ub(red, green, blue);
            glVertex2i(x*qw ,y * qh ); 
            glVertex2i(x*qw + qw,y * qh ); 
            glVertex2i(x*qw + qw,y * qh + qh); 
            glVertex2i(x*qw ,y * qh + qh ); 
            
        }   
    }
    glEnd();
    if (fade < 1){fade += 0.001*frame_time_ms;}
    if (fade > 1){fade = 1;}
    
    
    
}


//INIT
void init(){
    glClearColor(0.3,0.3,0.3,0);
    glPointSize(8); //AAAAAAA

    if(!map_init(&map, 35, 13)){
        fprintf(stderr, "Failed to allocate map\n");
        return;
    }

    restore();

    glGenTextures(1, &screenTex); //create
    glBindTexture(GL_TEXTURE_2D, screenTex); // choose as current one
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RENDER_WIDTH, RENDER_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //allocate

    // draw style GL_NEAREST for retro look / GL_LINEAR for smoother view ( can make texture blurry )
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // tell GL not to repeat/wrap it at the edges, just clamp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    

    // add textures to gpu
    // glEnable(GL_TEXTURE_2D);
    // glGenTextures(number_of_textures, wallTextureID);
    // for (int i = 0; i < 21; i++)
    // {
    //     glBindTexture(GL_TEXTURE_2D, wallTextureID[i]);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, &All_Textures[i * 32 * 32 * 3]);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // }
    
    // glDisable(GL_TEXTURE_2D);


    glutSetCursor(GLUT_CURSOR_NONE);
    


    // sp[1].type = 2; sp[1].state = 1; sp[1].map = 1; sp[1].x = 1.5 * 64; sp[1].y = 4.5 * 64; sp[1].z =  0; // light 1
    // sp[2].type = 2; sp[2].state = 1; sp[2].map = 1; sp[2].x = 3.5 * 64; sp[2].y = 4.5 * 64; sp[2].z =  0; // light 2
    // sp[0].type = 1; sp[0].state = 1; sp[0].map = 0; sp[0].x = 1.5 * 64; sp[0].y = 5 * 64; sp[0].z = 20; // key 
    // sp[1].type = 3; sp[1].state = 1; sp[1].map = 2; sp[1].x = 2.5 * 64; sp[1].y = 2 * 64; sp[1].z = 20; // enemy
    // add color for 2D edit view
    // sp[0].r = 255; sp[0].g = 0; sp[0].b = 0;
    // sp[1].r = 255; sp[1].g = 255; sp[1].b = 0;

    //create perimeter
    int x, y;
    for(y = 0; y < map.height; y++){
        for(x = 0; x < map.width; x++){
            // if(x == 0 || x == mapX - 1 || y == 0 || y == mapY - 1){mapW[y*mapX + x] = 2;}
            map.floors[y*map.width + x] = 5;
            map.ceilings[y*map.width + x] = 8;
        }
    }

    applyVsync(vsyncEnabled);

    player_rotate(0);
    
}




//DISPLAY


void display(){

    //fps
    frame2 = glutGet(GLUT_ELAPSED_TIME); frame_time_ms = (frame2 - frame1); frame1 = glutGet(GLUT_ELAPSED_TIME);
    static float fpsTimer = 0;
    static int frameCount = 0;
    fpsTimer += frame_time_ms;
    frameCount++;
    if (fpsTimer >= 500.0f){
        printf("FPS: %.1f\n", 1000.0f * frameCount / fpsTimer);
        fpsTimer = 0;
        frameCount = 0;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //music and sf
    updateMusic();
    updateListener();
    updatePickupSound();
    updateFootsteps(frame_time_ms / 1000.0f);
    for(int x = 0; x < numSprite; x++){
        
        if(sp[x].type != 3){continue;} //skip if not enemy
        updateEnemySound(x);
    }
    game_update();

    glutPostRedisplay();
    glutSwapBuffers();
    
}

void resize(int w, int h){
    // glutReshapeWindow(1920, 1080); //960, 640
    int actualW = glutGet(GLUT_WINDOW_WIDTH);
    int actualH = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, actualW, actualH);
}


void render(){
    //memset(frameBuffer, 128, sizeof(frameBuffer));
    draw_sky(); 
    // draw_floor2D();
    drawRays2D();
    drawSprite();
    
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, RENDER_WIDTH, RENDER_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);
    // Uploads your whole finished frameBuffer into the already-allocated GPU texture.
    // This is the one bulk transfer per frame — cheap compared to thousands of glVertex2i calls.

    
    glColor3ub(255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2i(0,0);
    glTexCoord2f(1,0); glVertex2i(1920, 0);
    glTexCoord2f(1,1); glVertex2i(1920, 1080);
    glTexCoord2f(0,1); glVertex2i(0, 1080);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    drawMap2D();
    drawMapRays2D();
    drawPlayer2D();
    
}
