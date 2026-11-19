#ifndef WEAPON_H
#define WEAPON_H

#define MAX_BULLETS 30
#define BULLET_SIZE 3
#define BULLET_RENDER_FACTOR 0.1f

typedef struct
{
    float bx;
    float by;
    float ba;

    float bdx;
    float bdy;

    int is_active;
    int speed;

}Bullet;

extern int bullet_count;
extern Bullet bullet[MAX_BULLETS];

int create_bullet();
void bullet_update(int blt_indx);
void draw_bullet();
int check_bullet_collision(int blt_indx, int enm_indx);

#endif