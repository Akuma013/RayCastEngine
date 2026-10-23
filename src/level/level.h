#ifndef LEVEL_H
#define LEVEL_H

#include "../game/game.h"

#define ROWS (mapY - 1) / 2
#define COLS (mapX - 1) / 2

// The final maze will be 37 x 73
// 1 = wall, 0 = walkable
#define MAZE_HEIGHT (2 * ROWS + 1)
#define MAZE_WIDTH  (2 * COLS + 1)

// Cell in the logical maze
typedef struct {
    int visited;
} Cell;

extern int stackSize;

// Position used by the DFS stack
typedef struct {
    int row;
    int col;
} Position;


// Directions: 0 = UP, 1 = RIGHT, 2 = DOWN, 3 = LEFT
extern int dr[4];
extern int dc[4];

void initialize(Cell cells[ROWS][COLS], Position stack[ROWS*COLS], int maze[MAZE_HEIGHT][MAZE_WIDTH]);
int valid_cell(int row, int col, Cell cells[ROWS][COLS]);
void shuffle_directions(int directions[4]);
void generate_maze(void);
void print_maze(int maze[MAZE_HEIGHT][MAZE_WIDTH]);
void save_maze(int maze[MAZE_HEIGHT][MAZE_WIDTH]);
void load_maze(int maze[MAZE_HEIGHT][MAZE_WIDTH]);




#endif