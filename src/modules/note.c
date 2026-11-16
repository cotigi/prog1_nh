#include <SDL3/SDL_stdinc.h>

#include "../../include/modules/note.h"
#include "../../include/modules/constants.h"
#include "../../include/debugmalloc.h"

//  Arguments:
//      - int note_number: Number of the given note starting from 0;
//  Return -> double:
//      returns the frequency of the given note;
double get_freq(int note_number) {
    return SDL_pow(2, (note_number-49.0)/12)*TUNING;
}

//  Arguments:
//      - double freq: Frequency of a given note;
//  Return -> float:
//      Returns the phase increment value;
float get_increment(double freq) {
    return (freq * WAVETABLE_SAMPLERATE) / SAMPLERATE;
}

