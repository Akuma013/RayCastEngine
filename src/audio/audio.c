#include "audio.h"
#include "../game/player.h"
#include "../game/game.h"
#include "../game/map.h"


#include <math.h>
#include <string.h>
#include <stdlib.h>

ALCdevice *audioDevice = NULL;
ALCcontext *audioContext = NULL;

//door audio
ALuint openDoorBuffer;
ALuint openDoorSource;
//pickup audio
ALuint pickupBuffer;
ALuint pickupSource;
//bg music
ALuint bgMusic_1_Buffer;
ALuint bgMusic_1_Source;
// steps sound
ALuint footstepBuffer;
ALuint footstepSource;

// die sound
ALuint diedBuffer;
ALuint diedSource;


// win sound
ALuint winBuffer;
ALuint winSource;

//enemy sound
ALuint enemySoundBuffer[32];
ALuint enemySoundSource[32];

float footstepTimer = 0.0f;

static float randomFloat(float min, float max){
    return min + ((float)rand() / RAND_MAX) * (max - min);
}



int enemyCanSeePlayer(int x)
{
    float enemyX = sp[x].x;
    float enemyY = sp[x].y;

    float dx = px - enemyX;
    float dy = py - enemyY;

    float distance = sqrt(dx * dx + dy * dy);

    // How far the enemy can see
    if (distance > 400.0f)
        return 0;

    // Number of points we check between enemy and player
    int steps = (int)(distance / 4.0f);

    for (int i = 1; i < steps; i++)
    {
        float checkX = enemyX + dx * ((float)i / steps);
        float checkY = enemyY + dy * ((float)i / steps);

        int mapXPos = (int)checkX / 64;
        int mapYPos = (int)checkY / 64;

        // Wall blocks the enemy's view
        if (map.walls[mapYPos * map.width + mapXPos] != 0)
        {
            return 0;
        }
    }

    return 1;
}


//AUDIO
void updateEnemySound(int x)
{
    
    alSource3f(
        enemySoundSource[x],
        AL_POSITION,
        sp[x].x / 64.0f,
        sp[x].y / 64.0f,
        0.0f
    );
    float dx = fabs(px - sp[x].x);
    float dy = fabs(py - sp[x].y);
    float dist_sqr = dx * dx + dy * dy;
    if (gameState == 2  && dist_sqr <= (5*mapS) * (5*mapS) ){
        ALint state;
        alGetSourcei(enemySoundSource[x], AL_SOURCE_STATE, &state);

        if (state != AL_PLAYING){
            alSourcePlay(enemySoundSource[x]);
        }
    }else{
        alSourcePause(enemySoundSource[x]);
    }      

}


void updateFootsteps(float delta_time)
{
    int walking = (Keys.w || Keys.s || Keys.a || Keys.d);
    if (!walking) {
        footstepTimer = 0.0f;
        return;
    }

    footstepTimer -= delta_time;

    if(footstepTimer <= 0){
        alSourcef(footstepSource, AL_PITCH, randomFloat(0.92f, 1.08f));

        alSourcef(footstepSource, AL_GAIN, randomFloat(0.65f, 0.85f));

        alSourceRewind(footstepSource);
        alSourcePlay(footstepSource);

        footstepTimer = randomFloat(0.32f, 0.44f);

    }
}



void updateListener()
{
    // Player position
    alListener3f(
        AL_POSITION,
        px / 64.0f,
        py / 64.0f,
        0.0f
    );

    // Convert player's angle to radians
    float angle = degToRad(pa);

    // OpenAL listener orientation
    float orientation[] = {
        -cos(angle), sin(angle), 0.0f,   // forward
        0.0f,        0.0f,       1.0f    // up
    };

    alListenerfv(AL_ORIENTATION, orientation);
}



void updatePickupSound()
{
    alSource3f(
        pickupSource,   
        AL_POSITION,
        sp[0].x / 64.0f,
        sp[0].y / 64.0f,
        0.0f
    );

}





void updateMusic()
{
    if (gameState == 2){
        ALint state;
        alGetSourcei(bgMusic_1_Source, AL_SOURCE_STATE, &state);

        if (state != AL_PLAYING){
            alSourcePlay(bgMusic_1_Source);
        }
    }
    else{
        alSourceStop(bgMusic_1_Source);
    }
}


int loadWav(const char *filename, ALuint *buffer)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        printf("Could not open WAV file: %s\n", filename);
        return 0;
    }

    char riff[4];
    char wave[4];
    int chunkSize;

    fread(riff, 1, 4, file);
    fread(&chunkSize, 4, 1, file);
    fread(wave, 1, 4, file);

    if (strncmp(riff, "RIFF", 4) != 0 ||
        strncmp(wave, "WAVE", 4) != 0)
    {
        printf("%s is not a WAV file!\n", filename);
        fclose(file);
        return 0;
    }

    short audioFormat;
    short channels;
    int sampleRate;
    short bitsPerSample;

    char fmt[4];
    int fmtSize;

    fread(fmt, 1, 4, file);
    fread(&fmtSize, 4, 1, file);

    fread(&audioFormat, 2, 1, file);
    fread(&channels, 2, 1, file);
    fread(&sampleRate, 4, 1, file);

    fseek(file, 6, SEEK_CUR);

    fread(&bitsPerSample, 2, 1, file);

    printf("WAV: %s\n", filename);
    printf("Format: %d\n", audioFormat);
    printf("Channels: %d\n", channels);
    printf("Sample rate: %d\n", sampleRate);
    printf("Bits per sample: %d\n", bitsPerSample);


    
     // Find the "data" chunk
    char chunkID[4];
    int dataSize;

    while (1)
    {
        if (fread(chunkID, 1, 4, file) != 4)
        {
            printf("Could not find WAV data chunk!\n");
            fclose(file);
            return 0;
        }

        fread(&dataSize, 4, 1, file);

        if (strncmp(chunkID, "data", 4) == 0)
        {
            break;
        }

        
         // skip to data chunk
        fseek(file, dataSize, SEEK_CUR);
    }

    printf("Audio data size: %d bytes\n", dataSize);


    
     // Allocate memory for the audio samples
    unsigned char *data = malloc(dataSize);

    if (data == NULL)
    {
        printf("Could not allocate memory for audio!\n");
        fclose(file);
        return 0;
    }


    fread(data, 1, dataSize, file);
    fclose(file);
    
    // Create an OpenAL buffer
    alGenBuffers(1, buffer);
    
    // Tell OpenAL what kind of audio we loaded
    ALenum format;

    if (channels == 1 && bitsPerSample == 16)
    {
        format = AL_FORMAT_MONO16;
    }
    else if (channels == 2 && bitsPerSample == 16)
    {
        format = AL_FORMAT_STEREO16;
    }
    else
    {
        printf("Unsupported WAV format!\n");
        free(data);
        return 0;
    }
    // Put the audio data into the OpenAL buffer
    alBufferData(
        *buffer,
        format,
        data,
        dataSize,
        sampleRate
    );

    free(data);
    return 1;

}



void loadEnemySounds()
{
    for (int i = 0; i < numSprite; i++)
    {
        if (sp[i].type != 3)
            continue;
        
        int sound_num = (rand() % 3) + 1;
        char filename[64];
        sprintf(filename, "assets/audio/enm%d.wav", sound_num);

        if (!loadWav(filename,&enemySoundBuffer[i])){
            printf("Failed to load enemy %d sound!\n", i);
            continue;
        }

        printf("Enemy %d sound loaded!\n", i);
        alGenSources(1, &enemySoundSource[i]);
        alSourcei(enemySoundSource[i],AL_BUFFER,enemySoundBuffer[i]);
        alSourcef(enemySoundSource[i], AL_GAIN, 0.5f);
        alSourcei(enemySoundSource[i], AL_LOOPING, AL_TRUE);

        // positional audio
        alSourcei(enemySoundSource[i],AL_SOURCE_RELATIVE,AL_FALSE);
        alSourcef(enemySoundSource[i],AL_REFERENCE_DISTANCE,1.0f);
        alSourcef(enemySoundSource[i],AL_ROLLOFF_FACTOR,3.0f);
        alSourcef(enemySoundSource[i], AL_MAX_DISTANCE, 5.0f);
    }
}

void audioInit()
{

    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);


    audioDevice = alcOpenDevice(NULL);

    if (audioDevice == NULL){
        printf("Failed to open audio device!\n");
        return;
    }

    audioContext = alcCreateContext(audioDevice, NULL);

    if (audioContext == NULL)
    {
        printf("Failed to create OpenAL context!\n");
        alcCloseDevice(audioDevice);
        audioDevice = NULL;
        return;
    }

    alcMakeContextCurrent(audioContext);
    printf("OpenAL audio initialized!\n");

    //load door audio
    if (!loadWav("assets/audio/open_door_2.wav", &openDoorBuffer)){
        printf("Failed to load door sound!\n");
    }
    else{
        printf("Door sound loaded!\n");
        alGenSources(1, &openDoorSource);
        alSourcei(openDoorSource, AL_BUFFER, openDoorBuffer);
        alSourcef(openDoorSource, AL_GAIN, 1.0f);

        alSourcei(openDoorSource, AL_SOURCE_RELATIVE, AL_FALSE);
        alSourcef(openDoorSource, AL_REFERENCE_DISTANCE, 1.0f);
        alSourcef(openDoorSource, AL_ROLLOFF_FACTOR, 1.0f);


    }

    //load pickup audio
    if (!loadWav("assets/audio/pickup.wav", &pickupBuffer)){
        printf("Failed to load pickup sound!\n");
    }
    else{
        printf("Pickup sound loaded!\n");
        alGenSources(1, &pickupSource);
        alSourcei(pickupSource, AL_BUFFER, pickupBuffer);
        alSourcef(pickupSource, AL_GAIN, 1.0f);

    }

    //load bg music
    // if (!loadWav("assets/audio/Maze3DSong.wav", &bgMusic_1_Buffer)){ //
    //     printf("Failed to load background music!\n");
    // }
    // else{
    //     printf("Background music loaded!\n");
    //     alGenSources(1, &bgMusic_1_Source);
    //     alSourcei(bgMusic_1_Source, AL_BUFFER, bgMusic_1_Buffer);
    //     alSourcei(bgMusic_1_Source, AL_SOURCE_RELATIVE, AL_TRUE);
    //     alSourcei(bgMusic_1_Source, AL_LOOPING, AL_TRUE);
    //     alSourcef(bgMusic_1_Source, AL_GAIN, 0.5f);
    // }

     //load walk audio
    if (!loadWav("assets/audio/walk1.wav", &footstepBuffer)){ //
        printf("Failed to load steps sound!\n");
    }
    else{
        printf("steps sound loaded!\n");
        alGenSources(1, &footstepSource);
        alSourcei(footstepSource, AL_BUFFER, footstepBuffer);
        alSourcei(footstepSource, AL_LOOPING, AL_FALSE);

        alSourcei(footstepSource, AL_SOURCE_RELATIVE, AL_TRUE);
        alSourcef(footstepSource, AL_GAIN, 1.0f);
    }

    //load die audio
    if (!loadWav("assets/audio/die.wav", &diedBuffer)){
        printf("Failed to load died sound!\n");
    }
    else{
        printf("died sound loaded!\n");
        alGenSources(1, &diedSource);
        alSourcei(diedSource, AL_BUFFER, diedBuffer);
        alSourcei(diedSource, AL_SOURCE_RELATIVE, AL_TRUE);
        alSourcef(diedSource, AL_GAIN, 1.0f);

    }

    //load win audio
    if (!loadWav("assets/audio/win.wav", &winBuffer)){
        printf("Failed to load win sound!\n");
    }
    else{
        printf("win sound loaded!\n");
        alGenSources(1, &winSource);
        alSourcei(winSource, AL_BUFFER, winBuffer);
        alSourcei(winSource, AL_SOURCE_RELATIVE, AL_TRUE);
        alSourcef(winSource, AL_GAIN, 0.5f);

    }

}
