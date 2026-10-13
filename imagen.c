#include "imagen.h"
#include <stdlib.h>
#include <string.h>

/**
 * ============================================================================
 * IMPLEMENTACIÓN: GESTIÓN DE MATRICES 2D DINÁMICAS (imagen.c)
 * ============================================================================
 * ¿Por qué esta técnica?
 *   Asignar una matriz como `Pixel **` permite indexar cómodamente como
 *   `img->matriz[y][x]`. Sin embargo, requiere asignar primero un arreglo de
 *   punteros a filas (`alto * sizeof(Pixel*)`) y luego un arreglo para cada
 *   fila (`ancho * sizeof(Pixel)`).
 *
 * Prevención de Memory Leaks (Rollback Pattern):
 *   Si la reserva falla en la fila 50 de 100, no podemos simplemente hacer
 *   `return NULL`, porque las primeras 49 filas quedarían flotando en la heap.
 *   El patrón de rollback itera hacia atrás liberando las filas ya asignadas
 *   antes de liberar el arreglo principal y salir.
 * ============================================================================
 */

ImagenBMP *crear_imagen(int ancho, int alto)
{
    /* Validación de dimensiones mínimas válidas */
    if (ancho <= 0 || alto <= 0) {
        return NULL;
    }

    /* 1. Reservar la estructura contenedora */
    ImagenBMP *img = (ImagenBMP *)malloc(sizeof(ImagenBMP));
    if (!img) {
        return NULL;
    }

    img->ancho = ancho;
    img->alto = alto;
    img->res_horizontal = 2835; /* ~72 DPI por defecto (2835 px/m) */
    img->res_vertical = 2835;

    /* 2. Reservar el arreglo de punteros a filas */
    img->matriz = (Pixel **)malloc(alto * sizeof(Pixel *));
    if (!img->matriz) {
        free(img);
        return NULL;
    }

    /* 3. Reservar cada fila individualmente */
    for (int y = 0; y < alto; y++) {
        img->matriz[y] = (Pixel *)malloc(ancho * sizeof(Pixel));
        if (!img->matriz[y]) {
            /* Fallo en una fila: aplicar Rollback de todas las filas previas */
            for (int k = 0; k < y; k++) {
                free(img->matriz[k]);
            }
            free(img->matriz);
            free(img);
            return NULL;
        }
    }

    return img;
}

void liberar_imagen(ImagenBMP *img)
{
    if (!img) {
        return;
    }

    /* 1. Liberar cada fila */
    if (img->matriz) {
        for (int y = 0; y < img->alto; y++) {
            if (img->matriz[y]) {
                free(img->matriz[y]);
            }
        }
        /* 2. Liberar el vector de punteros a filas */
        free(img->matriz);
        img->matriz = NULL;
    }

    /* 3. Liberar la estructura contenedora */
    free(img);
}

ImagenBMP *clonar_imagen(const ImagenBMP *orig)
{
    if (!orig) {
        return NULL;
    }

    /* Asignar nueva imagen con las mismas dimensiones */
    ImagenBMP *copia = crear_imagen(orig->ancho, orig->alto);
    if (!copia) {
        return NULL;
    }

    copia->res_horizontal = orig->res_horizontal;
    copia->res_vertical = orig->res_vertical;

    /* Copiar el contenido fila por fila mediante memcpy para máxima velocidad */
    for (int y = 0; y < orig->alto; y++) {
        memcpy(copia->matriz[y], orig->matriz[y], orig->ancho * sizeof(Pixel));
    }

    return copia;
}
