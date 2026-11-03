#include <stdlib.h>
#include <stdio.h>

#include "../../include/modules/note.h"

void push_note(Node* node, Note* note) {
    Node* current = node;

    if (current->note == NULL) { 
        current->note = note;
        return;
    }

    while (current->note != NULL && current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(Node));
    current = current->next;
    current->note = note;
    current->next = NULL;
}

Note* find_note_off(Node* node, int key) {
    Node* current = node;

    while (current != NULL) {

        if (current->note->key == key && current->note->end_frame == -1) {
            return current->note;
        }

        current = current->next;
    }

    return NULL;
}

void free_node(Node* node) {
    if (node == NULL) {
        return;
    }

    free_node(node->next);
    free(node->note);
    free(node);
}

/*
void pop_node(Node* node, Note* note) {
    Node* current = node;
    Node* next = NULL;

    if (current == NULL) { return; }

    while (current->next != NULL || current->next->note == note) {
        current = current->next;   
    }

    if (current->next == NULL) {
        return;
    }

    current->next = current->next->next;
}
*/
