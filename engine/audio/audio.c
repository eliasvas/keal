#include "engine.h"

/*
    // for a basic sound (with memleak :D)
    nSound *s = push_array(get_global_arena(), nSound, 1);
    nsound_load(get_nactx(), s, "assets/sound.wav");
    nsound_play(get_nactx(), s);

    // and for basic music
    nMusic *m = push_array(get_global_arena(), nMusic, 1);
    nmusic_load(get_nactx(), m, "assets/sound.mp3");
    nmusic_play(get_nactx(), m, NMUSIC_PLAY_FOREVER);
*/

// Im too Bored to do this correctly, with function pointers and stuff, I SHOULD do it though, someday!
void naudio_impl_context_init(nAudioContext *actx);
void naudio_impl_context_deinit(nAudioContext *actx);
void nsound_impl_load_from_pcm_data(nAudioContext *actx, nSound *s, nSoundPcmData *data);
void nsound_impl_load(nAudioContext *actx, nSound *s, const char *filepath);
void nsound_impl_deinit(nAudioContext *actx, nSound *s);
void nsound_impl_play(nAudioContext *actx, nSound *s);
void nmusic_impl_load(nAudioContext *actx, nMusic *m, const char *filepath);
void nmusic_impl_deinit(nAudioContext *actx, nMusic *m);
void nmusic_impl_play(nAudioContext *actx, nMusic *m, u64 times);



void naudio_context_init(nAudioContext *actx) {
    naudio_impl_context_init(actx);
    actx->master_volume = 20;
}
void naudio_context_deinit(nAudioContext *actx) {
    naudio_impl_context_deinit(actx);
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

void nsound_load_from_pcm_data(nAudioContext *actx, nSound *s, nSoundPcmData *data) {
    nsound_impl_load_from_pcm_data(actx, s,data);
    s->volume = actx->master_volume;
}
void nsound_load(nAudioContext *actx, nSound *s, const char *filepath) {
    nsound_impl_load(actx,s,filepath);
    s->volume = actx->master_volume;
}
void nsound_deinit(nAudioContext *actx, nSound *s) {
    nsound_impl_deinit(actx, s);
}
void nsound_play(nAudioContext *actx, nSound *s) {
    nsound_impl_play(actx,s);
}

void nmusic_load(nAudioContext *actx, nMusic *m, const char *filepath) {
    nmusic_impl_load(actx, m, filepath);
}
void nmusic_deinit(nAudioContext *actx, nMusic *m) {
    nmusic_impl_deinit(actx, m);
}
void nmusic_play(nAudioContext *actx, nMusic *m, u64 times) {
    nmusic_impl_play(actx, m, times);
}


