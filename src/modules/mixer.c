#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>
#include <stdatomic.h>

#include "../../include/modules/mixer.h"
#include "../../include/modules/oscillator.h"
#include "../../include/modules/channel.h"
#include "../../include/modules/node.h"
#include "../../include/modules/helper.h"
#include "../../include/debugmalloc.h"

//  Description:
//      Initialize the mixer data structue;
//  Arguments:
//      - Mixer *mixer: Pointer to the mixer;
//  Return -> void:
//      No return value;
void init_mixer(Mixer *mixer) {
    for (int i = 0; i<16; i++) {
        printf("Channel: %d\n", i);
        mixer->channels[i].notes = NULL; 
        mixer->channels[i].current_node = NULL; 
        mixer->channels[i].active_notes = NULL; 
        atomic_init(&mixer->channels[i].amplitude, 0.7);
        atomic_init(&mixer->channels[i].limit, 1.0);
        //mixer->channels[i].amplitude = 0.7;
        atomic_init(&mixer->channels[i].primary_osc.amplitude, 0.7);
        atomic_init(&mixer->channels[i].primary_osc.limit, 1.0);
        atomic_init(&mixer->channels[i].secondary_osc.amplitude, 0.0);
        atomic_init(&mixer->channels[i].secondary_osc.limit, 1.0);
        //mixer->channels[i].osc.amplitude = 0.7;
        //read_waveform(&mixer->channels[i].osc, "square");
    }

    mixer->sample_index = 0;
    mixer->sample_length = 0;
    mixer->active_channel_count = 0;
    mixer->tempo = 0;
    //mixer->amplitude = 0.7;
    atomic_init(&mixer->amplitude, 0.7);
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
    //const double step = 2.0 * SDL_PI_D * 110.0 / 44100.0;

    for (int i = 0; i<sample_count; i++) {
        double amplitude = 0.0;
        // for (int i = 0; i<mixer->active_channel_count; i++) {
        //
        // This shouldn't go through the empty channels, this is 
        // just a quick fix.
        for (int j = 0; j<16; j++) {
            if (mixer->channels[j].notes != NULL) {
                //double phase = ((double)(mixer->sample_index+i)*1720)/44100;
                //amplitude += (Sint16)(MAX_VOLUME*SDL_sin(step*(mixer->sample_index+i)));
                amplitude += synthesize_channel(&mixer->channels[j], mixer->sample_index+i);
            }
        }
        //printf("\tdata -> %d -> %d\n", amplitude, amplitude/mixer->active_channel_count);
        amplitude /= mixer->active_channel_count;
        amplitude = limit(amplitude, 1.0);
        /*
        amplitude = (amplitude >= 1.0) ?
            1.0 : (amplitude <= -1.0) ?
                -1.0 :
                amplitude
            ;
        */
        amplitude *= atomic_load(&mixer->amplitude);

        atomic_store(&mixer->level, (amplitude<0) ? -amplitude : amplitude);

        data[i] = (Sint16)(MAX_VOLUME*(amplitude));
        //data[i] = (Sint16)(MAX_VOLUME*SDL_sin(step*(mixer->sample_index+i)));
        //double amp = synthesize_channel(&mixer->channels[0], mixer->sample_index+i);
        //double amp = synthesize_channel(&mixer->channels[1], mixer->sample_index+i);
        //data[i] = (Sint16)(0.6*MAX_VOLUME*amp);
        //data[i] = (Sint16)(0.6*MAX_VOLUME*synthesize_channel(&mixer->channels[0], mixer->sample_index+i));
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
        //printf("\n\tChannel %d active: %p", i, mixer->channels[i].active_notes);
        free_active_list(mixer->channels[i].active_notes);
    }
}
