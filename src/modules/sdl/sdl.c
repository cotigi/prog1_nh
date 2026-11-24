#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_video.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>

#include "../../../include/modules/sdl/sdl.h"
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
        0
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

    gui->is_running = true;

    return true;
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
    SDL_SetRenderDrawColor(gui->renderer, 243, 139, 168, 255);
    SDL_RenderClear(gui->renderer); 
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
        event_handling(gui);
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

    SDL_Quit();

    printf("SUCCESS: SDL3 successfully freed\n");
}
