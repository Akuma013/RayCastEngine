#ifndef AUDIO_H
#define AUDIO_H

#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>

#define ENEMY_SOURCE_MAX 32
#define WEAPON_SOURCE_MAX 4
#define PICKUP_SOURCE_MAX 4
#define DOOR_SOURCE_MAX 4

//audio glb vars
extern ALCdevice *audioDevice;
extern ALCcontext *audioContext;
//door audio
extern ALuint openDoorBuffer;
extern ALuint openDoorSource[DOOR_SOURCE_MAX];
//pickup audio
extern ALuint pickupBuffer[PICKUP_SOURCE_MAX];
extern ALuint pickupSource[PICKUP_SOURCE_MAX];
//bg music
extern ALuint bgMusic_1_Buffer;
extern ALuint bgMusic_1_Source;
// steps sound
extern ALuint footstepBuffer;
extern ALuint footstepSource;

// die sound
extern ALuint diedBuffer;
extern ALuint diedSource;


// win sound
extern ALuint winBuffer;
extern ALuint winSource;


// enemy killed sound
extern ALuint enemyDeathBuffer[ENEMY_SOURCE_MAX];
extern ALuint enemyDeathSource[ENEMY_SOURCE_MAX];
//enemy sound
extern ALuint enemySoundBuffer[ENEMY_SOURCE_MAX];
extern ALuint enemySoundSource[ENEMY_SOURCE_MAX];

//weapon sound
extern ALuint weaponSoundBuffer[WEAPON_SOURCE_MAX];
extern ALuint weaponSoundSource[WEAPON_SOURCE_MAX];


extern float footstepTimer;

int enemyCanSeePlayer(int x);
void enemyDeadSound(int x);
void updateEnemySound(int x);
void updateFootsteps(float delta_time);
void updateListener();
void updatePickupSound(int key_id);
void updateDoorSound(float doorX, float doorY);
void updateMusic();
int loadWav(const char *filename, ALuint *buffer);
void loadEnemySounds();
void loadPickupSounds();
void loadDoorSounds();
void loadWeaponSounds();
void load_audio();
void audioInit();

#endif