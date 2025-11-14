#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

/**
 * Convierte un carácter de nucleótido (A, C, G, T) a su índice correspondiente.
 * @param c: carácter en mayúscula (se asume validado previamente)
 * @return índice: 0=A, 1=C, 2=G, 3=T. -1 si es un carácter inválido.
 */
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

/**
 * Crea un nuevo nodo del Trie.
 * Inicializa todos los hijos a NULL y el arreglo de posiciones vacío.
 * @return Puntero al nodo creado, o NULL si falla malloc.
 */
Node* create_node()
{
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;

    for (int i = 0; i < ALPHABET_SIZE; i++)
        node->children[i] = NULL;

    node->positions = NULL;
    node->count = 0;
    node->capacity = 0;        // Capacidad actual del arreglo dinámico
    return node;
}

/**
 * Inserta un gen (k-mer) en el Trie junto con su posición de aparición.
 * @param root: raíz del Trie (nodo inicial)
 * @param gene: cadena del gen (ya en mayúsculas y validada)
 * @param position: posición inicial del gen en la secuencia (índice base 0)
 *
 * Estrategia:
 *  - Recorre/baja por el Trie siguiendo los caracteres del gen.
 *  - Crea nodos intermedios si no existen.
 *  - Al llegar a la hoja (final del gen), incrementa el contador y guarda la posición.
 *  - Usa realloc con crecimiento exponencial para minimizar realocaciones.
 */
void insert(Node* root, const char* gene, int position)
{
    Node* current = root;

    /* Baja por el árbol siguiendo el gen */
    for (int i = 0; gene[i] != '\0'; i++)
    {
        int idx = char_to_index(gene[i]);
        if (idx == -1)
            return;  // Carácter inválido → se ignora la inserción

        if (current->children[idx] == NULL)
            current->children[idx] = create_node();

        current = current->children[idx];
    }

    /* === Estamos en la hoja correspondiente al gen completo === */

    /* Redimensionar el arreglo de posiciones si es necesario (crecimiento exponencial) */
    if (current->count >= current->capacity)
    {
        int new_cap = (current->capacity == 0) ? 8 : current->capacity * 2;
        int* temp = (int*)realloc(current->positions, new_cap * sizeof(int));
        if (!temp)
        {
            fprintf(stderr, "Error: no hay memoria suficiente para almacenar posiciones\n");
            return;
        }
        current->positions = temp;
        current->capacity = new_cap;
    }

    /* Guardar la nueva posición */
    current->positions[current->count] = position;
    current->count++;
}

/**
 * Busca un gen en el Trie.
 * @param root: raíz del Trie
 * @param gene: gen a buscar (en mayúsculas)
 * @return Puntero al nodo hoja si existe, NULL si no se encuentra o hay caracteres inválidos.
 */
Node* search(Node* root, const char* gene)
{
    Node* current = root;

    for (int i = 0; gene[i] != '\0'; i++)
    {
        int idx = char_to_index(gene[i]);
        if (idx == -1 || current->children[idx] == NULL)
            return NULL;

        current = current->children[idx];
    }
    return current;  // La hoja del gen (puede tener count == 0 si nunca se insertó)
}

/**
 * Libera recursivamente toda la memoria ocupada por el Trie.
 * @param node: nodo actual (se llama con la raíz al inicio)
 *
 * Recorrido post-orden: primero libera hijos, luego el arreglo de posiciones,
 * y finalmente el nodo actual.
 */
void free_tree(Node* node)
{
    if (!node) return;

    for (int i = 0; i < ALPHABET_SIZE; i++)
        free_tree(node->children[i]);

    free(node->positions);  // Libera el arreglo dinámico de posiciones
    free(node);             // Libera el nodo
}
