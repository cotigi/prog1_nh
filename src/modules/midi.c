#include <smf.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../include/modules/note.h"

void print_notes(Node* node) {
    Node* current = node;

    if (current == NULL || current->note == NULL) {
        fprintf(stdout, "ERROR: print_notes -> Empty linked list");
        return;
    }

    while (current != NULL) {
        printf(
            "\n\tKey: %d -> Pulse: %d<>%d",
            current->note->key,
            current->note->entry_frame,
            current->note->end_frame
        );

        current = current->next;
    }
}

void read(char* file_name) {
    smf_t *smf;
    smf_event_t *event;

    smf = smf_load(file_name);
    if (smf == NULL) {
        fprintf(stderr, "ERROR: Couldn't read MIDI file named:\n\t%s\n", file_name);
        return;
    }

    //Node channel_notes[16] = { {NULL} };    // Complete note list.
    Node channel_actives[16] = { {NULL} };  // Processing list to determine 
                                            // the length of each note.

    while ((event = smf_get_next_event(smf)) != NULL) {
        if (smf_event_is_metadata(event)) { continue; }

        unsigned char *buffer = event->midi_buffer;
        int type    = buffer[0] & 0xF0; 
        int channel = buffer[0] & 0x0F; // 0-15 

        if (type != 0x80 && type != 0x90) { continue; }
        // Only handle NOTE ON or NOTE OFF events.

        int key = buffer[1];
        int velocity = buffer[2];

        printf(
            "\tType: %04x | Channel: %d | Key: %d | Velocity: %d | Event: %s | Time: %d\n",
            type, 
            channel,
            key,
            velocity,
            (type == 0x90 && velocity != 0) ? "ON" : "OFF",
            event->time_pulses
        );

        if (type == 0x90 && velocity != 0) {
            // Sometimes a NOTE ON and velocity = 0 
            // means a NOTE OFF
            Note* note = malloc(sizeof(Note));
            note->key = key;
            note->entry_frame = event->time_pulses;
            note->end_frame = -1;
            note->phase = 0.0;
            // handle freq and increment
            push_note(&channel_actives[channel], note);
        } else if (type == 0x80 || velocity == 0) {
            Note* note = find_note_off(&channel_actives[channel], key);
            if (note == NULL) { 
                fprintf(stderr, "ERROR EVENT OFF: On event not found.\n");
                return;
            }
            note->end_frame = event->time_pulses;

            printf("Note done: %d -> %d-%d\n", note->key, note->entry_frame, note->end_frame);
        }
    }
    printf("\n\nRead done\n");

    smf_delete(smf);

    for (int i = 0; i < 16; i++) {
        if (channel_actives[i].note == NULL) { continue; }

        printf("\n#### CHANNEL %d", i+1);
        print_notes(&channel_actives[i]);

        free_node(channel_actives[i].next);
    }
}
