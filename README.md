**Tarea 3 - Estructura de Datos**: Sistema de búsqueda de genes usando árboles
- Profesor: Christian Vásquez Rebolledo
## Autores  
# Grupo 2  
- Alexander Lucero  
- Simon Navarro  
- Diego Peralta  
- Alan Sánchez  
## Descripción  

Sistema que permite buscar patrones de genes cortos dentro de secuencias genéticas largas. Utiliza un árbol 4-ario (Trie) para almacenar y buscar eficientemente subsecuencias de ADN compuestas por las bases nucleotídicas: A, C, G, T.  

## Funcionalidades

- **Inicialización**: Crear árbol Trie con altura específica (tamaño del gen)
- **Carga de secuencias**: Leer secuencias de ADN desde archivos
- **Búsqueda individual**: Buscar un gen específico y mostrar sus posiciones
- **Análisis estadístico**: 
  - Encontrar genes más frecuentes
  - Encontrar genes menos frecuentes
  - Listar todos los genes presentes
- **Gestión de memoria**: Liberación adecuada de estructuras

## 🚀 Compilación

```bash
make
```

El ejecutable se llama `bio`.

## Uso

### Comandos disponibles

```bash
# Inicializar el árbol con genes de tamaño m
>bio start <m>

# Cargar secuencia desde archivo
>bio read <archivo.txt>

# Buscar un gen específico
>bio search <gen>

# Mostrar gen(es) más frecuente(s)
>bio max

# Mostrar gen(es) menos frecuente(s)
>bio min

# Mostrar todos los genes encontrados
>bio all

# Salir del programa
>bio exit
```

### Ejemplo de uso

```bash
>bio start 2
Tree created with height 2

>bio read adn.txt
Sequence S read from file

>bio search AA
4 7

>bio max
AA 4 7
AG 5 8
TA 0 3

>bio min
AC 1
CT 2
GA 6
GC 9

>bio all
AA 4 7
AC 1
AG 5 8
CT 2
GA 6
GC 9
TA 0 3

>bio exit
Clearing cache and exiting…
```

## Estructura del Proyecto

```
.
├── main.c          # Programa principal y manejo de comandos
├── trie.c          # Implementación del árbol Trie
├── trie.h          # Definiciones de estructuras y funciones
├── Makefile        # Script de compilación
├── README.md       # Documentación
├── .gitignore      # Archivos ignorados por Git
└── adn.txt         # Archivo de ejemplo con secuencia genética
```

## Formato de Archivos de Entrada

Los archivos de secuencias genéticas deben:
- Contener solo caracteres: A, C, G, T
- Estar en una sola línea
- Terminar con un salto de línea

Ejemplo (`adn.txt`):
```
TACTAAGAAGC
```

## Notas de Desarrollo

- El árbol Trie tiene una altura igual al tamaño del gen buscado (m)
- Cada nodo interno puede tener hasta 4 hijos (uno por cada base)
- Las hojas contienen listas con las posiciones donde aparece el gen

## 🔧 Limpieza

Para eliminar los archivos compilados:
```bash
make clean
```

## Fecha de Entrega

17 de noviembre de 2025 - 23:59