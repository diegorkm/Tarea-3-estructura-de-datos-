#ifndef TRIE_H
#define TRIE_H

#define ALPHABET_SIZE 4  // A, C, G, T

typedef struct Node {
    struct Node* children[ALPHABET_SIZE];
    int* positions;
    int count;
    int capacity;  // Para crecimiento eficiente del array
} Node;

int char_to_index (char c);
Node* create_node();
void insert(Node* root, const char* gene, int position);
Node* search(Node* root, const char* gene);
void free_tree(Node* node);

#endif
