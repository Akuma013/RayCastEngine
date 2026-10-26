#include <stdlib.h>

#include "map.h"

Map map = {0};

int map_init(Map* map, int width, int height){
    map->width = width;
    map->height = height;

    size_t size = (size_t) width * height;

    map->walls = calloc(size ,sizeof( *map->walls));
    map->floors = calloc(size ,sizeof( *map->floors));
    map->ceilings = calloc(size ,sizeof( *map->ceilings));

    if (map->walls == NULL ||
        map->floors == NULL ||
        map->ceilings == NULL) {
        destroy_map(map);
        return 0;
    }

    return 1;


}

void destroy_map(Map* map){
    if(map == NULL){
        return;
    }

    free(map->walls);
    free(map->floors);
    free(map->ceilings);

    map->walls = NULL;
    map->floors = NULL;
    map->ceilings = NULL;
    map->width = 0;
    map->height = 0;
}