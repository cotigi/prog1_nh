#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/modules/midi.h"
#include "../include/modules/mixer.h"
#include "../include/modules/sdl/sdl.h"
#include "../include/modules/sdl/audio.h"
#include "../include/modules/oscillator.h"
#include "../include/debugmalloc.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stdout, "ERROR: File name argument missing");
        return 1;
    }

    Mixer mixer;

    // Concatinate the midi file path
    char midi_filepath[64] = "midi/";
    strcat(midi_filepath, argv[1]);
    strcat(midi_filepath, ".mid");

    init_mixer(&mixer);
    read_midi(&mixer, midi_filepath);

    // Temporary
    mixer.channels[0].amplitude = 0.5;
    mixer.channels[0].osc.amplitude = 0.5;

    /*
    printf("\n\n## FINAL CHANNEL NOTES");
    for (int i = 0; i < 16; i++) {
        if (mixer.channels[i].notes == NULL) { continue; }

        printf("\n#### CHANNEL %d", i+1);
        print_notes(mixer.channels[i].notes);
    }
    */
    
    GUI gui = {0}; 
    gui.mixer = &mixer;
  
    if (!new_gui(&gui)) {
        return EXIT_FAILURE;
    }

    init_audio(&gui);

    run_gui(&gui);

    free_audio(&gui);
    free_gui(&gui);

    return EXIT_SUCCESS;
}
