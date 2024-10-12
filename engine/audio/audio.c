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
    actx->master_volume = 5;
}
void naudio_context_deinit(nAudioContext *actx) {
    naudio_impl_context_deinit(actx);
}

#define SAMPLE_RATE 44100

nSoundPcmData nsound_gen_sample_pcm_data(nWaveformType waveform, float frequency, float amplitude, float duration) {
    nSoundPcmData data = {0};
    u32 num_samples = (u32)(SAMPLE_RATE * duration);
    data.sample_count = num_samples;
    u8* buffer = (u8*)ALLOC(num_samples * sizeof(u8));

    for (u32 i = 0; i < num_samples; i+=1) {
        f32 phase = (2.0f * PI * frequency * i) / SAMPLE_RATE;
        f32 sample_value = 0;
        switch (waveform) {
            case NWAVEFORM_SINE:
                sample_value = sin(phase);
                break;
            case NWAVEFORM_SQUARE:
                sample_value = (sin(phase) >= 0) ? 1.0f : -1.0f;
                break;
            case NWAVEFORM_TRIANGLE:
                sample_value = (2.0f / PI) * asin(sin(phase));
                break;
            case NWAVEFORM_SAWTOOTH:
                sample_value = 2.0f * (phase / (2.0f * PI) - floor(phase / (2.0f * PI) + 0.5f));
                break;
        }
        // [-1, 1] -> [0, AMPLITUDE] for 8-bit audio
        buffer[i] = (u8)((amplitude / 2.0f) * (sample_value + 1.0f));
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


