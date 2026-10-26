#include <GL/gl.h>
#include <GL/freeglut.h>

#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#include <float.h>
#include <string.h>
#include <time.h>

#include "audio/audio.h"
#include "game/game.h"
#include "game/player.h"

#include "renderer/raycaster.h"
#include "renderer/textures.h"
#include "renderer/sprites.h"
#include "editor/editor.h"
#include "input/input.h"
#include "renderer/renderer.h"


int main(int argc, char* argv[]){
     srand((unsigned int)time(NULL));

     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
     glutInitWindowSize(1920, 1080);
     glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH) / 2 - 1920 / 2, glutGet(GLUT_SCREEN_HEIGHT) / 2 - 1080 / 2);
     glutCreateWindow("Maze3D");
     gluOrtho2D(0,1920,1080,0);
     
     init();
     audioInit();
     glutCloseFunc(game_shutdown);
     glutMouseFunc(mouse);
     glutMotionFunc(MouseMove);
     glutPassiveMotionFunc(MouseLook);
     glutDisplayFunc(display);
     glutReshapeFunc(resize);
     glutKeyboardFunc(ButtonDown);
     glutKeyboardUpFunc(ButtonUp);
     glutMainLoop();
}




