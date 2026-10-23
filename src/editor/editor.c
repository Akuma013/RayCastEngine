#include <stdio.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <float.h>
#include <string.h>

#include "editor.h"


#include "../game/game.h"
#include "../game/player.h"
#include "../renderer/textures.h"

#include "../renderer/raycaster.h"
#include "../renderer/sprites.h"
#include "../level/level.h"
#include "../input/input.h"



void addTextures(int x, int y){
    //convert mouse to larger pixel screen
    x = x/48;
    y = y/48;

    //cant add outside of map area
    if(x > mapX || y > mapY) {return;}

    //add on click or dragging
    int arrayPosition = y * mapX + x;
    if(currentMap == 0) { mapW[arrayPosition] = currrentTexture;}
    if(currentMap == 1) { mapF[arrayPosition] = currrentTexture;}
    if(currentMap == 2) { mapC[arrayPosition] = currrentTexture;}

}

void save(){
    int x,y;
    char fileName[32];
    // snprintf(fileName, sizeof(fileName), "levels/level_%d_data.dat", currentLevel);
    snprintf(fileName, sizeof(fileName), "levels/level_%d.dat", currentLevel);

    FILE *fp = fopen(fileName, "w");
    if(fp == NULL){return;}

    //map width height
    fprintf(fp, "%i,%i,", mapX, mapY);

    //player info
    fprintf(fp, "%f,%f,%f,", px, py, pa);    


    //number of sprites
    fprintf(fp, "%i, ", numSprite);

    //sprites
    for(x = 0; x < numSprite; x++){
        fprintf(fp, "%i,%i,%i, %f,%f,%f, %i,%i,%i,", 
            sp[x].type, sp[x].state, sp[x].map,
            sp[x].x, sp[x].y, sp[x].z,
            sp[x].r, sp[x].g, sp[x].b);
    }

    //walls, floor, ceiling
    for(x = 0; x < mapX * mapY; x++){
        fprintf(fp, "%i,%i,%i,", mapW[x], mapF[x], mapC[x]);
    }


    //enemyCount
    fprintf(fp, "%i, ",enemyCount);

    //close
    fclose(fp);
    //print if saved
    printf("saved level: %i\n", currentLevel);
}


void load(){
    int x,y;
    char fileName[32];
    snprintf(fileName, sizeof(fileName), "levels/level_%d.dat", currentLevel);
    // snprintf(fileName, sizeof(fileName), "levels/level_%d_data.dat", currentLevel);

    FILE *fp = fopen(fileName, "r");

    if(fp == NULL){printf("No file to load\n"); return;}

    char c;
    //map width and height
    fscanf(fp, "%i", &mapX); c = getc(fp);
    fscanf(fp, "%i", &mapY); c = getc(fp);

    //player variables
    fscanf(fp, "%f", &px); c = getc(fp);
    fscanf(fp, "%f", &py); c = getc(fp);
    fscanf(fp, "%f", &pa); c = getc(fp);
    pdx = cos(degToRad(pa));
    pdy = -sin(degToRad(pa));

    //number of sprites
    fscanf(fp, "%i", &numSprite); c = getc(fp);

    //sprites
    for(x = 0; x < numSprite; x++){
        fscanf(fp, "%i", &sp[x].type); c = getc(fp);
        fscanf(fp, "%i", &sp[x].state); c = getc(fp);
        fscanf(fp, "%i", &sp[x].map); c = getc(fp);
        fscanf(fp, "%f", &sp[x].x); c = getc(fp);
        fscanf(fp, "%f", &sp[x].y); c = getc(fp);
        fscanf(fp, "%f", &sp[x].z); c = getc(fp);
        fscanf(fp, "%i", &sp[x].r); c = getc(fp);
        fscanf(fp, "%i", &sp[x].g); c = getc(fp);
        fscanf(fp, "%i", &sp[x].b); c = getc(fp);
    }

    //walls, floors, ceiling
    for(x = 0; x < mapX * mapY; x++){
        
        
        fscanf(fp, "%i", &mapW[x]); c = getc(fp); // not used for now
        fscanf(fp, "%i", &mapF[x]); c = getc(fp);
        fscanf(fp, "%i", &mapC[x]); c = getc(fp);
    }
    

    //enemyCount
    fscanf(fp, "%i",&enemyCount); c = getc(fp);
    //printf("\n\n ENEMY COUNT ==== %i \n\n", enemyCount);
 
    //close
    fclose(fp);
    


    //open maze to read
    // snprintf(fileName, sizeof(fileName), "levels/level_%d_map.dat", currentLevel);
    // fp = fopen(fileName, "r");

    // if(fp == NULL){printf("No file to load\n"); return;}

    // for (x = 0; x < mapX*mapY; x++)
    // {
    //     fscanf(fp, "%i", &mapW[x]); c = getc(fp);
    // }
    // //close
    // fclose(fp);

    //print if loaded
    printf("loaded level: %i\n", currentLevel);

}
void mouse(int button, int state, int x, int y){
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
            if(buttonState == 2){load();} // load
            if(buttonState == 3){ pa += 45; if(pa > 359){pa -= 360;} pdx = cos(degToRad(pa)); pdy = -sin(degToRad(pa)); } //rotate player by 45 degrees
            if(buttonState == 4){
                int s = numSprite;
                sp[s].type = 3; sp[s].state = 1; sp[s].map = 2; sp[s].x = numSprite*32; sp[s].y = 1*32; sp[s].z = 20; //add enemy
                sp[s].r = 255; sp[s].g = 255; sp[s].b = 0;
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
        if(mx + 10 > px && mx - 10 < px && my + 10 > py && my - 10 < py){dragItem = 1;}

        //check all sprites if selected
        for (int i = 0; i < numSprite; i++)
        {
            if(mx + 10 > sp[i].x && mx - 10 < sp[i].x && my + 10 > sp[i].y && my - 10 < sp[i].y){dragItem = i + 10; break;}
        }
        
    }
    //right button up
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP){dragItem = 0;}
    
}


void MouseMove(int x, int y){
    if(gameState == -1) { //player
        if(dragItem == 1){
            px = x * 64/6/8;
            py = y * 64/6/8;
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

        delta = centerX - x;
        player_rotate(delta * sensitivity);
        glutWarpPointer(centerX, centerY);
    }
    
    
}


void drawSquareFromArray(int v, int posX, int posY, int *array, int black){
    if (array[v] == 0){return;} //skip if not wall

    glBegin(GL_POINTS); 
    for (int y = 0; y < 6; y++){
        for (int x = 0; x < 6; x++){
            int pixel = ((x*2) * TEX + (y*2)) + (array[v] ) *TEX*TEX;
            int red = All_Textures[pixel * 3 + 0];
            int green = All_Textures[pixel * 3 + 1];
            int blue = All_Textures[pixel * 3 + 2];
            if (black == 1 && array[v] *TEX*TEX > 0){
                if(currentMap == 2 && mapC[posY*mapX + posX] == 0){
                    red = 255; green = 255; blue = 255;
                }else{
                    red = 0, green = 0, blue = 0;
                }
            }
            glColor3ub(red, green, blue);glVertex2i(x*8 + (8*6*posX) + 4,y*8 + (8*6*posY) + 4); 
            
        }
    }
    glEnd();
    
    
}
void drawCurrentTexture(int v, int posX, int posY){
    int x,y;
    glBegin(GL_POINTS);
     for (int y = 0; y < 16; y++){
        for (int x = 0; x < 16; x++){
            int pixel = (x*16) + (y*16) * TEX + v*TEX*TEX;
            int red = All_Textures[pixel * 3 + 0];
            int green = All_Textures[pixel * 3 + 1];
            int blue = All_Textures[pixel * 3 + 2];
            
            glColor3ub(red, green, blue); glVertex2i(x*8 + posX +  4,y*8 + posY  + 4); 
            
        }
    }
    glEnd();
}

void mapEditor(){
    int x,y,i,j;
    //clear bg
    glBegin(GL_POINTS);
    for (y = 0; y < 135; y++)
    {
        for (x = 0; x < 240; x++)
        {
            glColor3ub(200,220,240);
            glVertex2i(x*8 + 4,y*8 + 4);
        
        }
        
    }
    glEnd();

    //grid
    for (y = 0; y < mapY; y++)
    {
        for (x = 0; x < mapX; x++)
        {
            if(currentMap == 0){drawSquareFromArray(x + y*mapX, x, y, mapW, 0);}
            if(currentMap == 1){drawSquareFromArray(x + y*mapX, x, y, mapF, 0);}
            if(currentMap == 2){drawSquareFromArray(x + y*mapX, x, y, mapC, 0);}
            if(currentMap > 0){drawSquareFromArray(x + y*mapX, x, y, mapW, 1);} //draw black walls
            
        }
        
    }
    // the current texture 
    drawCurrentTexture(currrentTexture, 1784, 0); 

    //buttons
    glBegin(GL_POINTS);
    for (y = 0; y < 64; y++)
    {
        for (x = 0; x < 16; x++)
        {
            int pixel = x + y * 16;
             int red = All_Buttons[pixel * 3 + 0];
            int green = All_Buttons[pixel * 3 + 1];
            int blue = All_Buttons[pixel * 3 + 2];

                 if(buttonState == 1 && y >= 0 && y < 8){red = 0;}
            else if(buttonState == 2 && y >= 8 && y <16){red = 0;}
            else if(buttonState == 3 && y >=16 && y <24){red = 0;}
            else if(buttonState == 4 && y >=24 && y <32){red = 0;}
            else if(buttonState == 5 && y >=32 && y <40){red = 0;}
            else if(buttonState == 6 && y >=40 && y <48){red = 0;}
            else if(buttonState == 7 && y >=48 && y <56){red = 0;}
            else if(buttonState == 8 && y >=56 && y <64){red = 0;}
            

            glColor3ub(red, green, blue);
            glVertex2i((x*8) + (8*223) + 4, y*8 + (8*16) + 4); 

        }
        
    }
    glEnd();
    


    //player
    glBegin(GL_POINTS);
    glColor3ub(0,255,00); glVertex2i(px/64*6*8, py/64*6*8);
    glColor3ub(0,155,00); glVertex2i((px + pdx*16)/64*6*8, (py + pdy*16)/64*6*8);
    glEnd();
    
    glBegin(GL_POINTS);
    for (x = 0; x < numSprite; x++)
    {
        glColor3ub(sp[x].r, sp[x].g, sp[x].b);
        glVertex2i(sp[x].x/64*6*8, sp[x].y/64*6*8);
    }
    glEnd();
    
}

