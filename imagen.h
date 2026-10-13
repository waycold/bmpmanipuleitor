#ifndef IMAGEN_H
#define IMAGEN_H

/**
 * ============================================================================
 * MÓDULO: IMAGEN (imagen.h)
 * ============================================================================
 * Responsabilidad:
 *   Definición de las estructuras de datos fundamentales para representar una
 *   imagen BMP en memoria mediante una matriz 2D dinámica de píxeles, junto
 *   con las primitivas de asignación segura de memoria, clonación y liberación.
 *
 * Concepto clave para el estudiante:
 *   En C, una matriz 2D dinámica se implementa típicamente como un doble
 *   puntero (Pixel **matriz), donde matriz es un vector de punteros a filas,
 *   y cada fila es un bloque contiguo de píxeles en memoria heap.
 * ============================================================================
 */

#include <stdint.h>
#include <stdbool.h>

#pragma pack(push, 1)
/**
 * Estructura de un píxel de 24 bits (3 bytes exactos).
 * IMPORTANTE: El formato BMP almacena los canales de color en orden BGR
 * (Blue, Green, Red), no RGB. Al usar #pragma pack(push, 1), garantizamos
 * que sizeof(Pixel) sea exactamente 3 bytes sin bytes de relleno por alineación.
 */
typedef struct {
    uint8_t b; /* Componente Azul  (0 a 255) */
    uint8_t g; /* Componente Verde (0 a 255) */
    uint8_t r; /* Componente Rojo  (0 a 255) */
} Pixel;
#pragma pack(pop)

/**
 * Estructura principal que representa una imagen BMP cargada en memoria.
 *
 * Sistema de coordenadas en memoria (orientación visual):
 *   - matriz[y][x]:
 *     y = índice de fila (0 = fila superior, alto - 1 = fila inferior)
 *     x = índice de columna (0 = borde izquierdo, ancho - 1 = borde derecho)
 *
 * Nota: Los archivos BMP en disco suelen guardarse invertidos (bottom-up).
 *       El módulo bmp_io se encarga de esa traducción al leer y escribir,
 *       permitiendo que la matriz en memoria se trabaje siempre de forma natural.
 */
typedef struct {
    int ancho;             /* Ancho de la imagen en píxeles */
    int alto;              /* Alto de la imagen en píxeles */
    Pixel **matriz;        /* Matriz 2D dinámica: matriz[fila][columna] */
    int32_t res_horizontal;/* Resolución horizontal en píxeles por metro */
    int32_t res_vertical;  /* Resolución vertical en píxeles por metro */
} ImagenBMP;

/* ========================================================================= */
/*                         PRIMITIVAS DE MEMORIA                             */
/* ========================================================================= */

/**
 * Reserva la memoria dinámica para la estructura ImagenBMP y su matriz 2D.
 * Si cualquier malloc falla en el proceso, libera ordenadamente toda la
 * memoria asignada hasta el momento y retorna NULL (evita memory leaks).
 */
ImagenBMP *crear_imagen(int ancho, int alto);

/**
 * Libera en orden inverso toda la memoria dinámica de una imagen:
 * primero cada una de las filas (matriz[y]), luego el vector de punteros
 * a filas (matriz) y finalmente la estructura contenedora.
 */
void liberar_imagen(ImagenBMP *img);

/**
 * Crea una copia profunda (deep-copy) independiente de una imagen existente.
 */
ImagenBMP *clonar_imagen(const ImagenBMP *orig);

#endif /* IMAGEN_H */
