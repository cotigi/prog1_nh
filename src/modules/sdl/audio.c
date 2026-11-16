#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_error.h>
#include <stdio.h>

#include "../../../include/modules/sdl/audio.h"
#include "../../../include/modules/mixer.h"
#include "../../../include/debugmalloc.h"

//  Description:
//      Initializes the audio stream;
//  Arguments:
//      - GUI *gui: Pointer to the gui data structure;
//  Return -> bool:
//      Returns true for success and false for error;
bool init_audio(GUI *gui) {
    gui->spec.channels = 1; // Mono audio
    gui->spec.freq = 44100; // Samplerate
    gui->spec.format = SDL_AUDIO_S16LE; //  Format of the samples: 
                                        //      - signed 16 bit integer

    gui->stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, // The audio device will be decided by SDL3.
        &gui->spec,
        synth_callback, // Reference of the callback function.
        gui             // Pass any user related data. In this case the gui structure.
    );

    if (!gui->stream) {
        fprintf(stderr, "ERROR: SDL_OpenAudioDeviceStream failed:\n\t%s\n", SDL_GetError()); 
        return false;
    }

    return true;
}

//  Description:
//      Frees the audio stream;
//  Arguments:
//      - GUI *gui: Pointer to the gui data structure;
//  Return -> void:
//      No return value;
void free_audio(GUI *gui) {
    SDL_DestroyAudioStream(gui->stream);    // Also closes the binded audio device
}

//  Description:
//      This function will be called by the SDL3 mixer automatically
//      and will be supplied with the relevant arguments by it.
//      The main job of this function is to allocate enough memory
//      for the given number of samples requested and call the synthesize 
//      function on the mixer;
//  Arguments:
//      - void *gui_p: The gui was passed as the userdata 
//          which will be converted to the correct type and used 
//          to synthesize the mixer;
//      - SDL_AudioStream *stream: Pointer to the stream of which 
//          the function will pass the relevant samples;
//      - int sample_amount: The amount of new samples to synthesize;
//      - int total_amount: The amount of total samples synthesized;
//  Return -> void:
//      No return value;
void SDLCALL synth_callback(
    void *gui_p,
    SDL_AudioStream *stream,
    int sample_amount,
    int total_amount
) {  
    // Cast the userdata pointer to the correct type
    GUI *gui = (GUI *)gui_p;

    // Check if the sample index exceeds the mixer's sample length.
    if (gui->mixer->sample_index+sample_amount > gui->mixer->sample_length) {
        SDL_PauseAudioStreamDevice(gui->stream);
        gui->mixer->sample_index = 0;
        return;
    }

    if (sample_amount > 0) {  
        Sint16 *data = SDL_stack_alloc(Sint16, sample_amount/sizeof(Sint16)); 

        if (data) {  
            synthesize(gui->mixer, data, sample_amount/sizeof(Sint16));
            SDL_PutAudioStreamData(stream, data, sample_amount);  
            SDL_stack_free(data);  
        }  
    } 

    // Temporary fix. Ignore it.
    sample_amount = total_amount;
}  
