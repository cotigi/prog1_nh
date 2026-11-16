#include <SDL3_mixer/SDL_mixer.h>
#include "../../../include/modules/sdl/sdl.h"
#include <stdbool.h>

#ifndef AUDIO_H
#define AUDIO_H
bool init_audio(GUI *gui);
void free_audio(GUI *gui);
void SDLCALL synth_callback(void *userdata, SDL_AudioStream *stream,  
                            int additional_amount, int total_amount);
#endif // AUDIO_H
