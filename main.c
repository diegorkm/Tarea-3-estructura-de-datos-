#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

int main() {
    Node* root = NULL;
    int m;
    char filename[50];

    printf(">bio start [m]: ");
    scanf("%d", &m);
    root = create_node();

    if (!root) {
        printf("Error: no se pudo crear el árbol.\n");
        return 1;
    }

    printf("Tree created with height %d\n", m);

    printf(">bio read [archivo]: ");
    scanf("%s", filename);

    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Error al abrir el archivo.\n");
        free_tree(root);
        return 1;
    }

    char sequence[10000];
    fscanf(f, "%s", sequence);
    fclose(f);

    int n = strlen(sequence);

    for (int i = 0; i <= n - m; i++) {
        char gene[10];
        strncpy(gene, &sequence[i], m);
        gene[m] = '\0';
        insert(root, gene, i);
    }

    printf("Sequence loaded successfully.\n");

    while (1) {
        char command[20];
        printf(">bio ");
        scanf("%s", command);

        if (strcmp(command, "search") == 0) {
            char gene[10];
            scanf("%s", gene);
            Node* result = search(root, gene);
            if (result && result->count > 0) {
                for (int i = 0; i < result->count; i++)
                    printf("%d ", result->positions[i]);
                printf("\n");
            } else {
                printf("-1\n");
            }
        } else if (strcmp(command, "exit") == 0) {
            printf("Clearing cache and exiting...\n");
            break;
        } else {
            printf("Comando no reconocido.\n");
        }
    }

    free_tree(root);
    return 0;
}
