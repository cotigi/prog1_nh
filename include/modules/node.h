#include "note.h"

#ifndef NODE_H
#define NODE_H
typedef struct Node {
    Note*           note;
    struct Node*    next;
} Node;

Node* push_note(Node *base, Note *note);
Note* find_note_off(Node *base, int key);
void free_list(Node *base);
Node* pop_note(Node *base, Note *node);
#endif // NODE_H
