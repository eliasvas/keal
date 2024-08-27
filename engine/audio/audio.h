#ifndef NAUDIO_H
#define NAUDIO_H

#include "engine.h"

#define NAUDIO_MAX_VOLUME 125
#define NAUDIO_MIN_VOLUME 0 
typedef struct nAudioContext nAudioContext;
struct nAudioContext {
    u32 master_volume;
    void *impl_state;
};
void naudio_context_init(nAudioContext *actx);
void naudio_context_deinit(nAudioContext *actx);

typedef struct nSoundPcmData nSoundPcmData;
struct nSoundPcmData{
    u8 *samples;
    u32 sample_count;
};
nSoundPcmData nsound_gen_sample_pcm_data();
void nsound_pcm_data_deinit(nSoundPcmData *pcm_data);

#define NSOUND_MAX_NAME_SIZE 64
typedef struct nSound nSound;
struct nSound {
    u32 volume;
    char name[NSOUND_MAX_NAME_SIZE];
    void *impl_state;
};

void nsound_load_from_pcm_data(nAudioContext *actx, nSound *s, nSoundPcmData *data);
void nsound_load(nAudioContext *actx, nSound *s, const char *filepath);
void nsound_deinit(nAudioContext *actx, nSound *s);
// TODO -- This should generate a 'nSoundInstance' struct that we can control its properties, like speed/volume
void nsound_play(nAudioContext *actx, nSound *s);

#endif