#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

int char_to_index(char c) 
{
    switch (c) 
    {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default: return -1;
    }
}

Node* create_node() 
{
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;

    for (int i = 0; i < ALPHABET_SIZE; i++)
        node->children[i] = NULL;

    node->positions = NULL;
    node->count = 0;
    return node;
}

void insert(Node* root, const char* gene, int position) 
{
    Node* current = root;
    /*baja por el arbol, sigue el gen*/
    for (int i = 0; gene[i] != '\0'; i++) 
    {
        int index = char_to_index(gene[i]);
        if (index == -1) return;
        if (current->children[index] == NULL)
            current->children[index] = create_node();

        current = current->children[index];
    }

/*al llegar a la hoja, agregar a la posición*/
    current->count++;

/*validación realloc, evitamos mem leaks*/
    int* temp = realloc(current->positions, current->count * sizeof(int));
    if (!temp)
    {
        /*SI REALLOC LLEGASE A FALLAAR, no perdemos el arreglo original*/
        fprintf(stderr, "Error: no se pudo asignar la memoria a las posiciones :(\n");
        current->count--; /*revierte el incremento*/
        return;
    }
    current->positions = temp;
    current->positions[current->count - 1] = position;
}

Node* search(Node* root, const char* gene) 
{
    Node* current = root;
    for (int i = 0; gene[i] != '\0'; i++) 
    {
        int index = char_to_index(gene[i]);
        if (index == -1 || current->children[index] == NULL)
            return NULL;
        current = current->children[index];
    }
    return current;
}

void free_tree(Node* node) 
{
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        free_tree(node->children[i]);
    free(node->positions);
    free(node);
}