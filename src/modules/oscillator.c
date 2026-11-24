#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL_stdinc.h>

#include "../../include/modules/oscillator.h"
#include "../../include/modules/note.h"
#include "../../include/modules/constants.h"
#include "../../include/debugmalloc.h"

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
void read_waveform(Oscillator *osc, char *name) {
    // Concatinate the path, name and extension.
    char filepath[32] = "saves/waveforms/";
    strcat(filepath, name);
    strcat(filepath, ".txt");

    FILE* file = fopen(filepath, "r");

    if (file == NULL) {
        fprintf(stdout, "ERROR: Couldn't read waveform file:\n\t%s", name); 
        return;
    }

    int index = 0;
    char buff[70];

    // Every waveform is saved as a signed 16 bit value is the file. 
    // The loop goes until the table is full or the file has ended.
    while (index < WAVETABLE_SAMPLERATE && fgets(buff, sizeof(buff), file)) {
        osc->waveform[index] = (double)atof(buff);
        index++;
    }

    fclose(file);
}

//  Arguments:
//      - Oscillator *osc: Pointer to the given oscillator needed
//          for the waveform indexing;
//      - Note *note: Pointer to the given note;
//  Return -> Sint16:
//      Returns the notes calculated amplitude as a signed 16 bit integer;
double synthesize_note(Oscillator *osc, Note *note) {
    // Zero order interpolation;
    // The phase index is floored and converted to an int;
    int index = (int)SDL_floorf(note->phase_index);

    double amplitude = osc->waveform[index]*note->velocity;
    amplitude = envelope(amplitude, note);

    // Increment the phase and modulate it with the wavetable samplerate;
    note->sample_index++;
    note->phase_index += note->phase_increment;
    note->phase_index = (note->phase_index >= WAVETABLE_SAMPLERATE) ? 
        0 : note->phase_index;

    return amplitude;
}

double envelope(double amplitude, Note *note) {
    /*
    if (note->sample_index < 1500) {
        return amplitude*SDL_sin(note->sample_index/1000.0);
    } else if (note->sample_end - note->sample_index < 1500) {
        return amplitude*SDL_cos((note->sample_end-note->sample_index)/1000.0);
    }
    */

    double t = (double)note->sample_index/(note->sample_end-note->sample_start);

    
    return amplitude*(0.25/(t+0.25));
}
