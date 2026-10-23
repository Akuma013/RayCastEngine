#ifndef AUDIO_H
#define AUDIO_H

#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>

//audio glb vars
extern ALCdevice *audioDevice;
extern ALCcontext *audioContext;
//door audio
extern ALuint openDoorBuffer;
extern ALuint openDoorSource;
//pickup audio
extern ALuint pickupBuffer;
extern ALuint pickupSource;
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

//enemy sound
extern ALuint enemySoundBuffer[32];
extern ALuint enemySoundSource[32];

extern float footstepTimer;

int enemyCanSeePlayer(int x);
void updateEnemySound(int x);
void updateFootsteps(float delta_time);
void updateListener();
void updatePickupSound();
void updateMusic();
int loadWav(const char *filename, ALuint *buffer);
void loadEnemySounds();
void audioInit();

#endif