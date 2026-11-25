#include <SDL3/SDL_stdinc.h>
#include <stdatomic.h>

#include "./node.h"
#include "./oscillator.h"

#ifndef CHANNEL_H
#define CHANNEL_H
typedef struct {
    Node        *notes;
    Node        *current_node;
    Node        *active_notes;
    Oscillator  primary_osc;
    Oscillator  secondary_osc;
    _Atomic double      amplitude;
    _Atomic double      limit;
    //double      output[];
    // Velocity function pointer
} Channel;
double synthesize_channel(Channel *channel, int sample_index);
#endif // CHANNEL_H
