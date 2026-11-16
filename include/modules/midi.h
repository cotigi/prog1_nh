#include "node.h"
#include "mixer.h"

#ifndef MIDI_H
#define MIDI_H
int get_channel_length(Node *base);
void read_midi(Mixer *mixer, char* file_name);
void print_notes(Node* node);
#endif // MIDI_H
