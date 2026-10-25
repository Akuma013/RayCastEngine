#include "level.h"
#include "../game/game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int stackSize = 0;

// Directions: 0 = UP, 1 = RIGHT, 2 = DOWN, 3 = LEFT
int dr[] = { -1, 0, 1, 0 };
int dc[] = { 0, 1, 0, -1 };


// Initialize the maze
void initialize(Cell cells[ROWS][COLS], Position stack[ROWS*COLS], int maze[MAZE_HEIGHT][MAZE_WIDTH])
{

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            cells[r][c].visited = 0;
        }
    }

    // Initially everything in the final maze is a wall
    for (int r = 0; r < MAZE_HEIGHT; r++)
    {
        for (int c = 0; c < MAZE_WIDTH; c++)
        {
            maze[r][c] = 1;
        }
    }

    stackSize = 0;
}


// Check whether a cell is valid and unvisited
int valid_cell(int row, int col, Cell cells[ROWS][COLS])
{
    if (row < 0 || row >= ROWS)
        return 0;

    if (col < 0 || col >= COLS)
        return 0;

    if (cells[row][col].visited)
        return 0;

    return 1;
}


// Shuffle directions to make the maze random
void shuffle_directions(int directions[4])
{
    for (int i = 3; i > 0; i--)
    {
        int j = rand() % (i + 1);

        int temp = directions[i];
        directions[i] = directions[j];
        directions[j] = temp;
    }
}


// Generate the maze using DFS
void generate_maze(void)
{
    if (mapX % 2 == 0 || mapY % 2 == 0) {
        printf("ERROR: maze generator requires odd mapX/mapY (got %d x %d)\n", mapX, mapY);
        return;
    }   
    // Original logical maze cells
    Cell cells[ROWS][COLS];

    // Final 0/1 maze
    // 1 = wall, 0 = walkable
    int maze[MAZE_HEIGHT][MAZE_WIDTH];

    // DFS stack
    Position stack[ROWS * COLS];

    initialize(cells, stack, maze);
    
    // Start DFS at cell (0, 0)
    cells[0][0].visited = 1;

    stack[stackSize].row = 0;
    stack[stackSize].col = 0;
    stackSize++;

    while (stackSize > 0)
    {
        // The current cell is the top of the stack
        Position current = stack[stackSize - 1];

        // Start with all four directions
        int directions[4] = { 0, 1, 2, 3 };

        // Randomize the direction order
        shuffle_directions(directions);

        // Used to check if an unvisited neighbor was found
        int found = 0;

        // Look for an unvisited neighboring cell
        for (int i = 0; i < 4; i++)
        {
            int direction = directions[i];

            int nextRow = current.row + dr[direction];
            int nextCol = current.col + dc[direction];

            if (valid_cell(nextRow, nextCol, cells))
            {
                // Mark the new cell as visited
                cells[nextRow][nextCol].visited = 1;

                // Convert logical coordinates to matrix coordinates
                int r1 = current.row * 2 + 1;
                int c1 = current.col * 2 + 1;

                int r2 = nextRow * 2 + 1;
                int c2 = nextCol * 2 + 1;

                // Open the current cell
                maze[r1][c1] = 0;

                // Open the next cell
                maze[r2][c2] = 0;

                // Open the wall between the two cells
                int wallRow = (r1 + r2) / 2;
                int wallCol = (c1 + c2) / 2;

                maze[wallRow][wallCol] = 0;

                // Push the new cell onto the DFS stack
                stack[stackSize].row = nextRow;
                stack[stackSize].col = nextCol;
                stackSize++;

                found = 1;

                break;
            }
        }

        // If no neighbor was found, backtrack
        if (!found)
        {
            stackSize--;
        }
    }

    save_maze(maze);
}


// Print the generated maze to the terminal
void print_maze(int maze[MAZE_HEIGHT][MAZE_WIDTH])
{
    printf("\nGenerated maze:\n\n");

    for (int r = 0; r < MAZE_HEIGHT; r++)
    {
        for (int c = 0; c < MAZE_WIDTH; c++)
        {
            printf("%d", maze[r][c]);
        }

        printf("\n");
    }
}


// Save the generated maze to a file
void save_maze(int maze[MAZE_HEIGHT][MAZE_WIDTH])
{
    char fileName[32];
    snprintf(fileName, sizeof(fileName), "levels/level_%d_map.dat", currentLevel);

    FILE* file = fopen(fileName, "w");

    if (file == NULL)
    {
        printf("ERROR: Could not save maze!\n");
        return;
    }

    // Write every matrix value to the file
    for (int r = 0; r < MAZE_HEIGHT; r++)
    {
        for (int c = 0; c < MAZE_WIDTH; c++)
        {
            fprintf(file, "%d,", maze[r][c]);
        }
    }

    fclose(file);

    printf("\nMaze saved successfully to: %s\n", fileName);
}
//load maze to mapW[]
void load_maze(){
    char fileName[32];
    snprintf(fileName, sizeof(fileName), "levels/level_%d_map.dat", currentLevel);

    FILE* file = fopen(fileName, "r");

    if (file == NULL)
    {
        printf("ERROR: Could not load maze!\n");
        return;
    }

    // Write every matrix value to the file
    for (int r = 0; r < mapY; r++){
        for (int c = 0; c < mapX; c++){
             fscanf(file, "%d,", &mapW[r * mapX + c]);
        }
    }

    fclose(file);

    printf("\nMaze loaded successfully to: %s\n", fileName);
}


