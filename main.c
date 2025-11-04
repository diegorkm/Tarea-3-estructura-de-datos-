#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"
#include <ctype.h>

#define MAX_SEQUENCE_LENGTH 1000
#define MAX_GENE_LENGTH 20
#define MAX_FILENAME_LENGTH 100

/*VALIDación para que un gen solo tenga
caracteres validos (ACGT)*/

int validate_gene(const char* gene)
{
    for (int i = 0; gene[i] != '\0'; i++)
    {
    char c = toupper(gene[i]);
        {
            if (c != 'A' && c != 'C' && c != 'G' && c != 'T')
            return 0; /*caracter invalido en algún lado*/
        }
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

    /*convertri caracteres a mayusculas y validar*/
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
    printf("  start <m>        - Inicializar arbol con genes de tamaño m\n");
    printf("  read <archivo>   - Cargar secuencia desde archivo\n");
    printf("  search <gen>     - Buscar un gen especifico\n");
    printf("  max              - Mostrar gen(es) más frecuente(s)\n");
    printf("  min              - Mostrar gen(es) menos frecuente(s)\n");
    printf("  all              - Mostrar todos los genes\n");
    printf("  help             - Mostrar este menu\n");
    printf("  exit             - Salir del programa\n\n");
}

int main() 
{
    Node* root = NULL;
    int m = 0;
    char command[20];
    char filename[50];

    printf(">bio start [m]: ");
    scanf("%d", &m);
    root = create_node();

    if (!root) 
    {
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

    for (int i = 0; i <= n - m; i++) 
    {
        char gene[10];
        strncpy(gene, &sequence[i], m);
        gene[m] = '\0';
        insert(root, gene, i);
    }

    printf("Sequence loaded successfully.\n");

    while (1) 
    {
        char command[20];
        printf(">bio ");
        scanf("%s", command);

        if (strcmp(command, "search") == 0) 
        {
            char gene[10];
            scanf("%s", gene);
            Node* result = search(root, gene);
            if (result && result->count > 0) 
            {
                for (int i = 0; i < result->count; i++)
                    printf("%d ", result->positions[i]);
                printf("\n");
            } else 
            {
                printf("-1\n");
            }
        } else if (strcmp(command, "exit") == 0) 
        {
            printf("Clearing cache and exiting...\n");
            break;
        } else 
        {
            printf("Comando no reconocido.\n");
        }
    }

    free_tree(root);
    return 0;
}