#define TABLE_LENGTH 4096
#define SAMPLERATE 44100

typedef struct {
    int     key;
    int     entry_frame;
    int     end_frame;
    double  phase; 
    double  increment;
    float   freq;
} Note;

typedef struct Node {
    Note*           note;
    struct Node*    next;
} Node;

typedef struct {
    Node    notes;
    Note**  active_notes; // might need a better structure
    double  amplitude;
} Oscillator;

void push_note(Node* notes, Note* note);
Note* find_note_off(Node* node, int note);
void free_node(Node* node);

//void pop_node(Node* node, Note* note);
