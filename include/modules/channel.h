#include <SDL3/SDL_stdinc.h>

#include "node.h"
#include "oscillator.h"

#ifndef CHANNEL_H
#define CHANNEL_H
typedef struct {
    Node        *notes;
    Node        *current_node;
    Node        *active_notes;
    Oscillator  osc;
    double      amplitude;
    //double      output[];
    // Velocity function pointer
} Channel;
double synthesize_channel(Channel *channel, int sample_index);
#endif // CHANNEL_H
