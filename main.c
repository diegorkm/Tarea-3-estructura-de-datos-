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
    printf("  max              - Mostrar gen(es) mas frecuente(s)\n");
    printf("  min              - Mostrar gen(es) menos frecuente(s)\n");
    printf("  all              - Mostrar todos los genes\n");
    printf("  help             - Mostrar este menu\n");
    printf("  exit             - Salir del programa\n\n");
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
    printf(">bio: ");
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

            // validar longitud del genn*/
            if (strlen(gene) != m) 
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
        
        /* Comando: min (TODO: implementar)*/
        else if (strcmp(command, "min") == 0) 
        {
            if (!initialized || !sequence_loaded) 
            {
                printf("error: primero debe inicializar y cargar una secuencia\n");
                continue;
            }
            printf("comando 'min' no implementado :(\n");
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