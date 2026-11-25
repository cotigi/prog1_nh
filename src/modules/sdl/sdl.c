#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_video.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>

#include "../../../include/modules/sdl/sdl.h"
#include "../../../include/modules/ui.h"
#include "../../../include/debugmalloc.h"

//  Description:
//      Initializes the GUI data structure and start the 
//      relevant SDL3 components like the window and renderer;
//  Arguments:
//      - GUI *gui: Pointer to the gui data structure;
//  Return -> bool:
//      Return a true for success and false for error;
bool init_gui(GUI *gui) {
    if (!SDL_Init(SDL_FLAGS)) {
        fprintf(stderr, "ERROR: SDL_Init failed:\n\t%s\n", SDL_GetError());
        return false;
    }

    gui->window = SDL_CreateWindow(
        WINDOW_TITLE,
        WINDOW_WIDTH, 
        WINDOW_HEIGHT,
        SDL_WINDOW_FULLSCREEN
    );

    if (gui->window == NULL) {
        fprintf(stderr, "ERROR: SDL_CreateWindow failed:\n\t%s\n", SDL_GetError());
        return false;
    }

    gui->renderer = SDL_CreateRenderer(gui->window, NULL);

    if (gui->renderer == NULL) {
        fprintf(stderr, "ERROR: SDL_CreateRenderer failed:\n\t%s\n", SDL_GetError());
        return false;
    }

    return true;
}

//  Description:
//      Initializes the gui and does other miscellaneous
//      assignments;
//  Arguments:
//      - GUI *gui: Pointer to the gui data structure;
//  Return -> bool:
//      Return true for success and false for error;
bool new_gui(GUI *gui) {
    if (!init_gui(gui)) {
        return false;
    }

    gui->mu = malloc(sizeof(mu_Context));

    if (gui->mu == NULL) {
        return false;
    }

    init_ui(gui);
    atomic_init(&gui->is_running, true);
    atomic_init(&gui->is_audio_paused, true);
    gui->waveform_count = 0;

    read_waveforms_dir(gui);

    if (gui->waveform_count > 0) {
        for (int i = 0; i < 16; i++) {
            //gui->mixer->channels[i].osc.waveform = gui->waveforms[0];
            atomic_init(&gui->mixer->channels[i].primary_osc.selected_waveform, 1);
            atomic_init(
                &gui->mixer->channels[i].primary_osc.waveform,
                gui->waveforms[1]
            );

            atomic_init(&gui->mixer->channels[i].secondary_osc.selected_waveform, 1);
            atomic_init(
                &gui->mixer->channels[i].secondary_osc.waveform,
                gui->waveforms[0]
            );
        }
    }

    return true;
}

//  Description:
//      Reads the waveform data from a specified file as Sint16 values;
//  Arguments:
//      - Oscillator *osc: Pointer to the given oscillator
//          to assign the waveform table values;
//      - char *name: Name of the waveform file.
//          This only contains the name ("sine") and 
//          doesn't contain the path ("saves/waveforms/") or the 
//          extension (".txt");
//  Return -> void:
//      No return value;
void read_waveforms_dir(GUI *gui) {
    struct dirent *de;

    DIR *dr = opendir("./saves/waveforms");

    if (dr == NULL) {
        fprintf(stderr, "ERROR: Couldn't open `saves/waveforms` directory.\n");
    }

    while ((de = readdir(dr)) != NULL) {
        if (de->d_type == DT_REG) {
            if (strstr(de->d_name, ".txt")) { gui->waveform_count++; }
        }
    }

    gui->waveforms = malloc(sizeof(Waveform*)*gui->waveform_count);

    rewinddir(dr);
    int index = 0;

    while ((de = readdir(dr)) != NULL) {
        if (de->d_type == DT_REG) {
            if (strstr(de->d_name, ".txt")) { 
                read_waveform_file(gui, de->d_name, index);
                index++;
            }
        }
    }

    closedir(dr);
}

void read_waveform_file(GUI *gui, char *name, int waveform_index) {
    char filepath[64] = "saves/waveforms/";
    strcat(filepath, name);

    FILE* file = fopen(filepath, "r");

    if (file == NULL) {
        fprintf(stdout, "ERROR: Couldn't read waveform file:\n\t%s", name); 
        return;
    }

    gui->waveforms[waveform_index] = malloc(sizeof(Waveform));
    strcpy(gui->waveforms[waveform_index]->name, name);

    int index = 0;
    char buff[70];

    // Every waveform is saved as a signed 16 bit value is the file. 
    // The loop goes until the table is full or the file has ended.
    while (index < WAVETABLE_SAMPLERATE && fgets(buff, sizeof(buff), file)) {
        gui->waveforms[waveform_index]->waveform[index] = (double)atof(buff);
        //osc->waveform[index] = (double)atof(buff);
        index++;
    }

    fclose(file);
}

//  Description:
//      Checks the current event and does what is needed;
//  Arguments:
//      - GUI *gui: Pointer to the gui data structure;
//  Return -> void:
//      No return value;
void event_handling(GUI *gui) {
    // Loop through every available event.
    while (SDL_PollEvent(&gui->event)) {
        switch (gui->event.type) {
        case SDL_EVENT_QUIT: 
            gui->is_running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (gui->event.key.scancode == SDL_SCANCODE_SPACE) {
                SDL_ResumeAudioStreamDevice(gui->stream); // Start audio playback
                printf("Audio playing\n");
            } else if (gui->event.key.scancode == SDL_SCANCODE_Q) {
                gui->is_running = false;
            } 
            // TODO: Should make this a toggle option.
            break;
        default:
            break;
        }
    }
}

//  Description:
//      Basic ass rendering for now. Nothing much;
//  Arguments:
//      - GUI *gui: Pointer to the gui data structure;
//  Return -> void:
//      No return value;
void render(GUI *gui) {
    SDL_SetRenderDrawColor(gui->renderer, 30, 30, 46, 255);
    SDL_RenderClear(gui->renderer); 
    draw_ui(gui);
    SDL_RenderPresent(gui->renderer);
}

//  Description:
//      Start the game loop;
//  Arguments:
//      - GUI *gui: Pointer to the gui data structure;
//  Return -> void:
//      No return value;
void run_gui(GUI *gui) {
    while (gui->is_running) {
        //event_handling(gui);
        ui_handle_event(gui);
        update_ui(gui);
        render(gui);

        // This delay makes the rendering about 60fps.
        SDL_Delay(16);
    }
}

//  Description:
//      Frees the gui data structure;
//  Arguments:
//      - GUI *gui: Pointer to the gui data structure;
//  Return -> void:
//      No return value;
void free_gui(GUI *gui) {
    if (gui->renderer != NULL) {
        SDL_DestroyRenderer(gui->renderer);
        gui->renderer = NULL;
    }

    if (gui->window != NULL) {
        SDL_DestroyWindow(gui->window);
        gui->window = NULL;
    }

    if (gui->mu != NULL) {
        free(gui->mu);
        gui->mu = NULL;
    }

    for (int i = 0; i < gui->waveform_count; i++) {
        free(gui->waveforms[i]);
    }
    free(gui->waveforms);
    gui->waveforms = NULL;

    SDL_Quit();

    printf("SUCCESS: SDL3 successfully freed\n");
}
