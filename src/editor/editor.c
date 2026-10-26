#include <stdio.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include <math.h>
#include <float.h>
#include <string.h>

#include "editor.h"


#include "../game/game.h"
#include "../game/player.h"
#include "../game/map.h"
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
    if(x > map.width || y > map.height) {return;}

    //add on click or dragging
    int arrayPosition = y * map.width + x;
    if(currentMap == 0) { map.walls[arrayPosition] = currrentTexture;}
    if(currentMap == 1) { map.floors[arrayPosition] = currrentTexture;}
    if(currentMap == 2) { map.ceilings[arrayPosition] = currrentTexture;}

}

void save(){
    int x,y;
    char fileName[32];
    snprintf(fileName, sizeof(fileName), "levels/level_%d_data.dat", currentLevel);
    //snprintf(fileName, sizeof(fileName), "levels/level_%d.dat", currentLevel);

    FILE *fp = fopen(fileName, "w");
    if(fp == NULL){return;}

    //map width height
    fprintf(fp, "%i,%i,", map.width, map.height);

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
    // for(x = 0; x < mapX * mapY; x++){
    //     fprintf(fp, "%i,%i,%i,", mapW[x], mapF[x], mapC[x]);
    // }


    //enemyCount
    fprintf(fp, "%i, ",enemyCount);

    //close
    fclose(fp);
    //print if saved
    printf("saved level: %i\n", currentLevel);
}


void load_data(){
    int x,y;
    char fileName[32];
    //snprintf(fileName, sizeof(fileName), "levels/level_%d.dat", currentLevel);
    snprintf(fileName, sizeof(fileName), "levels/level_%d_data.dat", currentLevel);

    FILE *fp = fopen(fileName, "r");

    if(fp == NULL){printf("No file to load\n"); return;}

    char c;
    //map width and height
    fscanf(fp, "%i", &map.width); c = getc(fp);
    fscanf(fp, "%i", &map.height); c = getc(fp);

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
    // for(x = 0; x < mapX * mapY; x++){
        
        
    //     fscanf(fp, "%i", &mapW[x]); c = getc(fp); // not used for now
    //     fscanf(fp, "%i", &mapF[x]); c = getc(fp);
    //     fscanf(fp, "%i", &mapC[x]); c = getc(fp);
    // }
    

    //enemyCount
    fscanf(fp, "%i",&enemyCount); c = getc(fp);
    //printf("\n\n ENEMY COUNT ==== %i \n\n", enemyCount);
 
    //close
    fclose(fp);
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
                if(currentMap == 2 && map.ceilings[posY*map.width + posX] == 0){
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
    for (y = 0; y < map.height; y++)
    {
        for (x = 0; x < map.width; x++)
        {
            if(currentMap == 0){drawSquareFromArray(x + y*map.width, x, y, map.walls, 0);}
            if(currentMap == 1){drawSquareFromArray(x + y*map.width, x, y, map.floors, 0);}
            if(currentMap == 2){drawSquareFromArray(x + y*map.width, x, y, map.ceilings, 0);}
            if(currentMap > 0){drawSquareFromArray(x + y*map.width, x, y, map.walls, 1);} //draw black walls
            
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

