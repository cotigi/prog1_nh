//#include <stdlib.h>
//#include <string.h>
#include <SDL3/SDL_stdinc.h>
#include <stdatomic.h>

#include "../../include/modules/oscillator.h"
#include "../../include/modules/note.h"
#include "../../include/modules/constants.h"
#include "../../include/modules/helper.h"
#include "../../include/debugmalloc.h"

//  Arguments:
//      - Oscillator *osc: Pointer to the given oscillator needed
//          for the waveform indexing;
//      - Note *note: Pointer to the given note;
//  Return -> Sint16:
//      Returns the notes calculated amplitude as a signed 16 bit integer;
double synthesize_note(Oscillator *osc, Note *note) {
    // Zero order interpolation;
    // The phase index is floored and converted to an int;
    Waveform *waveform = atomic_load(&osc->waveform);
    int index = (int)SDL_floorf(note->phase_index);

    double amplitude = waveform->waveform[index]*note->velocity;
    amplitude = envelope(amplitude, note);

    amplitude = limit(amplitude, atomic_load(&osc->limit));

    amplitude *= atomic_load(&osc->amplitude);

    return amplitude;
}

void next_phase(Note *note) {
    // Increment the phase and modulate it with the wavetable samplerate;
    note->sample_index++;
    note->phase_index += note->phase_increment;
    note->phase_index = (note->phase_index >= WAVETABLE_SAMPLERATE) ? 
        0 : note->phase_index;
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
