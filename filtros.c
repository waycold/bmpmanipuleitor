#include "filtros.h"
#include <stdlib.h>

/**
 * ============================================================================
 * IMPLEMENTACIÓN: TRANSFORMACIONES Y FILTROS SOBRE MATRICES 2D (filtros.c)
 * ============================================================================
 * Conceptos Matemáticos:
 *
 * 1. Clamping (Asegurar rango [0, 255]):
 *    Al multiplicar o sumar canales, el resultado puede exceder 255 o caer bajo 0.
 *    La función `clampear_byte()` asegura que el valor quepa en uint8_t sin overflow.
 *
 * 2. Fórmula de Contraste:
 *    El contraste expande o comprime la distancia de cada tono respecto al
 *    punto medio de luminosidad (128):
 *      Factor = 1.0 + (porcentaje / 100.0)  [Aumentar]
 *      Factor = 1.0 - (porcentaje / 100.0)  [Reducir]
 *      Nuevo = 128.0 + (Original - 128.0) * Factor
 *
 * 3. Rotación de Coordenadas 90°:
 *    Original: Dimensiones (W, H). Matriz[y][x] con 0 <= y < H, 0 <= x < W.
 *    Rotada:   Dimensiones (H, W).
 *    Horario:      Nueva_Fila = x,          Nueva_Col = H - 1 - y
 *    Antihorario:  Nueva_Fila = W - 1 - x,  Nueva_Col = y
 * ============================================================================
 */

static uint8_t clampear_byte(int valor)
{
    if (valor < 0) return 0;
    if (valor > 255) return 255;
    return (uint8_t)valor;
}

/* ========================================================================= */
/*                              FILTROS BÁSICOS                              */
/* ========================================================================= */

ImagenBMP *filtro_negativo(const ImagenBMP *src)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            dst->matriz[y][x].b = (uint8_t)(255 - src->matriz[y][x].b);
            dst->matriz[y][x].g = (uint8_t)(255 - src->matriz[y][x].g);
            dst->matriz[y][x].r = (uint8_t)(255 - src->matriz[y][x].r);
        }
    }
    return dst;
}

ImagenBMP *filtro_escala_de_grises(const ImagenBMP *src)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            int suma = (int)src->matriz[y][x].r + (int)src->matriz[y][x].g + (int)src->matriz[y][x].b;
            uint8_t promedio = (uint8_t)(suma / 3);
            dst->matriz[y][x].b = promedio;
            dst->matriz[y][x].g = promedio;
            dst->matriz[y][x].r = promedio;
        }
    }
    return dst;
}

ImagenBMP *filtro_espejar_horizontal(const ImagenBMP *src)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            /* Invertir columnas: la columna 0 pasa a ser (ancho - 1) */
            dst->matriz[y][x] = src->matriz[y][src->ancho - 1 - x];
        }
    }
    return dst;
}

ImagenBMP *filtro_espejar_vertical(const ImagenBMP *src)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            /* Invertir filas: la fila superior 0 pasa a ser (alto - 1) */
            dst->matriz[y][x] = src->matriz[src->alto - 1 - y][x];
        }
    }
    return dst;
}

/* ========================================================================= */
/*                      FILTROS CON PARÁMETROS (0-100%)                      */
/* ========================================================================= */

ImagenBMP *filtro_aumentar_contraste(const ImagenBMP *src, int porcentaje)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    double factor = 1.0 + ((double)porcentaje / 100.0);

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            int b = (int)(128.0 + ((double)src->matriz[y][x].b - 128.0) * factor);
            int g = (int)(128.0 + ((double)src->matriz[y][x].g - 128.0) * factor);
            int r = (int)(128.0 + ((double)src->matriz[y][x].r - 128.0) * factor);
            dst->matriz[y][x].b = clampear_byte(b);
            dst->matriz[y][x].g = clampear_byte(g);
            dst->matriz[y][x].r = clampear_byte(r);
        }
    }
    return dst;
}

ImagenBMP *filtro_reducir_contraste(const ImagenBMP *src, int porcentaje)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    double factor = 1.0 - ((double)porcentaje / 100.0);

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            int b = (int)(128.0 + ((double)src->matriz[y][x].b - 128.0) * factor);
            int g = (int)(128.0 + ((double)src->matriz[y][x].g - 128.0) * factor);
            int r = (int)(128.0 + ((double)src->matriz[y][x].r - 128.0) * factor);
            dst->matriz[y][x].b = clampear_byte(b);
            dst->matriz[y][x].g = clampear_byte(g);
            dst->matriz[y][x].r = clampear_byte(r);
        }
    }
    return dst;
}

ImagenBMP *filtro_tonalidad_azul(const ImagenBMP *src, int porcentaje)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    double factor = 1.0 + ((double)porcentaje / 100.0);

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            int b = (int)((double)src->matriz[y][x].b * factor);
            dst->matriz[y][x].b = clampear_byte(b);
            dst->matriz[y][x].g = src->matriz[y][x].g;
            dst->matriz[y][x].r = src->matriz[y][x].r;
        }
    }
    return dst;
}

ImagenBMP *filtro_tonalidad_verde(const ImagenBMP *src, int porcentaje)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    double factor = 1.0 + ((double)porcentaje / 100.0);

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            int g = (int)((double)src->matriz[y][x].g * factor);
            dst->matriz[y][x].b = src->matriz[y][x].b;
            dst->matriz[y][x].g = clampear_byte(g);
            dst->matriz[y][x].r = src->matriz[y][x].r;
        }
    }
    return dst;
}

ImagenBMP *filtro_tonalidad_roja(const ImagenBMP *src, int porcentaje)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    double factor = 1.0 + ((double)porcentaje / 100.0);

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            int r = (int)((double)src->matriz[y][x].r * factor);
            dst->matriz[y][x].b = src->matriz[y][x].b;
            dst->matriz[y][x].g = src->matriz[y][x].g;
            dst->matriz[y][x].r = clampear_byte(r);
        }
    }
    return dst;
}

ImagenBMP *filtro_recortar(const ImagenBMP *src, int porcentaje)
{
    int nuevo_ancho = (src->ancho * porcentaje) / 100;
    int nuevo_alto = (src->alto * porcentaje) / 100;
    if (nuevo_ancho < 1) nuevo_ancho = 1;
    if (nuevo_alto < 1) nuevo_alto = 1;

    ImagenBMP *dst = crear_imagen(nuevo_ancho, nuevo_alto);
    if (!dst) return NULL;

    /*
     * Esquina inferior izquierda:
     * En nuestra matriz visual (fila 0 = arriba, fila alto-1 = abajo),
     * las filas inferiores van desde (alto - nuevo_alto) hasta (alto - 1).
     * Las columnas de la izquierda van desde 0 hasta (nuevo_ancho - 1).
     */
    int fila_inicio = src->alto - nuevo_alto;

    for (int y = 0; y < nuevo_alto; y++) {
        for (int x = 0; x < nuevo_ancho; x++) {
            dst->matriz[y][x] = src->matriz[fila_inicio + y][x];
        }
    }
    return dst;
}

ImagenBMP *filtro_achicar(const ImagenBMP *src, int porcentaje)
{
    int nuevo_ancho = (src->ancho * porcentaje) / 100;
    int nuevo_alto = (src->alto * porcentaje) / 100;
    if (nuevo_ancho < 1) nuevo_ancho = 1;
    if (nuevo_alto < 1) nuevo_alto = 1;

    ImagenBMP *dst = crear_imagen(nuevo_ancho, nuevo_alto);
    if (!dst) return NULL;

    /* Reescalado mediante muestreo por vecino más cercano (Nearest Neighbor) */
    for (int y = 0; y < nuevo_alto; y++) {
        int orig_y = (y * src->alto) / nuevo_alto;
        for (int x = 0; x < nuevo_ancho; x++) {
            int orig_x = (x * src->ancho) / nuevo_ancho;
            dst->matriz[y][x] = src->matriz[orig_y][orig_x];
        }
    }
    return dst;
}

/* ========================================================================= */
/*                                ROTACIONES                                 */
/* ========================================================================= */

ImagenBMP *filtro_rotar_derecha(const ImagenBMP *src)
{
    /* Al rotar 90°, el nuevo ancho es el alto original y viceversa */
    ImagenBMP *dst = crear_imagen(src->alto, src->ancho);
    if (!dst) return NULL;

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            dst->matriz[x][src->alto - 1 - y] = src->matriz[y][x];
        }
    }
    return dst;
}

ImagenBMP *filtro_rotar_izquierda(const ImagenBMP *src)
{
    ImagenBMP *dst = crear_imagen(src->alto, src->ancho);
    if (!dst) return NULL;

    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            dst->matriz[src->ancho - 1 - x][y] = src->matriz[y][x];
        }
    }
    return dst;
}

/* ========================================================================= */
/*                              CONCATENACIONES                              */
/* ========================================================================= */

ImagenBMP *filtro_concatenar_horizontal(const ImagenBMP *img1, const ImagenBMP *img2)
{
    int ancho_total = img1->ancho + img2->ancho;
    int alto_total = (img1->alto > img2->alto) ? img1->alto : img2->alto;

    ImagenBMP *dst = crear_imagen(ancho_total, alto_total);
    if (!dst) return NULL;

    /* Color de relleno teal/turquesa ({b:128, g:128, r:0}), ni blanco ni negro */
    Pixel relleno = {128, 128, 0};

    for (int y = 0; y < alto_total; y++) {
        /* Píxeles de la primera imagen (lado izquierdo) */
        for (int x = 0; x < img1->ancho; x++) {
            if (y < img1->alto) {
                dst->matriz[y][x] = img1->matriz[y][x];
            } else {
                dst->matriz[y][x] = relleno;
            }
        }
        /* Píxeles de la segunda imagen (lado derecho) */
        for (int x = 0; x < img2->ancho; x++) {
            if (y < img2->alto) {
                dst->matriz[y][img1->ancho + x] = img2->matriz[y][x];
            } else {
                dst->matriz[y][img1->ancho + x] = relleno;
            }
        }
    }
    return dst;
}

ImagenBMP *filtro_concatenar_vertical(const ImagenBMP *img1, const ImagenBMP *img2)
{
    int ancho_total = (img1->ancho > img2->ancho) ? img1->ancho : img2->ancho;
    int alto_total = img1->alto + img2->alto;

    ImagenBMP *dst = crear_imagen(ancho_total, alto_total);
    if (!dst) return NULL;

    Pixel relleno = {128, 128, 0};

    /* Primera imagen colocada arriba */
    for (int y = 0; y < img1->alto; y++) {
        for (int x = 0; x < ancho_total; x++) {
            if (x < img1->ancho) {
                dst->matriz[y][x] = img1->matriz[y][x];
            } else {
                dst->matriz[y][x] = relleno;
            }
        }
    }

    /* Segunda imagen colocada abajo */
    for (int y = 0; y < img2->alto; y++) {
        for (int x = 0; x < ancho_total; x++) {
            if (x < img2->ancho) {
                dst->matriz[img1->alto + y][x] = img2->matriz[y][x];
            } else {
                dst->matriz[img1->alto + y][x] = relleno;
            }
        }
    }
    return dst;
}

/* ========================================================================= */
/*                                  COMODÍN                                  */
/* ========================================================================= */

ImagenBMP *filtro_comodin(const ImagenBMP *src)
{
    ImagenBMP *dst = crear_imagen(src->ancho, src->alto);
    if (!dst) return NULL;

    /*
     * Efecto Sepia (Vintage):
     * Matriz de transformación lineal estándar W3C para tonos cálidos:
     *   R' = 0.393*R + 0.769*G + 0.189*B
     *   G' = 0.349*R + 0.686*G + 0.168*B
     *   B' = 0.272*R + 0.534*G + 0.131*B
     */
    for (int y = 0; y < src->alto; y++) {
        for (int x = 0; x < src->ancho; x++) {
            double r = (double)src->matriz[y][x].r;
            double g = (double)src->matriz[y][x].g;
            double b = (double)src->matriz[y][x].b;

            int tr = (int)(0.393 * r + 0.769 * g + 0.189 * b);
            int tg = (int)(0.349 * r + 0.686 * g + 0.168 * b);
            int tb = (int)(0.272 * r + 0.534 * g + 0.131 * b);

            dst->matriz[y][x].r = clampear_byte(tr);
            dst->matriz[y][x].g = clampear_byte(tg);
            dst->matriz[y][x].b = clampear_byte(tb);
        }
    }
    return dst;
}
