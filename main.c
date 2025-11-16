#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "trie.h"

#define MAX_SEQUENCE_LENGTH 2000000
#define MAX_GENE_LENGTH     20
#define MAX_FILENAME_LENGTH 256

/* Validación para que un gen solo tenga caracteres válidos (ACGT) */
int validate_gene(const char* gene) {
    for (int i = 0; gene[i]; i++) {
        char c = toupper(gene[i]);
        if (c != 'A' && c != 'C' && c != 'G' && c != 'T')
            return 0;
    }
    return 1;
}

/* Convierte una cadena a mayúsculas */
void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++)
        str[i] = toupper(str[i]);
}

/* Lee una secuencia desde archivo (elimina \n y \r) */
int read_sequence(const char* filename, char* sequence, int max_len) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("Error: no se pudo abrir el archivo '%s'\n", filename);
        return 0;
    }
    if (!fgets(sequence, max_len, f)) {
        printf("Error: archivo vacio o no legible\n");
        fclose(f);
        return 0;
    }
    fclose(f);

    /* Elimina salto de línea */
    int len = strlen(sequence);
    if (len > 0 && (sequence[len-1] == '\n' || sequence[len-1] == '\r'))
        sequence[--len] = '\0';
    if (len > 0 && (sequence[len-1] == '\n' || sequence[len-1] == '\r'))
        sequence[--len] = '\0';

    to_uppercase(sequence);
    if (!validate_gene(sequence)) {
        printf("Error: la secuencia contiene caracteres invalidos\n");
        return 0;
    }
    return 1;
}

/* Carga todos los genes de tamaño m en el Trie */
void load_genes(Node* root, const char* sequence, int m) {
    int n = strlen(sequence);
    for (int i = 0; i <= n - m; i++) {
        char gene[MAX_GENE_LENGTH + 1];
        strncpy(gene, sequence + i, m);
        gene[m] = '\0';
        to_uppercase(gene);           // por si acaso
        insert(root, gene, i);
    }
}

void show_help() 
{
    printf("\nComandos disponibles:\n");
    printf("  start <m>        - Inicializar arbol con genes de dimension m\n");
    printf("  read <archivo>   - Cargar secuencia desde archivo\n");
    printf("  search <gen>     - Buscar un gen especifico\n");
    printf("  max              - Mostrar gen(es) mas frecuente(s)\n");
    printf("  min              - Mostrar gen(es) menos frecuente(s)\n");
    printf("  all              - Mostrar todos los genes\n");
    printf("  help             - Mostrar este menu\n");
    printf("  exit             - Salir del programa\n\n");
}

/* === RECORRIDO ITERATIVO DFS === */
static void dfs_traversal(Node* root, int m, int target_count, int show_all_flag, int* found)
{
    if (!root) return;
    const char* ALPH = "ACGT";
    char buf[MAX_GENE_LENGTH + 1];
    Node* stack[MAX_GENE_LENGTH + 1];
    int idx_stack[MAX_GENE_LENGTH + 1];
    int depth = 0;

    stack[0] = root;
    idx_stack[0] = 0;

    while (depth >= 0) {
        if (depth == m) {
            Node* leaf = stack[depth];
            int do_print = 0;
            if (show_all_flag)
                do_print = (leaf && leaf->count > 0);
            else if (target_count >= 0)
                do_print = (leaf && leaf->count == target_count);

            if (do_print && leaf->count > 0) 
            {
                buf[m] = '\0';
                printf("%s", buf);
                for (int k = 0; k < leaf->count; k++)
                    printf(" %d", leaf->positions[k]);
                printf("\n");
                *found = 1;
            }
            depth--;
            if (depth >= 0) idx_stack[depth]++;
            continue;
        }

        int i = idx_stack[depth];
        if (i >= 4) {
            depth--;
            if (depth >= 0) idx_stack[depth]++;
            continue;
        }

        Node* child = stack[depth] ? stack[depth]->children[i] : NULL;
        if (child) {
            buf[depth] = ALPH[i];
            depth++;
            stack[depth] = child;
            idx_stack[depth] = 0;
        } else {
            idx_stack[depth]++;
        }
    }
}

static void calculate_extreme(Node* root, int m, int* extreme, int* found, int is_max) {
    if (!root) return;

    Node* stack[MAX_GENE_LENGTH + 1];
    int idx_stack[MAX_GENE_LENGTH + 1];
    int depth = 0;

    stack[0] = root;
    idx_stack[0] = 0;

    while (depth >= 0) {
        if (depth == m) {
            Node* nd = stack[depth];
            if (nd && nd->count > 0) {
                if (!*found) {
                    *extreme = nd->count;
                    *found = 1;
                } else if ((is_max && nd->count > *extreme) || (!is_max && nd->count < *extreme)) {
                    *extreme = nd->count;
                }
            }
            depth--;
            if (depth >= 0) idx_stack[depth]++;
            continue;
        }

        int i = idx_stack[depth];
        if (i >= 4) {
            depth--;
            if (depth >= 0) idx_stack[depth]++;
            continue;
        }

        Node* child = stack[depth] ? stack[depth]->children[i] : NULL;
        if (child) {
            depth++;
            stack[depth] = child;
            idx_stack[depth] = 0;
        } else {
            idx_stack[depth]++;
        }
    }
}

/* === Comandos públicos === */
void show_max(Node* root, int m) {
    int max_count = 0, found = 0;
    calculate_extreme(root, m, &max_count, &found, 1);
    if (found) dfs_traversal(root, m, max_count, 0, &found);
}

void show_min(Node* root, int m) {
    int min_count = 0, found = 0;
    calculate_extreme(root, m, &min_count, &found, 0);
    if (found) dfs_traversal(root, m, min_count, 0, &found);
}

void show_all(Node* root, int m) {
    int found = 0;
    dfs_traversal(root, m, -1, 1, &found);
}

/* ====================== MAIN ====================== */
int main() {
    Node* root = NULL;
    int m = 0;
    int initialized = 0;
    int sequence_loaded = 0;
    char command[20];

    printf("Sistema de busqueda de genes en secuencias geneticas\n");
    show_help();

    while (1) {
        printf(">bio ");
        if (scanf("%s", command) != 1) break;

        if (strcmp(command, "start") == 0) {
            if (initialized) {
                printf("Error: el arbol ya esta inicializado. Use 'exit' primero.\n");
                while(getchar() != '\n');
                continue;
            }
            if (scanf("%d", &m) != 1 || m <= 0 || m > MAX_GENE_LENGTH) {
                printf("Error: tamaño de gen invalido (1-%d)\n", MAX_GENE_LENGTH);
                while(getchar() != '\n');
                continue;
            }
            root = create_node();
            if (!root) { printf("Error: no se pudo crear el arbol\n"); return 1; }
            initialized = 1;
            printf("Tree created with height %d\n", m);
        }

        else if (strcmp(command, "read") == 0) {
            if (!initialized) { printf("Error: use 'start' primero\n"); continue; }
            char filename[MAX_FILENAME_LENGTH];
            if (scanf("%s", filename) != 1) continue;

            char* sequence = malloc(MAX_SEQUENCE_LENGTH);
            if (!sequence) { printf("Error: sin memoria\n"); continue; }

            if (!read_sequence(filename, sequence, MAX_SEQUENCE_LENGTH)) {
                free(sequence); continue;
            }
            if ((int)strlen(sequence) < m) {
                printf("Error: la secuencia es mas corta que el tamaño del gen\n");
                free(sequence); continue;
            }

            load_genes(root, sequence, m);
            free(sequence);
            sequence_loaded = 1;
            printf("Sequence S read from file\n");
        }

        else if (strcmp(command, "search") == 0) {
            if (!initialized || !sequence_loaded) {
                printf("Error: inicialice y cargue secuencia primero\n"); continue;
            }
            char gene[MAX_GENE_LENGTH + 1];
            if (scanf("%s", gene) != 1) continue;
            to_uppercase(gene);
            if ((int)strlen(gene) != m) {
                printf("Error: el gen debe tener espacio: %d\n", m); continue;
            }
            if (!validate_gene(gene)) {
                printf("Error: caracteres invalidos en gen\n"); continue;
            }
            Node* res = search(root, gene);
            if (res && res->count > 0) {
                for (int i = 0; i < res->count; i++)
                    printf("%d ", res->positions[i]);
                printf("\n");
            } else printf("-1\n");
        }

        else if (strcmp(command, "max") == 0) {
            if (!initialized || !sequence_loaded) {
                printf("Error: inicialice y cargue secuencia primero\n"); continue;
            }
            show_max(root, m);
        }

        else if (strcmp(command, "min") == 0) {
            if (!initialized || !sequence_loaded) {
                printf("Error: inicialice y cargue secuencia primero\n"); continue;
            }
            show_min(root, m);
        }

        else if (strcmp(command, "all") == 0) {
            if (!initialized || !sequence_loaded) {
                printf("Error: inicialice y cargue secuencia primero\n"); continue;
            }
            show_all(root, m);
        }

        else if (strcmp(command, "help") == 0) show_help();

        else if (strcmp(command, "exit") == 0) {
            if (initialized) {
                printf("Clearing cache and exiting...\n");
                free_tree(root);
            } else printf("Exiting...\n");
            break;
        }

        else printf("Comando desconocido. Use 'help'\n");

        while(getchar() != '\n');  // limpia buffer
    }
    return 0;
}
