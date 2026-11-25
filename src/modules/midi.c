#include <smf.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/modules/note.h"
#include "../../include/modules/node.h"
#include "../../include/modules/mixer.h"
#include "../../include/modules/constants.h"
#include "../../include/debugmalloc.h"

//  Description:
//      Gets how long does the given channel play
//      in samples by grabbing the sample_end field of the last note;
//  Arguments:
//      - Node *base: Pointer to the channels note list;
//  Return -> int:
//      Returns the last sample that is relevant to the given channel;
int get_channel_length(Node *base) {
    Node *current = base;
    int max = 0;

    while (current != NULL) {
        max = (current->note->sample_end > max) ? current->note->sample_end : max;
        current = current->next;
    }

    return max;
}

//  Description:
//      Prints out all the notes for a given channel note list;
//      This is only used for debugging;
//  Arguments:
//      - Node *node: Pointer to the channels note list;
//  Return -> void:
//      No return value;
void print_notes(Node *base) {
    Node* current = base;

    if (current == NULL || current->note == NULL) {
        fprintf(stdout, "ERROR: print_notes -> Empty linked list");
        return;
    }

    while (current != NULL) {
        printf(
            "\n\tKey: %d (%lf) -> Pulse: %d<>%d",
            current->note->key,
            current->note->velocity,
            current->note->sample_start,
            current->note->sample_end
        );

        current = current->next;
    }
}

//  Description:
//      Reads following information from the given midi file:
//          - tempo
//          - notes (ON and OFF events)
//  Arguments:
//      - Mixer *mixer: Pointer to the mixer;
//      - char *file_name: Name of the midi file.
//          This only contains the name ("test") and 
//          not the path ("midi/") or extension (".mid");
//  Return -> void:
//      No return value;
void read_midi(Mixer *mixer, char *file_name) {
    smf_t *smf;
    smf_event_t *event;

    smf = smf_load(file_name);

    if (smf == NULL) {
        fprintf(stderr, "ERROR: Couldn't read MIDI file named:\n\t%s\n", file_name);
        return;
    }

    smf_tempo_t *tempo = smf_get_tempo_by_pulses(smf, 0);

    mixer->tempo = 6000000.0/tempo->microseconds_per_quarter_note;

    while ((event = smf_get_next_event(smf)) != NULL) {
        if (smf_event_is_metadata(event)) { continue; }

        unsigned char *buffer = event->midi_buffer;
        int type    = buffer[0] & 0xF0; 
        int channel = buffer[0] & 0x0F; // 0-15 

        if (type != 0x80 && type != 0x90) { continue; }
        // Only handle NOTE ON or NOTE OFF events.

        int key = buffer[1];
        int velocity = buffer[2];

        if (type == 0x90 && velocity != 0) {
            // Sometimes a NOTE ON and velocity = 0 
            // means a NOTE OFF
            Note* note = malloc(sizeof(Note));
            note->key = key;
            note->sample_start = event->time_seconds * SAMPLERATE;
            note->sample_end = -1;
            note->sample_index = 0;
            note->freq = get_freq(key);
            note->phase_index = 0;
            note->phase_increment = get_increment(note->freq);
            note->velocity = velocity/127.0;

            mixer->channels[channel].notes = push_note(mixer->channels[channel].notes, note);
        } else if (type == 0x80 || velocity == 0) {
            Note* note = find_note_off(mixer->channels[channel].notes, key);
            if (note == NULL) { 
                fprintf(stderr, "ERROR EVENT OFF: On event not found.\n\tNote will be ignored");
                return;
            } 
            note->sample_end = event->time_seconds * SAMPLERATE;
        }
    }

    for (int i = 0; i<16; i++) {
        if (mixer->channels[i].notes != NULL) {
            // Select the current node for synthesis.
            // This is always the first the will go further along
            // the linked list as the current note has started playing.
            mixer->channels[i].current_node = mixer->channels[i].notes;

            // Get the sample length of all the channels and select the maximum.
            // This is needed to automatically stop the audio stream from calling
            // the callback function when no audio is supposed to be playing.
            int sample_length = get_channel_length(mixer->channels[i].notes);
            mixer->sample_length = (sample_length > mixer->sample_length) ? sample_length : mixer->sample_length;

            mixer->active_channel_count++;
        }
    }

    smf_delete(smf);
}
