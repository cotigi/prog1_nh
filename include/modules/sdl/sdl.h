#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_stdinc.h>
#include <stdbool.h>
#include <stdatomic.h>

#include "../mixer.h"
#include "../../microui/microui.h"

#ifndef SDL_H
#define SDL_H

#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_AUDIO)
#define WINDOW_TITLE "Synth"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

typedef struct {
    SDL_Window          *window;
    SDL_Renderer        *renderer;
    SDL_Event           event;
    mu_Context          *mu;


    SDL_AudioStream     *stream;
    SDL_AudioSpec       spec;

    Mixer               *mixer;

    _Atomic bool        is_running;
    _Atomic bool        is_audio_paused;

    Waveform            **waveforms;
    int                 waveform_count;
} GUI;

bool init_gui(GUI *gui);
bool new_gui(GUI *gui);
void read_waveforms_dir(GUI *gui);
void read_waveform_file(GUI *gui, char *name, int waveform_index);
void event_handling(GUI *gui);
void render(GUI *gui);
void run_gui(GUI *gui);
void free_gui(GUI *gui);
#endif // SDL_H
