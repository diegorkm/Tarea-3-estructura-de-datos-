#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "trie.h"

#define MAX_SEQUENCE_LENGTH 1000
#define MAX_GENE_LENGTH 20
#define MAX_FILENAME_LENGTH 100

/*Validación para que un gen solo tenga
caracteres validos (ACGT)*/

int validate_gene(const char* gene)
{
    for (int i = 0; gene[i] != '\0'; i++)
    {
        char c = toupper(gene[i]);
        if (c != 'A' && c != 'C' && c != 'G' && c != 'T')
        return 0; /*caracter invalido en algún lado*/
    }
    return 1; /*gen valido*/
}

/*convierte un gen a mayusculas*/
void to_uppercase(char* gene)
{
    for (int i = 0; gene[i] != '\0'; i++)
    {
        gene[i] = toupper(gene[i]);
    }
}

/**
 * procesa la secuencia y carga todos los genes desde el trie
 * @param root: raiz del trie
 * @param sequence: secuencia génetica
 * @param m: tamaño del gen
 */ 
void load_genes(Node* root, const char* sequence, int m)
{
    int n = strlen(sequence);

    for (int i = 0; i <= n - m; i++)
    {
        char gene[MAX_GENE_LENGTH];
        strncpy(gene, &sequence[i], m);
        gene[m] = '\0';
        insert(root, gene, i);
    }
}

int read_sequence(const char* filename, char* sequence, int max_length)
{
    FILE* f = fopen(filename, "r");
    if (!f)
    {
        printf("Error: no se pudo abrir el archivo '%s' :(\n", filename);
        return 0;
    }

    if (!fgets(sequence, max_length, f))
    {
        printf("error: el archivo esta vacio o no se pudo leer\n");
        fclose(f);
        return 0;
    }

    fclose(f);

    /*eliminar ese salto de linea raro del final*/
    int len = strlen(sequence);
    if (len > 0 && sequence[len - 1] == '\n')
    {
        sequence[len - 1] = '\0';
        len--;
    }

    if (len > 0 && sequence[len - 1] == '\r')
    {
        sequence[len - 1] = '\0';
        len--;
    }

    /*convertir caracteres a mayusculas y validar*/
    to_uppercase(sequence);
    if (!validate_gene(sequence))
    {
        printf("Error: la secuencia contiene caracteres invalidos.\n");
        return 0;
    }

    return 1; /*lectura exitosa!!!*/
}

/**/
void show_help() 
{
    printf("\nComandos disponibles:\n");
    printf("  start <m>        - Inicializar arbol con genes de tamanyo m\n");
    printf("  read <archivo>   - Cargar secuencia desde archivo\n");
    printf("  search <gen>     - Buscar un gen especifico\n");
    printf("  max              - Mostrar gen(es) mas frecuente(s)\n");
    printf("  min              - Mostrar gen(es) menos frecuente(s)\n");
    printf("  all              - Mostrar todos los genes\n");
    printf("  help             - Mostrar este menu\n");
    printf("  exit             - Salir del programa\n\n");
}

/*
show_min
Imprime todos los genes (de largo m) que aparecen la MENOR cantidad de veces (>0)
en la secuencia cargada en el trie, junto con sus posiciones.

Parámetros:
    - root: raíz del trie ya poblado con insert() (tras ejecutar "read <archivo>")
    - m   : largo del gen (altura lógica del trie)

Supuestos:
    - El trie ya fue construido y cargado (start m -> read archivo).
    - MAX_GENE_LENGTH está definido (usado para buffers locales).

Formato de salida:
    Una línea por gen: "GEN pos1 pos2 ...", en orden A,C,G,T.

Estrategia:
    1) Recorrido iterativo (DFS) para calcular el menor conteo > 0 (min_count).
    2) Segundo recorrido para imprimir todos los genes cuyo count == min_count.

Complejidad:
    - Tiempo: O(N) sobre el número de nodos visitados (en la práctica, hojas existentes).
    - Memoria: O(m) por la pila manual y el buffer del gen.
 */
void show_min(Node* root, int m)
{
    if (root == NULL) return;

    /* Mapeo índice->carácter en el orden A,C,G,T para salidas  */
    const char* ALPH = "ACGT";

    /* Buffer donde vamos armando el gen durante el descenso (m + 1 para '\0') */
    char buf[MAX_GENE_LENGTH + 1];

    /* Pilas manuales para un DFS iterativo:
       - node_stack: nodo actual por cada profundidad [0..m]
       - idx_stack : siguiente hijo a explorar (0..3) por cada profundidad */
    Node* node_stack[MAX_GENE_LENGTH + 1];
    int   idx_stack[MAX_GENE_LENGTH + 1];

    int depth;   /* profundidad actual (0 = raíz) */
    int i;       /* índice de hijo a explorar en el nivel 'depth' */

    /* min_count guarda el mínimo count > 0 observado; -1 significa "aún no definido"  */
    int min_count = -1;

    /* 

    PASO 1: hallar min_count (>0) 

    */

    /* Inicializamos el DFS en la raíz */
    depth = 0;
    node_stack[0] = root;
    idx_stack[0] = 0;

    for (;;) {
        /* Caso 1: alcanzamos profundidad m -> estamos en hoja lógica (gen completo) */
        if (depth == m) {
            Node* nd = node_stack[depth];

            /* Si esta hoja representa un gen presente (count>0), actualizamos min_count */
            if (nd != NULL && nd->count > 0) {
                if (min_count == -1 || nd->count < min_count) {
                    min_count = nd->count;
                }
            }

            /* Backtrack: si ya estamos en la raíz, terminó el recorrido */
            if (depth == 0) break;

            /* Retrocedemos un nivel y avanzamos al siguiente hijo de ese nivel */
            depth--;
            idx_stack[depth]++;
            continue;
        }

        /* Caso 2: aún no llegamos a m => seguimos explorando hijos en este nivel */
        i = idx_stack[depth];

        /* Si ya miramos los 4 hijos en este nivel, hacemos backtrack */
        if (i >= 4) {
            if (depth == 0) break;   /* no hay más por explorar en la raíz */
            depth--;
            idx_stack[depth]++;
            continue;
        }

        /* Tomamos el hijo i-ésimo del nodo actual (si existe) */
        Node* cur = node_stack[depth];
        Node* child = NULL;
        if (cur != NULL) {
            child = cur->children[i];
        }

        if (child != NULL) {
            /* Si existe el hijo:
               - fijamos el carácter correspondiente en el buffer
               - bajamos un nivel y reiniciamos el índice de hijos en ese nuevo nivel */
            buf[depth] = ALPH[i];
            depth++;
            node_stack[depth] = child;
            idx_stack[depth] = 0;
            /* NOTA: no incrementamos idx_stack[depth-1] aquí; se incrementa al volver (backtrack) */
            continue;
        }
        else {
            /* Si el hijo no existe, pasamos al siguiente hijo en este mismo nivel */
            idx_stack[depth]++;
            continue;
        }
    }

    /* Si no encontramos ningún gen con count>0, no hay nada que imprimir */
    if (min_count == -1) return;

    /*
    
    PASO 2: imprimir todos los genes con count == min_count

    */

    /* Repetimos el mismo DFS iterativo, al llegar al nivel m, imprimimos sólo los que tengan count == min_count */
    depth = 0;
    node_stack[0] = root;
    idx_stack[0] = 0;

    for (;;) {
        if (depth == m) {
            Node* nd = node_stack[depth];

            if (nd != NULL && nd->count == min_count && nd->count > 0) {
                /* Cerramos el string y emitimos: GEN pos1 pos2 ... */
                buf[m] = '\0';
                printf("%s", buf);
                for (int k = 0; k < nd->count; k++) {
                    printf(" %d", nd->positions[k]);
                }
                printf("\n");
            }

            if (depth == 0) break;
            depth--;
            idx_stack[depth]++;
            continue;
        }

        i = idx_stack[depth];
        if (i >= 4) {
            if (depth == 0) break;
            depth--;
            idx_stack[depth]++;
            continue;
        }

        Node* cur = node_stack[depth];
        Node* child = NULL;
        if (cur != NULL) {
            child = cur->children[i];
        }

        if (child != NULL) {
            buf[depth] = ALPH[i];
            depth++;
            node_stack[depth] = child;
            idx_stack[depth] = 0;
            continue;
        }
        else {
            idx_stack[depth]++;
            continue;
        }
    }
}

int main() 
{
    Node* root = NULL;
    int m = 0;
    int initialized = 0;
    int sequence_loaded = 0;
    char command[20];

    printf("Sistema de busqueda de genes en secuencias geneticas\n");
    show_help();

    while (1)
    {
    printf(">bio ");
    if (scanf("%s", command) != 1)
    {
        break;
    }
        // Comando: start
        if (strcmp(command, "start") == 0) 
        {
            if (initialized) 
            {
                printf("Error: el arbol ya esta inicializado. Use 'exit' primero.\n");
                continue;
            }

            if (scanf("%d", &m) != 1 || m <= 0 || m > MAX_GENE_LENGTH) 
            {
                printf("Error: tamaño de gen invalido (debe ser 1-%d)\n", MAX_GENE_LENGTH);
                while (getchar() != '\n'); // Limpiar buffer
                continue;
            }

            root = create_node();
            if (!root) 
            {
                printf("Error: no se pudo crear el arbol\n");
                return 1;
            }

            initialized = 1;
            printf("Tree created with height %d\n", m);
        }
        
        /* comndo: read*/
        else if (strcmp(command, "read") == 0) 
        {
            if (!initialized) 
            {
                printf("Error: primero debe inicializar el arbol con 'start'\n");
                continue;
            }

            char filename[MAX_FILENAME_LENGTH];
            if (scanf("%s", filename) != 1) 
            {
                printf("Error: nombre de archivo invalido\n");
                continue;
            }

            char sequence[MAX_SEQUENCE_LENGTH];
            if (!read_sequence(filename, sequence, MAX_SEQUENCE_LENGTH)) 
            {
                continue;
            }

            int n = strlen(sequence);
            if (n < m) 
            {
                printf("Error: la secuencia es mas corta que el tamaño del gen\n");
                continue;
            }

            load_genes(root, sequence, m);
            sequence_loaded = 1;
            printf("Sequence S read from file\n");
        }
        
        // Comando: search
        else if (strcmp(command, "search") == 0) 
        {
            if (!initialized || !sequence_loaded) 
            {
                printf("Error: primero debe inicializar y cargar una secuencia\n");
                continue;
            }

            char gene[MAX_GENE_LENGTH];
            if (scanf("%s", gene) != 1) 
            {
                printf("Error: gen invalido\n");
                continue;
            }

            to_uppercase(gene);

            // validar longitud del gen
            if ((int)strlen(gene) != m) 
            {
                printf("Error: el gen debe tener tamaño %d\n", m);
                continue;
            }

            /*validación de caracteres*/
            if (!validate_gene(gene)) 
            {
                printf("Error: la secuencia contiene caracteres invalidos (solo A, C, G, T)\n");
                continue;
            }

            Node* result = search(root, gene);
            if (result && result->count > 0) 
            {
                for (int i = 0; i < result->count; i++) 
                {
                    printf("%d ", result->positions[i]);
                }
                printf("\n");
            } else 
            {
                printf("-1\n");
            }
        }
        
        // Comando: max (TODO: implementar)
        else if (strcmp(command, "max") == 0) 
        {
            if (!initialized || !sequence_loaded) 
            {
                printf("Error: primero debe inicializar y cargar una secuencia\n");
                continue;
            }
            printf("Comando 'max' en desarrollo\n");
        }
        
        // Comando: min
        else if (strcmp(command, "min") == 0)
        {
            if (!initialized || !sequence_loaded)
            {
                printf("Error: primero debe inicializar y cargar una secuencia\n");
                continue;
            }
            show_min(root, m);
            }
        
        // Comando: all (TODO: implementar)
        else if (strcmp(command, "all") == 0) 
        {
            if (!initialized || !sequence_loaded) 
            {
                printf("Error: primero debe inicializar y cargar una secuencia\n");
                continue;
            }
            printf("Comando 'all' no implementado :(\n");
        }
        
        // Comando: help
        else if (strcmp(command, "help") == 0) 
        {
            show_help();
        }
        
        // Comando: exit
        else if (strcmp(command, "exit") == 0) 
        {
            if (initialized) 
            {
                printf("Clearing cache and exiting...\n");
                free_tree(root);
            } else 
            {
                printf("Exiting...\n");
            }
            break;
        }
        
        // Comando no reconocido
        else 
        {
            printf("Comando no reconocido. Use 'help' para ver comandos disponibles.\n");
        }
    }

    return 0;
}