#ifndef FILTROS_H
#define FILTROS_H

/**
 * ============================================================================
 * MÓDULO: FILTROS (filtros.h)
 * ============================================================================
 * Responsabilidad:
 *   Contiene las funciones puras de procesamiento de imágenes.
 *   Cada filtro recibe una estructura `ImagenBMP` de entrada (de sólo lectura)
 *   y produce una nueva estructura `ImagenBMP` con la transformación aplicada,
 *   garantizando la inmutabilidad de la imagen original.
 * ============================================================================
 */

#include "imagen.h"

/* ========================================================================= */
/*                              FILTROS BÁSICOS                              */
/* ========================================================================= */

/**
 * Invierte los colores de la imagen: nuevo = 255 - original.
 */
ImagenBMP *filtro_negativo(const ImagenBMP *src);

/**
 * Convierte a escala de grises promediando aritméticamente RGB:
 * gris = (r + g + b) / 3.
 */
ImagenBMP *filtro_escala_de_grises(const ImagenBMP *src);

/**
 * Voltea la imagen horizontalmente (espejo izquierda-derecha).
 */
ImagenBMP *filtro_espejar_horizontal(const ImagenBMP *src);

/**
 * Voltea la imagen verticalmente (espejo arriba-abajo).
 */
ImagenBMP *filtro_espejar_vertical(const ImagenBMP *src);

/* ========================================================================= */
/*                      FILTROS CON PARÁMETROS (0-100%)                      */
/* ========================================================================= */

/**
 * Aumenta el contraste en un X% alejando los valores del punto medio (128).
 */
ImagenBMP *filtro_aumentar_contraste(const ImagenBMP *src, int porcentaje);

/**
 * Reduce el contraste en un X% acercando los valores al punto medio (128).
 */
ImagenBMP *filtro_reducir_contraste(const ImagenBMP *src, int porcentaje);

/**
 * Incrementa en un X% la intensidad del canal azul con clampeo a 255.
 */
ImagenBMP *filtro_tonalidad_azul(const ImagenBMP *src, int porcentaje);

/**
 * Incrementa en un X% la intensidad del canal verde con clampeo a 255.
 */
ImagenBMP *filtro_tonalidad_verde(const ImagenBMP *src, int porcentaje);

/**
 * Incrementa en un X% la intensidad del canal rojo con clampeo a 255.
 */
ImagenBMP *filtro_tonalidad_roja(const ImagenBMP *src, int porcentaje);

/**
 * Mantiene solo el X% del tamaño original comenzando desde la esquina
 * inferior izquierda y descartando el resto (rango válido: 1-100).
 */
ImagenBMP *filtro_recortar(const ImagenBMP *src, int porcentaje);

/**
 * Reduce las dimensiones de la imagen reescalándola al X% mediante muestreo
 * por vecino más cercano (rango válido: 1-100).
 */
ImagenBMP *filtro_achicar(const ImagenBMP *src, int porcentaje);

/* ========================================================================= */
/*                                ROTACIONES (90°)                           */
/* ========================================================================= */

/**
 * Rota la imagen 90° en sentido horario (intercambia ancho por alto).
 */
ImagenBMP *filtro_rotar_derecha(const ImagenBMP *src);

/**
 * Rota la imagen 90° en sentido antihorario (intercambia ancho por alto).
 */
ImagenBMP *filtro_rotar_izquierda(const ImagenBMP *src);

/* ========================================================================= */
/*                              CONCATENACIONES                              */
/* ========================================================================= */

/**
 * Une dos imágenes horizontalmente lado a lado (primero img1).
 * Si las alturas difieren, rellena el sobrante con color teal.
 */
ImagenBMP *filtro_concatenar_horizontal(const ImagenBMP *img1, const ImagenBMP *img2);

/**
 * Une dos imágenes verticalmente una sobre otra (img1 arriba).
 * Si los anchos difieren, rellena el sobrante con color teal.
 */
ImagenBMP *filtro_concatenar_vertical(const ImagenBMP *img1, const ImagenBMP *img2);

/* ========================================================================= */
/*                                  COMODÍN                                  */
/* ========================================================================= */

/**
 * Filtro personalizado del grupo: Efecto Sepia (vintage cálido)
 * mediante matriz cromática estándar.
 */
ImagenBMP *filtro_comodin(const ImagenBMP *src);

#endif /* FILTROS_H */
