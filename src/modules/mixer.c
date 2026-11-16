#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>

#include "../../include/modules/mixer.h"
#include "../../include/modules/oscillator.h"
#include "../../include/modules/channel.h"
#include "../../include/modules/node.h"
#include "../../include/debugmalloc.h"

//  Description:
//      Initialize the mixer data structue;
//  Arguments:
//      - Mixer *mixer: Pointer to the mixer;
//  Return -> void:
//      No return value;
void init_mixer(Mixer *mixer) {
    for (int i = 0; i<16; i++) {
        mixer->channels[i].notes = NULL; 
        mixer->channels[i].amplitude = 0.7;
        read_waveform(&mixer->channels[i].osc, "sine");
    }

    mixer->sample_index = 0;
    mixer->active_channel_count = 0;
}

//  Description:
//      Synthesize a given number of sample;
//  Arguments:
//      - Mixer *mixer: Pointer to the mixer;
//      - Sint16 *data: Pointer to the array of amplitudes with 
//          signed 16 bit values;
//      - int sample_count: The number of samples that need to be calculated;
//  Return -> void:
//      No return value;
void synthesize(Mixer *mixer, Sint16 *data, int sample_count) {
    for (int i = 0; i<sample_count; i++) {
        Sint16 amplitude = 0;
        // for (int i = 0; i<mixer->active_channel_count; i++) {
        //
        // This shouldn't go through the empty channels, this is 
        // just a quick fix.
        for (int i = 0; i<16; i++) {
            if (mixer->channels[i].notes != NULL) {
                amplitude += synthesize_channel(&mixer->channels[i], mixer->sample_index+i);
            }
        }
        //printf("\tdata -> %d -> %d\n", amplitude, amplitude/mixer->active_channel_count);

        // Normalize the amplitude by dividing it by the number of active channels.
        // The multiply of 0.1 is not needed but due to some errors either
        // while calculating the amplitudes or when passing them to the
        // SDL3 mixer, the sound is very loud.
        // By the way, this doesn't work. weird!!!
        data[i] = 0.1*amplitude/mixer->active_channel_count;
    }

    mixer->sample_index += sample_count;
}

//  Description:
//      Free the given mixer structue;
//  Arguments:
//      - Mixer *mixer: Pointer to the mixer;  
//  Return -> void:
//      No return value;
void free_mixer(Mixer *mixer) {
    // The mixer is stored on the stack so only some
    // channel fields need to be freed.
    for (int i = 0; i<16; i++) {
        free_list(mixer->channels[i].notes);
    }
}
