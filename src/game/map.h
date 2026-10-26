#ifndef MAP_H
#define MAP_H

#define mapS 64 // map cube size

typedef struct 
{
    int width;
    int height;

    int *walls;
    int *floors;
    int *ceilings;
}Map;

extern Map map;

int map_init(Map* map, int width, int height);
void destroy_map(Map* map);

#endif