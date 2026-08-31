#include <GL/gl.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <float.h>
 
#include "Textures/All_Textures.txt"
#include "Textures/sky.ppm"
#include "Textures/title.ppm"
#include "Textures/won.ppm"
#include "Textures/lost.ppm"
#include "Textures/sprites.txt"



#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define mapX 8  // map width
#define mapY 8  // map height
#define mapS 64 // map cube size





//MAP
int mapW[] = {
    1,1,1,1,1,3,1,1,
    6,0,0,1,0,0,0,1,
    1,0,0,4,0,2,0,1,
    1,5,4,5,0,0,0,1,
    2,0,0,0,0,0,0,1,
    2,0,0,0,0,1,0,1,
    2,0,0,0,0,0,0,1,
    1,1,3,1,3,1,3,1,

};

int mapF[] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,1,1,1,0,
    0,0,0,0,2,0,1,0,
    0,0,0,0,1,1,1,0,
    0,0,2,0,0,0,0,0,
    0,0,2,0,0,0,0,0,
    0,1,1,1,1,0,0,0,
    0,0,0,0,0,0,0,0,
};

int mapC[] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,    
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,1,0,
    0,1,3,1,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};




//PLAYER
float degToRad(float a) { return a*M_PI/180.0;}
float FixAng(float a){ if(a>359){ a-=360;} if(a<0){ a+=360;} return a;}



float px, py, pdx, pdy, pa; //player coordinates

float frame1, frame2, fps;
int gameState = 0;
int timer = 0;
float fade = 0;

int currentMap = 0; // 0 = walls, 1 = floor, 2 = ceiling
int currrentTexture = 0; // texture id number , 0=empty




//CALC DISTANCE OF RAY's PROJECTION (fixes fish eye)
float distance(float ax, float ay, float bx, float by, float ang){
    return cos(degToRad(ang))*(bx-ax) - sin(degToRad(ang))*(by-ay);
}

typedef struct
{
    int w, a, d, s;

}ButtonKeys;
ButtonKeys Keys;

typedef struct
{
    int type; //static, key, enemy
    int state; //on/off
    int map; //texture to show
    float x, y, z; //position 
}sprite;
sprite sp[4];

int depth[120];

void drawSquareFromArray(int v, int posX, int posY, int *array, int black){
    if (array[v] == 0){return;} //skip if not wall

    for (int y = 0; y < 6; y++){
        for (int x = 0; x < 6; x++){
            int pixel = ((x*2) * 32 + (y*2)) + (array[v] -1 ) *32*32;
            int red = All_Textures[pixel * 3 + 0];
            int green = All_Textures[pixel * 3 + 1];
            int blue = All_Textures[pixel * 3 + 2];
            if (black == 1 && array[v] *32*32 > 0){red = 0, green = 0, blue = 0;}
            glColor3ub(red, green, blue);glBegin(GL_POINTS); glVertex2i(x*8 + (8*6*posX) + 4,y*8 + (8*6*posY) + 4); glEnd();
            
        }
    }
    
    
}
void drawCurrentTexture(int v, int posX, int posY){
    int x,y;

     for (int y = 0; y < 16; y++){
        for (int x = 0; x < 16; x++){
            int pixel = (x*2) + (y*2) * 32 + v*32*32;
            int red = All_Textures[pixel * 3 + 0];
            int green = All_Textures[pixel * 3 + 1];
            int blue = All_Textures[pixel * 3 + 2];
            
            glColor3ub(red, green, blue);glBegin(GL_POINTS); glVertex2i(x*8 + posX +  4,y*8 + posY  + 4); glEnd();
            
        }
    }
}

void mapEditor(){
    int x,y,i,j;
    //clear bg
    for (y = 0; y < 80; y++)
    {
        for (x = 0; x < 120; x++)
        {
            glColor3ub(200,220,240);
            glBegin(GL_POINTS);
            glVertex2i(x*8 + 4,y*8 + 4);
            glEnd();

        }
        
    }

    //grid
    for (y = 0; y < mapY; y++)
    {
        for (x = 0; x < mapX; x++)
        {
            if(currentMap == 0){drawSquareFromArray(x + y*mapX, x, y, mapW, 0);}
            if(currentMap == 1){drawSquareFromArray(x + y*mapX, x, y, mapF, 0);}
            if(currentMap == 2){drawSquareFromArray(x + y*mapX, x, y, mapC, 0);}
            if(currentMap > 0){drawSquareFromArray(x + y*mapX, x, y, mapW, 1);} //draw black walls
            drawCurrentTexture(currrentTexture, 832, 0); //the current texture
        }
        
    }
    
    
}



void drawSprite(){

    int x,y, s;

    if (px < sp[0].x +30 && px > sp[0].x -30 && py < sp[0].y +30 && py > sp[0].y -30){sp[0].state = 0;} // if picked up dont draw the sprite
    if (px < sp[3].x +30 && px > sp[3].x -30 && py < sp[3].y +30 && py > sp[3].y -30){gameState = 4;}  // enemy kills the player


    int spx = (int)sp[3].x >> 6, spy = (int)sp[3].y >> 6; //normal grid position
    int spx_add = ((int)sp[3].x + 15) >> 6 , spy_add = ((int)sp[3].y + 15) >> 6; //normal grid pos + offset
    int spx_sub = ((int)sp[3].x - 15) >> 6 , spy_sub = ((int)sp[3].y - 15) >> 6; //normal grid pos - offset

    if(sp[3].x > px && mapW[spy*8 + spx_sub] == 0){sp[3].x -= 0.06*fps;}
    if(sp[3].x < px && mapW[spy*8 + spx_add] == 0){sp[3].x += 0.06*fps;}
    if(sp[3].y > py && mapW[spy_sub*8 + spx] == 0){sp[3].y -= 0.06*fps;}
    if(sp[3].y < py && mapW[spy_add*8 + spx] == 0){sp[3].y += 0.06*fps;} 


    for (s = 0; s < 4; s++)
    {
        float sx = sp[s].x - px; // temp float vars
        float sy = sp[s].y - py;
        float sz = sp[s].z; 

        float CS = cos(degToRad(pa)), SN = sin(degToRad(pa)); //rotate around origin
        float a = sy * CS + sx * SN;
        float b = sx * CS - sy * SN;
        sx = a, sy = b;

        sx = (sx * 108.0 /sy) + (120/2); //convert to screen x, y   
        sy = (sz * 108.0 /sy) + (80/2);

        
        int scale = 32*80/b;
        if(scale < 0){scale = 0;} if(scale > 120){scale = 120;} 

        float t_x = 0, t_y = 31;
        float t_x_step = 31.5 / (float) scale, t_y_step = 32.0 / (float)scale;


        for ( x = sx - scale/2; x < sx + scale/2 ; x++){
            t_y = 31;
            for (y = 0; y < scale; y++)
            {
                if (sp[s].state == 1 && x > 0 && x < 120 && b > 0 && b < depth[x]){
                    int pixel = ((int)t_y*32 + (int) t_x ) * 3 + (sp[s].map * 32*32*3);
                    int red = sprites[pixel + 0];
                    int green = sprites[pixel + 1];
                    int blue = sprites[pixel + 2];
                    if (!(red == 255 && green == 0 && blue == 255))
                    {
                        glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(x*8, sy*8 - y* 8);glEnd();
                    }
                    
                    t_y -= t_y_step;
                    if (t_y < 0){t_y = 0;}
                    
                }
            }
            t_x += t_x_step;
            
        }
    }
    
    

    

    
}





//RAYS
void drawRays2D(){

    int r, mx, my, mp, dof;
    float vx, vy; //reserve for drawing (to take shortest ray)
    float rx, ry, ra, xo, yo;
    float disH, disV;

    ra = FixAng(pa + 30); // ray set back 30 degrees
    for(r = 0; r<120 ; r++){
        int vmt = 0; int hmt = 0;

        //Check Vewrtical Lines
        disV = FLT_MAX;
        dof = 0;
        float Tan = tan(degToRad(ra));
        if(cos(degToRad(ra)) > 0.001)//LOOKING RIGHT
        {
            rx = (((int)px >> 6) << 6) + 64;
            ry = (px - rx) * Tan + py;
            xo = 64;
            yo = -xo * Tan;
        }
        else if(cos(degToRad(ra)) < -0.001)// LOOKING LEFT
        {
            rx = (((int)px >> 6) << 6) - 0.0001;
            ry = (px - rx) * Tan + py;
            xo = -64;
            yo = -xo * Tan;
        }
        else//LOOKING STRAIGHT UP OR DOWN
        {
            rx = px;
            ry = py;
            dof = 8;
        }
        while(dof < 8){
            mx = (int) rx >> 6;
            my = (int) ry >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && mapW[mp] > 0)//HIT A WALL
            {
                vmt = mapW[mp] - 1;
                dof = 8;
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
        if(sin(degToRad(ra))> 0.001)//LOOKING UP
        {
            ry = (((int)py >> 6) << 6) - 0.0001;
            rx = (py - ry) * Tan + px;
            yo = -64;
            xo = -yo * Tan;
        }
        
        else if(sin(degToRad(ra))< -0.001)// LOOKING DOWN
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
            dof = 8;
        }
        while(dof < 8){
            mx = (int) rx >> 6;
            my = (int) ry >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && mapW[mp] > 0)//HIT A WALL
            {
                hmt = mapW[mp] - 1;
                dof = 8;
                disH = distance(px, py, rx, ry, ra);
            }
            else{
                rx += xo;
                ry += yo;
                dof++;
            }
        }

        float shade = 1;
        glColor3f(0, 0.8, 0);
        if (disV < disH)
        {
            hmt = vmt;
            shade = 0.5;
            rx = vx;
            ry = vy;
            disH = disV;
            glColor3f(0, 0.6, 0);
        } // horizontal hit first


        //DRAW 3D WALLS
        int ca=FixAng(pa-ra); // ray's and player's angle difference
        disH = disH * cos(degToRad(ca)); // fix fisheye

        int lineH = (mapS*640)/disH; 
        float ty_step = 32.0 / (float)lineH;
        float ty_off = 0;
        if(lineH > 640) {ty_off = (lineH - 640) / 2.0; lineH = 640;} //cap at 320 (line Height)
        int lineOff = 320 - (lineH >> 1); //line offset
        
        depth[r] = disH; // save line's depth 

        //DRAW WALLS
        int y;
        float ty = ty_off * ty_step; // hmt * 32;
        float tx;
        if (shade == 1){
            tx = (int) (rx / 2.0) % 32; if (ra > 180){tx = 31 - tx;}
        }else{
            tx = (int) (ry / 2.0) % 32; if (ra > 90 && ra < 270){tx = 31 - tx;}
        }
        
        for (y= 0; y < lineH; y++)
        {
            int pixel = ((int)ty*32 + (int) tx ) * 3 + (hmt*32*32*3);
            int red = All_Textures[pixel + 0]*shade;
            int green = All_Textures[pixel + 1]*shade;
            int blue = All_Textures[pixel + 2]*shade;
            
            glPointSize(8);
            glColor3ub(red, green, blue);
            glBegin(GL_POINTS); 
            glVertex2i(r*8 + 4 ,y + lineOff);
            glEnd();
            ty+= ty_step;
        }
        // DRAW FLOORS
        for  (y = lineOff + lineH; y < 640; y++){
            float dy = y - (640/2.0), deg = degToRad(ra), raFix = cos(degToRad(FixAng(pa - ra)));
            tx = px/2 + cos(deg) * 158 * 2 * 32 / dy /raFix;
            ty = py/2 - sin(deg) * 158 * 2 * 32 / dy /raFix;

            int mp = mapF[(int) (ty / 32.0) * mapX + (int) (tx / 32.0)] * 32 * 32;
            int pixel = ((((int) (ty)) & 31) * 32 + ((int) (tx) & 31)) * 3 + mp * 3;

            int red = All_Textures[pixel + 0] * 0.7;
            int green = All_Textures[pixel + 1] * 0.7;
            int blue = All_Textures[pixel + 2] * 0.7;

            glPointSize(8);
            glColor3ub(red, green, blue);
            glBegin(GL_POINTS); 
            glVertex2i(r*8 + 4 ,y);
            glEnd();
            

            //DRAW ROOFS
    
            mp = mapC[(int) (ty / 32.0) * mapX + (int) (tx / 32.0)] * 32 * 32;
            pixel = ((((int) (ty)) & 31) * 32 + ((int) (tx) & 31)) * 3 + mp * 3;

            red = All_Textures[pixel + 0];
            green = All_Textures[pixel + 1];
            blue = All_Textures[pixel + 2];

           if (mp > 0)
           {
            glPointSize(8);
            glColor3ub(red, green, blue);
            glBegin(GL_POINTS); 
            glVertex2i(r*8 + 4 ,640 - y);
            glEnd();
        
           }
           
        }
        
        
        
        


        ra = FixAng(ra - 0.5); // go to next ray
    }
}

//DRAW SKY
void draw_sky(){
    int x, y;
    for (y = 0; y < 40; y++)
    {
        for (x = 0; x < 120; x++)
        {
            int xo = (int) pa * 2 - x; if (xo < 0 ){xo += 120;} xo %= 120;

            int pixel = (y*120 +  xo ) * 3;
            int red = sky[pixel + 0];
            int green = sky[pixel + 1];
            int blue = sky[pixel + 2];
            
            glPointSize(8);
            glColor3ub(red, green, blue);
            glBegin(GL_POINTS); 
            glVertex2i(x*8 + 4,y * 8 + 4); 
            glEnd();
        }   
    }
    
}




//DRAW SCREEN
void screen(int v){
    int x, y;
    int *T;
    if (v == 1){T = title;}
    if (v == 2){T = won;}
    if (v == 3){T = lost;}
    
    for (y = 0; y < 80; y++)
    {
        for (x = 0; x < 120; x++)
        {
            int pixel = (y*120 + x) * 3;
            int red = T[pixel + 0] * fade;
            int green = T[pixel + 1] * fade;
            int blue = T[pixel + 2] * fade;
            
            glPointSize(8);
            glColor3ub(red, green, blue);
            glBegin(GL_POINTS); 
            glVertex2i(x*8 + 4,y * 8 + 4); 
            glEnd();
        }   
    }
    if (fade < 1){fade += 0.001*fps;}
    if (fade > 1){fade = 1;}
    
    
    
}


//INIT
void init(){
    glClearColor(0.3,0.3,0.3,0);
    glPointSize(8);
    px=150; 
    py=400; 
    pa=90;
    pdx=cos(degToRad(pa)); 
    pdy=-sin(degToRad(pa)); 

    mapW[19] = 4;
    mapW[26] = 4;

    sp[0].type = 1; sp[0].state = 1; sp[0].map = 0; sp[0].x = 1.5 * 64; sp[0].y = 5 * 64; sp[0].z = 20; // key 
    sp[1].type = 2; sp[1].state = 1; sp[1].map = 1; sp[1].x = 1.5 * 64; sp[1].y = 4.5 * 64; sp[1].z =  0; // light 1
    sp[2].type = 2; sp[2].state = 1; sp[2].map = 1; sp[2].x = 3.5 * 64; sp[2].y = 4.5 * 64; sp[2].z =  0; // light 2
    sp[3].type = 3; sp[3].state = 1; sp[3].map = 2; sp[3].x = 2.5 * 64; sp[3].y = 2 * 64; sp[3].z = 20; // enemy
}




//DISPLAY


void display(){

    //fps
    frame2 = glutGet(GLUT_ELAPSED_TIME); fps = (frame2 - frame1); frame1 = glutGet(GLUT_ELAPSED_TIME);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //2d edit
    if(gameState == -1){mapEditor();}
    //3d game
    else{
        if (gameState == 0) {init();fade = 0; timer = 0; gameState = 1;}
        else if (gameState == 1) { //init game
            screen(1);
            timer +=1 * fps; 
            if (timer > 3000){fade = 0;timer = 0; gameState = 2;}    
        }
        

        else if (gameState == 2)
        {
                //buttons
            if(Keys.a == 1){
                pa+=0.2*fps; 
                pa = FixAng(pa);
                pdx = cos(degToRad(pa));
                pdy = -sin(degToRad(pa));
            }
            if(Keys.d == 1){
                pa-=0.2*fps; 
                pa = FixAng(pa);
                pdx = cos(degToRad(pa));
                pdy = -sin(degToRad(pa));
            }

            int xo = 0; if (pdx < 0) {xo = -20;} else {xo = 20;}
            int yo = 0; if (pdy < 0) {yo = -20;} else {yo = 20;}
            int ipx = px/64; int ipx_add_xo = (px + xo) / 64; int ipx_sub_xo = (px - xo) / 64;
            int ipy = py/64; int ipy_add_yo = (py + yo) / 64; int ipy_sub_yo = (py - yo) / 64;
            


            if(Keys.w == 1){
                if (mapW[ipy * mapX + ipx_add_xo] == 0){px += pdx*0.2*fps;}
                if (mapW[ipy_add_yo * mapX + ipx] == 0){py += pdy*0.2*fps;}
            }
            if(Keys.s == 1){
                if (mapW[ipy * mapX + ipx_sub_xo] == 0){px -= pdx*0.2*fps;}
                if (mapW[ipy_sub_yo * mapX + ipx] == 0){py -= pdy*0.2*fps;}
                
                
            }
            

            draw_sky();
            drawRays2D();
            drawSprite();

            if ((int) px >> 6 == 1 && (int) py >> 6 == 1){fade = 0; timer = 0; gameState = 3;}
            
            
        }
        else if (gameState == 3) { //won screen
            screen(2);
            timer +=1 * fps; 
            if (timer > 2000){fade = 0;timer = 0; gameState = 0;}    
        }
        else if (gameState == 4) { //lose screen
            screen(3);
            timer +=1 * fps; 
            if (timer > 2000){fade = 0;timer = 0; gameState = 0;}    
        }
    } 

    glutPostRedisplay();
    glutSwapBuffers();
    
}

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
        if (mapW[ipy_add_yo * mapX + ipx_add_xo] == 4) {mapW[ipy_add_yo * mapX + ipx_add_xo] = 0;}
        
    }
    
    
    
    glutPostRedisplay();
}

void ButtonUp(unsigned char key, int x, int y){
    if (key == 'a'){Keys.a = 0;}
    if (key == 'd'){Keys.d = 0;}
    if (key == 'w'){Keys.w = 0;}
    if (key == 's'){Keys.s = 0;}
    glutPostRedisplay();
}

void resize(int w, int h){
    glutReshapeWindow(960, 640);
}

int main(int argc, char* argv[]){
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
     glutInitWindowSize(960, 640);
     glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH) / 2 - 960 / 2, glutGet(GLUT_SCREEN_HEIGHT) / 2 - 640 / 2);
     glutCreateWindow("RayCast3D");
     gluOrtho2D(0,960,640,0);
     init();
     glutDisplayFunc(display);
     glutReshapeFunc(resize);
     glutKeyboardFunc(ButtonDown);
     glutKeyboardUpFunc(ButtonUp);
     glutMainLoop();
}
