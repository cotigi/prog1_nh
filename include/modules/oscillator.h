#include <SDL3/SDL_stdinc.h>
#include <stdatomic.h>

#include "./note.h"
#include "./constants.h"

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

typedef struct {
    double      waveform[WAVETABLE_SAMPLERATE];
    char        name[32];
} Waveform;

typedef struct {
    Waveform *_Atomic   waveform;
    _Atomic int selected_waveform;
    _Atomic double      amplitude;
    _Atomic double      limit;
} Oscillator;

double synthesize_note(Oscillator *osc, Note *note);
void next_phase(Note *note);
double envelope(double amplitude, Note *note);
#endif // OSCILLATOR_H
