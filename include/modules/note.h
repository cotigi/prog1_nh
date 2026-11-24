#ifndef NOTE_H
#define NOTE_H

typedef struct {
    int     key;
    int     sample_start;
    int     sample_end;
    int     sample_index;
    float   phase_index;
    float   phase_increment;
    // phase_index and phase_increment fields are float because
    // the final index will be determined through an interpolation.
    // For now this is the zero order interpolation.
    // Linear interpolation would be better but for now this will do.
    double  velocity;
    float   freq;
} Note;

double get_freq(int note_number);
float get_increment(double freq);
#endif // NOTE_H
