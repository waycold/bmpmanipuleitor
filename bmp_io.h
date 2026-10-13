#ifndef BMP_IO_H
#define BMP_IO_H

/**
 * ============================================================================
 * MÓDULO: BMP I/O (bmp_io.h)
 * ============================================================================
 * Responsabilidad:
 *   Estructuras binarias oficiales del formato BMP de Windows, cálculo del
 *   padding de alineación a 4 bytes, validaciones de cabeceras, inspección de
 *   metadatos y funciones de carga/guardado de imágenes en disco.
 *
 * Estructura de un archivo BMP de 24 bits sin compresión:
 *   +---------------------------------------+
 *   | EncabezadoArchivoBMP (14 bytes)       | -> Firma 'BM', FileSize, Offset
 *   +---------------------------------------+
 *   | EncabezadoInfoBMP (40 bytes)          | -> Width, Height, 24 bpp, Comp=0
 *   +---------------------------------------+
 *   | Datos de Píxeles (ancho*3 + pad)*alto | -> Píxeles BGR de abajo hacia arriba
 *   +---------------------------------------+
 * ============================================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "imagen.h"

/* ========================================================================= */
/*                          CÓDIGOS DE RETORNO Y CONSTANTES                  */
/* ========================================================================= */

#define RET_EXITO          0  /* Proceso ejecutado con éxito */
#define RET_ERR_ARGUMENTOS 1  /* Error en argumentos de línea de comandos */
#define RET_ERR_ARCHIVO    2  /* Archivo no encontrado o sin permisos */
#define RET_ERR_MEMORIA    3  /* Fallo de asignación dinámica (malloc falló) */
#define RET_ERR_BMP        4  /* Formato BMP inválido (firma, bpp o compresión) */

#define BMP_SIGNATURE      0x4D42  /* 'B' 'M' en formato little-endian (0x4D='M', 0x42='B') */
#define BMP_BPP_24         24      /* Profundidad obligatoria: 24 bits por píxel */
#define BMP_BI_RGB         0       /* Compresión 0: BI_RGB (sin compresión) */

/* ========================================================================= */
/*                   ENCABEZADOS BMP EMPAQUETADOS (#pragma pack)             */
/* ========================================================================= */

#pragma pack(push, 1)

/**
 * Encabezado de archivo BMP (BITMAPFILEHEADER): 14 bytes exactos.
 * Debe estar empaquetado (#pragma pack 1) porque los procesadores modernos
 * normalmente alinearían el campo `uint32_t tamano_archivo` a un múltiplo de 4,
 * agregando 2 bytes de relleno espurios después de `tipo` y corrompiendo la lectura.
 */
typedef struct {
    uint16_t tipo;             /* Bytes 0-1: Número mágico "BM" (0x4D42) */
    uint32_t tamano_archivo;   /* Bytes 2-5: Tamaño total del archivo en bytes */
    uint16_t reservado1;       /* Bytes 6-7: Reservado (debe ser 0) */
    uint16_t reservado2;       /* Bytes 8-9: Reservado (debe ser 0) */
    uint32_t offset_datos;     /* Bytes 10-13: Desplazamiento hasta el inicio de los píxeles (54) */
} EncabezadoArchivoBMP;

/**
 * Encabezado de información DIB (BITMAPINFOHEADER): 40 bytes exactos.
 * Describe las dimensiones, planos de color y método de compresión del bitmap.
 */
typedef struct {
    uint32_t tamano_cabecera;  /* Bytes 14-17: Tamaño de este encabezado DIB (40 bytes) */
    int32_t  ancho;            /* Bytes 18-21: Anchura de la imagen en píxeles */
    int32_t  alto;             /* Bytes 22-25: Altura en píxeles (positivo = bottom-up) */
    uint16_t planos;           /* Bytes 26-27: Planos de color (siempre debe ser 1) */
    uint16_t bpp;              /* Bytes 28-29: Profundidad de bits por píxel (debe ser 24) */
    uint32_t compresion;       /* Bytes 30-33: Tipo de compresión (debe ser 0 = BI_RGB) */
    uint32_t tamano_imagen;    /* Bytes 34-37: Tamaño del bloque de píxeles con padding */
    int32_t  res_horizontal;   /* Bytes 38-41: Resolución horizontal (píxeles por metro) */
    int32_t  res_vertical;     /* Bytes 42-45: Resolución vertical (píxeles por metro) */
    uint32_t tabla_color;      /* Bytes 46-49: Colores en la paleta (0 para 24 bpp) */
    uint32_t colores_import;   /* Bytes 50-53: Contador de colores importantes (0) */
} EncabezadoInfoBMP;

#pragma pack(pop)

/* ========================================================================= */
/*                          FUNCIONES DE UTILIDAD Y E/S                      */
/* ========================================================================= */

/**
 * Calcula los bytes de padding requeridos al final de cada fila para que
 * el ancho en bytes sea múltiplo de 4: (4 - ((ancho * 3) % 4)) % 4.
 */
int calcular_padding(int ancho);

/**
 * Extrae el nombre base de un archivo descartando cualquier ruta o carpeta previa.
 * Ejemplo: "img/foto.bmp" -> "foto.bmp" o "C:\imagenes\test.bmp" -> "test.bmp".
 */
const char *obtener_nombre_base(const char *ruta);

/**
 * Valida un archivo BMP sin procesarlo (para la opción --validar).
 * Retorna RET_EXITO (0) si es válido, RET_ERR_ARCHIVO (2) o RET_ERR_BMP (4).
 */
int validar_archivo_bmp(const char *ruta);

/**
 * Muestra información técnica detallada de las cabeceras (para --info).
 */
int mostrar_info_bmp(const char *ruta);

/**
 * Lee un archivo BMP desde el disco y carga su contenido en una estructura ImagenBMP.
 * Realiza todas las validaciones de firma, 24 bits, compresión y memoria.
 */
int cargar_bmp(const char *ruta, ImagenBMP **salida_img, bool verbose);

/**
 * Escribe una estructura ImagenBMP en disco generando las cabeceras estándar
 * de 54 bytes y calculando e insertando el padding correspondiente por fila.
 */
int guardar_bmp(const char *ruta, const ImagenBMP *img, bool verbose);

#endif /* BMP_IO_H */
