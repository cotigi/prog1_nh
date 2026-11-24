#include <SDL3/SDL_stdinc.h>

#include "../../include/modules/channel.h"
#include "../../include/modules/oscillator.h"
#include "../../include/modules/node.h"
#include "../../include/debugmalloc.h"

//  Description:
//      Synthesizez the whole channel (meaning all the active notes)
//      at the given sample index;
//  Arguments:
//      - Channel *channel: Pointer to the given channel;
//      - int sample_index: Index of the given sample to calculate;
//  Return -> Sint16:
//      Returns the amplitude at the given sample index for the channel;
double synthesize_channel(Channel *channel, int sample_index) {
    // Check if the current node has started yet. 
    while (channel->current_node != NULL && channel->current_node->note->sample_start == sample_index) {
        // If it has started add it to the active notes list.
        channel->active_notes = push_note(
            channel->active_notes, 
            channel->current_node->note
        );

        channel->current_node = channel->current_node->next;
    }

    // Poping every note that ends playing at the current sample index.
    Node *current_active_node = channel->active_notes;
    while (current_active_node != NULL) {
        Node *next = current_active_node->next;

        if (current_active_node->note->sample_end == sample_index) {
            channel->active_notes = pop_note(
                channel->active_notes,
                current_active_node->note
            );
        }

        /*
        if (current_active_node->note->sample_start < sample_index ||
            current_active_node->note->sample_end > sample_index) {
            channel->active_notes = pop_note(
                channel->active_notes,
                current_active_node->note
            );
        }
        */

        current_active_node = next;
    }

    // Go through all the active notes and calculate the amplitudes.
    double amplitude = 0;
    int note_count = 0;
    current_active_node = channel->active_notes;
    while (current_active_node != NULL) {
        amplitude += synthesize_note(
            &channel->osc,
            current_active_node->note
        );

        note_count++;
        current_active_node = current_active_node->next;
    }

    /*
    if (note_count != 0) {
        amplitude = (amplitude/note_count > 1.0) ?
            1.0 : (amplitude/note_count < -1.0) ?
                -1.0 :
                amplitude/note_count
            ;
    }
    */
    amplitude = (note_count != 0) ? amplitude/note_count : 0;
    amplitude = (amplitude >= 1.0) ?
        1.0 : (amplitude <= -1.0) ?
            -1.0 :
            amplitude
        ;

    return amplitude;
    //return (note_count != 0) ? amplitude/note_count : 0;
}
