#include <stdlib.h>

#include "../../include/modules/note.h"
#include "../../include/modules/node.h"
#include "../../include/debugmalloc.h"

//  Description:
//      Pushes a node to the given linked list 
//      passed as a pointer to the base node;
//  Arguments:
//      - Node *base: Pointer to the base of the linked list;
//      - Note *note: Pointer to the note to be added at the 
//          end of the linked list;
//  Return -> Node*:
//      Returns the possibly changed base pointer;
//      This is needed is the case that the base pointer 
//      is NULL and needs to be allocated in which case 
//      the holder of the base pointer wouldn't know 
//      about the change;
Node* push_note(Node *base, Note *note) {
    Node* current = base;

    // In case the base node is NULL 
    // allocate the new base and return it.
    if (current == NULL) {
        current = malloc(sizeof(Node));
        current->note = note;
        current->next = NULL;
        return current;
    }

    // If the base pointer exists but 
    // doesn't have an assigned note, 
    // assign and return it.
    if (current->note == NULL) { 
        current->note = note;
        return base;
    }

    // Loop until the last node is found.
    while (current->next != NULL) {
        current = current->next;
    }

    // Allocate the next node, step into it 
    // and assign the arguments.
    current->next = malloc(sizeof(Node));
    current = current->next;
    current->note = note;
    current->next = NULL;

    return base;
}

//  Description:
//      Finds the first note that matches the key and 
//      it's end sample index hasn't been assigned;
//  Arguments:
//      - Node *base: Pointer to the base node;
//      - int key: Key of the note indicated by it's number;
//  Return -> Note*:
//      Returns a pointer to the note that needs to be "closed";
//      Meaning that the note end_sample hasn't been assigned;
Note* find_note_off(Node *base, int key) {
    Node* current = base;

    // Loop until the note is found and return.
    // If the note is not found the loop ends
    // and returns NULL.
    while (current != NULL) {
        if (current->note->key == key && current->note->sample_end == -1) {
            return current->note;
        }

        current = current->next;
    }

    return NULL;
}

//  Description:
//      Frees all the nodes in the linked list along 
//      with the notes using recursion;
//  Arguments:
//      - Node *base: Pointer to the base of the linked list;
//  Return -> void:
//      No return value;
void free_list(Node* base) {
    while (base != NULL) {
        Node *next = base->next;
        free(base->note);
        free(base);
        base = next;
    }
}

//  Description:
//      Frees all the nodes in the linked list along 
//      with the notes using recursion;
//  Arguments:
//      - Node *base: Pointer to the base of the linked list;
//  Return -> void:
//      No return value;
void free_active_list(Node* base) {
    while (base != NULL) {
        Node *next = base->next;
        free(base);
        base = next;
    }
}

//  Description:
//      Pops a node from a given linked list specified by 
//      the base node pointer.
//      The note is found by compairing note pointers.
//
//      This function doesn't handling the freeing of these
//      data structure because they will be used for the 
//      active_notes list which will only store a 
//      pointer to the currently active notes to calculate.
//
//      This function must not be called on the actual 
//      list holding the notes as a whole (channel->notes).
//
//  Arguments:
//      - Node *base: Pointer to the base node of the linked list;
//      - Note *note: Pointer to the note to find;
//
//  Return -> Node*:
//      Returns the possibly new base node;
Node* pop_note(Node *base, Note *note) {
    if (base == NULL) { return base; }
    if (base->note == note) { 
        Node *next = base->next;
        free(base);
        return next; 
    }

    Node* current = base;

    while (current->next != NULL) {
        if (current->next->note == note) {
            Node *next = current->next->next;
            free(current->next);
            current->next = next;

            return base;
        }

        current = current->next;   
    }

    /*
    if (current->next->note == note) {
        Node *next = current->next->next;
        free(current->next);
        current->next = next;
    }
    */

    return base;
}
