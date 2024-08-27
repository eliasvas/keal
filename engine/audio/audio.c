#include "engine.h"

void naudio_context_init(nAudioContext *actx) {
    // Initialize SDL AUDIO
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return;
    }
    actx->master_volume = 20;
}


void naudio_context_deinit(nAudioContext *actx) {
    // TBA
}


#define SAMPLE_RATE 44100
#define FREQUENCY 440
#define AMPLITUDE 127
#define DURATION 2 // in seconds

nSoundPcmData nsound_gen_sample_pcm_data() {
    nSoundPcmData data = {0};
    int num_samples = SAMPLE_RATE * DURATION;
    int wave_period = SAMPLE_RATE / FREQUENCY;
    data.sample_count = num_samples;
    u8* buffer = (u8*)ALLOC(num_samples * sizeof(u8));

    // Fill the buffer with a square wave
    for (int i = 0; i < num_samples; ++i) {
        if ((i / wave_period) % 2 == 0) {
            buffer[i] = AMPLITUDE;
        } else {
            buffer[i] = 0;
        }
    }
    data.samples = buffer;

    return data;
}

void nsound_pcm_data_deinit(nSoundPcmData *pcm_data) {
    if (pcm_data->sample_count) {
        FREE(pcm_data->samples);
        M_ZERO_STRUCT(pcm_data);
    }
}

// Im too Bored to do this correctly, with function pointers and stuff, I SHOULD do it though, someday!
void nsound_impl_load_from_pcm_data(nAudioContext *actx, nSound *s, nSoundPcmData *data);
void nsound_impl_load(nAudioContext *actx, nSound *s, const char *filepath);
void nsound_impl_deinit(nAudioContext *actx, nSound *s);
void nsound_impl_play(nAudioContext *actx, nSound *s);

void nsound_load_from_pcm_data(nAudioContext *actx, nSound *s, nSoundPcmData *data) {
    nsound_impl_load_from_pcm_data(actx, s,data);
}
void nsound_load(nAudioContext *actx, nSound *s, const char *filepath) {
    nsound_impl_load(actx,s,filepath);
}
void nsound_deinit(nAudioContext *actx, nSound *s) {
    nsound_impl_deinit(actx, s);
}
void nsound_play(nAudioContext *actx, nSound *s) {
    nsound_impl_play(actx,s);
}



