#include <SDL3/SDL_stdinc.h>

#include "./note.h"
#include "./constants.h"

#ifndef OSCILLATOR_H
#define OSCILLATOR_H
typedef struct {
    double waveform[WAVETABLE_SAMPLERATE];
    double amplitude;
    // TODO: Envelope
} Oscillator;

void read_waveform(Oscillator *osc, char *name);
double synthesize_note(Oscillator *osc, Note *note);
double envelope(double amplitude, Note *note);
#endif // OSCILLATOR_H
