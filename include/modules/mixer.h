#include <SDL3/SDL_stdinc.h>
#include <stdatomic.h>

#include "channel.h"

#ifndef MIXER_H
#define MIXER_H
typedef struct {
    Channel channels[16];
    int     active_channel_count;

    double  tempo;
    _Atomic double  amplitude;
    _Atomic double  level;

    int     sample_index;
    int     sample_length;
} Mixer;
void init_mixer(Mixer *mixer);
void synthesize(Mixer *mixer, Sint16 *data, int sample_count);
void free_mixer(Mixer *mixer);
#endif // MIXER_H
