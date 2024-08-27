#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include "engine.h"

void nsound_impl_load_from_pcm_data(nAudioContext *actx, nSound *s, nSoundPcmData *data) {
    int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int chunk_count = 4096;
    if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, chunk_count) != 0) { fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError()); exit(1); }
    Mix_Chunk *sound = Mix_QuickLoad_RAW(data->samples, data->sample_count);
    s->impl_state = (void*)sound;
    s->volume = actx->master_volume;
}

void nsound_impl_load(nAudioContext *actx, nSound *s, const char *filepath) {
    //TBA
}
void nsound_impl_deinit(nAudioContext *actx, nSound *s) {
    Mix_FreeChunk((Mix_Chunk*)s->impl_state);
    M_ZERO_STRUCT(s);
}
void nsound_impl_play(nAudioContext *actx, nSound *s) {
    int channel = Mix_PlayChannel(-1,(Mix_Chunk*)s->impl_state,0);
    Mix_Volume(channel, s->volume);
}