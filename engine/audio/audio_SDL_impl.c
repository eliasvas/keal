#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include "engine.h"

void naudio_impl_context_init(nAudioContext *actx) {
    // Initialize SDL AUDIO
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return;
    }

    int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int chunk_count = 4096;
    //int audio_rate = 44100; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int chunk_count = 4096;
    if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, chunk_count) != 0) { fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError()); exit(1); }
}

void naudio_impl_context_deinit(nAudioContext *actx) {
    // TBA
}

void nsound_impl_load_from_pcm_data(nAudioContext *actx, nSound *s, nSoundPcmData *data) {
    Mix_Chunk *sound = Mix_QuickLoad_RAW(data->samples, data->sample_count);
    s->impl_state = (void*)sound;
}
// Note -- for sounds, only WAVs are supported
void nsound_impl_load(nAudioContext *actx, nSound *s, const char *filepath) {
    // This is so ugly its Sick (I hate cstdlib)
    assert(strlen(filepath) > 4 && strcmp(&filepath[strlen(filepath)-3],"wav") == 0 && "File is not a WAV!");

    Mix_Chunk *sound = Mix_LoadWAV(filepath);
    s->impl_state = (void*)sound;
}

void nsound_impl_deinit(nAudioContext *actx, nSound *s) {
    Mix_FreeChunk((Mix_Chunk*)s->impl_state);
    M_ZERO_STRUCT(s);
}
void nsound_impl_play(nAudioContext *actx, nSound *s) {
    int channel = Mix_PlayChannel(-1,(Mix_Chunk*)s->impl_state,0);
    Mix_Volume(channel, s->volume);
}


void nmusic_impl_load(nAudioContext *actx, nMusic *m, const char *filepath) {
    Mix_Music *music = Mix_LoadMUS(filepath);
    m->impl_state = (void*)music;
}
void nmusic_impl_deinit(nAudioContext *actx, nMusic *m) {
    Mix_FreeMusic((Mix_Music*)m->impl_state);
    M_ZERO_STRUCT(m);
}
void nmusic_impl_play(nAudioContext *actx, nMusic *m, u64 times) {
    int channel = Mix_PlayMusic((Mix_Music*)m->impl_state, (times != NMUSIC_PLAY_FOREVER) ? times : -1);
    Mix_Volume(channel, m->volume);
}