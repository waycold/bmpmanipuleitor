#include "bmp_io.h"
#include <stdlib.h>
#include <string.h>

/**
 * ============================================================================
 * IMPLEMENTACIÓN: ENTRADA / SALIDA Y MANEJO DEL FORMATO BMP (bmp_io.c)
 * ============================================================================
 * Detalle del Padding:
 *   En la arquitectura de Windows, los accesos a memoria de vídeo eran más
 *   eficientes cuando las líneas de píxeles comenzaban en direcciones alineadas
 *   a palabras de 32 bits (4 bytes).
 *   Si una imagen tiene ancho W:
 *     Bytes útiles por fila = W * 3 (ya que cada píxel tiene B, G, R).
 *     Padding = (4 - (bytes_útiles % 4)) % 4.
 *
 * Detalle del orden Bottom-Up:
 *   Los archivos BMP guardan primero la fila inferior de la imagen y por último
 *   la superior. Para que nuestra matriz 2D sea intuitiva (fila 0 = arriba),
 *   al leer iteramos desde `y = alto - 1` bajando hasta `0`. Al escribir en disco,
 *   escribimos en ese mismo orden invertido para respetar el estándar.
 * ============================================================================
 */

int calcular_padding(int ancho)
{
    return (4 - ((ancho * 3) % 4)) % 4;
}

const char *obtener_nombre_base(const char *ruta)
{
    const char *base = ruta;
    for (const char *p = ruta; *p != '\0'; p++) {
        if (*p == '/' || *p == '\\') {
            base = p + 1;
        }
    }
    return base;
}

int validar_archivo_bmp(const char *ruta)
{
    printf("Validando %s...\n", ruta);

    FILE *f = fopen(ruta, "rb");
    if (!f) {
        fprintf(stderr, "ERROR: No se pudo abrir el archivo '%s'\n", ruta);
        printf("ARCHIVO INVALIDO - No se puede procesar\n");
        return RET_ERR_ARCHIVO;
    }

    EncabezadoArchivoBMP file_hdr;
    EncabezadoInfoBMP info_hdr;

    /* 1. Validar lectura del encabezado de archivo (14 bytes) */
    if (fread(&file_hdr, sizeof(EncabezadoArchivoBMP), 1, f) != 1) {
        fclose(f);
        fprintf(stderr, "ERROR: Archivo truncado o cabecera incompleta\n");
        printf("ARCHIVO INVALIDO - No se puede procesar\n");
        return RET_ERR_BMP;
    }

    /* 2. Validar firma BM (0x4D42) */
    if (file_hdr.tipo != BMP_SIGNATURE) {
        fclose(f);
        fprintf(stderr, "ERROR: Signature BMP invalido (esperado BM)\n");
        printf("ARCHIVO INVALIDO - No se puede procesar\n");
        return RET_ERR_BMP;
    }

    printf("Signature BMP valido\n");

    /* 3. Validar lectura del encabezado DIB (40 bytes) */
    if (fread(&info_hdr, sizeof(EncabezadoInfoBMP), 1, f) != 1) {
        fclose(f);
        fprintf(stderr, "ERROR: Cabecera DIB incompleta\n");
        printf("ARCHIVO INVALIDO - No se puede procesar\n");
        return RET_ERR_BMP;
    }

    fclose(f);

    /* 4. Validar profundidad de color (solo 24 bits) */
    if (info_hdr.bpp != BMP_BPP_24) {
        printf("ERROR: Profundidad de color incorrecta (%u bits, esperado 24 bits)\n", (unsigned int)info_hdr.bpp);
        printf("ARCHIVO INVALIDO - No se puede procesar\n");
        return RET_ERR_BMP;
    }

    printf("Profundidad de 24 bits confirmada\n");

    /* 5. Validar compresión (solo 0 = BI_RGB no comprimido) */
    if (info_hdr.compresion != BMP_BI_RGB) {
        printf("ERROR: Compresion no soportada (esperado 0 - no comprimido)\n");
        printf("ARCHIVO INVALIDO - No se puede procesar\n");
        return RET_ERR_BMP;
    }

    printf("Compresion: No comprimido\n");
    printf("ARCHIVO VALIDO - Listo para procesar\n");
    return RET_EXITO;
}

int mostrar_info_bmp(const char *ruta)
{
    FILE *f = fopen(ruta, "rb");
    if (!f) {
        fprintf(stderr, "ERROR: No se pudo abrir el archivo '%s'\n", ruta);
        return RET_ERR_ARCHIVO;
    }

    EncabezadoArchivoBMP file_hdr;
    EncabezadoInfoBMP info_hdr;

    if (fread(&file_hdr, sizeof(EncabezadoArchivoBMP), 1, f) != 1 ||
        fread(&info_hdr, sizeof(EncabezadoInfoBMP), 1, f) != 1) {
        fclose(f);
        fprintf(stderr, "ERROR: No se pudieron leer las cabeceras BMP de '%s'\n", ruta);
        return RET_ERR_BMP;
    }
    fclose(f);

    if (file_hdr.tipo != BMP_SIGNATURE) {
        fprintf(stderr, "ERROR: Signature BMP invalido en '%s'\n", ruta);
        return RET_ERR_BMP;
    }

    int ancho = (info_hdr.ancho > 0) ? info_hdr.ancho : -info_hdr.ancho;
    int alto = (info_hdr.alto > 0) ? info_hdr.alto : -info_hdr.alto;
    int padding = calcular_padding(ancho);

    uint32_t tam_imagen = info_hdr.tamano_imagen;
    if (tam_imagen == 0) {
        tam_imagen = (uint32_t)((ancho * 3 + padding) * alto);
    }

    /* Salida formateada idéntica al ejemplo de la consigna */
    printf("Archivo: %s\n", obtener_nombre_base(ruta));
    printf("Tamaño del archivo: %u bytes\n", (unsigned int)file_hdr.tamano_archivo);
    printf("Dimensiones: %dx%d pixeles\n", ancho, alto);
    printf("Profundidad de color: %u bits\n", (unsigned int)info_hdr.bpp);
    printf("Compresión: %s\n", (info_hdr.compresion == 0) ? "No comprimido" : "Comprimido");
    printf("Offset de datos: %u bytes\n", (unsigned int)file_hdr.offset_datos);
    printf("Tamaño de imagen: %u bytes\n", (unsigned int)tam_imagen);
    printf("Padding por fila: %d bytes\n", padding);

    return RET_EXITO;
}

int cargar_bmp(const char *ruta, ImagenBMP **salida_img, bool verbose)
{
    if (verbose) {
        printf("[INFO] Cargando archivo: %s\n", ruta);
        printf("[INFO] Validando header BMP...\n");
    }

    FILE *f = fopen(ruta, "rb");
    if (!f) {
        fprintf(stderr, "ERROR: No se pudo abrir el archivo '%s'\n", ruta);
        return RET_ERR_ARCHIVO;
    }

    EncabezadoArchivoBMP file_hdr;
    EncabezadoInfoBMP info_hdr;

    if (fread(&file_hdr, sizeof(EncabezadoArchivoBMP), 1, f) != 1 ||
        fread(&info_hdr, sizeof(EncabezadoInfoBMP), 1, f) != 1) {
        fclose(f);
        fprintf(stderr, "ERROR: Formato de archivo BMP corrupto o truncado '%s'\n", ruta);
        return RET_ERR_BMP;
    }

    if (file_hdr.tipo != BMP_SIGNATURE) {
        fclose(f);
        fprintf(stderr, "ERROR: Signature BMP invalido (esperado 'BM')\n");
        return RET_ERR_BMP;
    }

    if (info_hdr.bpp != BMP_BPP_24) {
        fclose(f);
        fprintf(stderr, "ERROR: Profundidad de color incorrecta (%u bits, esperado 24 bits)\n", (unsigned int)info_hdr.bpp);
        return RET_ERR_BMP;
    }

    if (info_hdr.compresion != BMP_BI_RGB) {
        fclose(f);
        fprintf(stderr, "ERROR: Archivo comprimido no soportado\n");
        return RET_ERR_BMP;
    }

    int ancho = info_hdr.ancho;
    int alto = (info_hdr.alto < 0) ? -info_hdr.alto : info_hdr.alto;

    if (ancho < 1 || alto < 1) {
        fclose(f);
        fprintf(stderr, "ERROR: Dimensiones de imagen invalidas (%dx%d)\n", ancho, alto);
        return RET_ERR_BMP;
    }

    if (verbose) {
        printf("[INFO] Archivo válido - Dimensiones: %dx%d, Tamaño: %u bytes\n",
               ancho, alto, (unsigned int)file_hdr.tamano_archivo);
        printf("[INFO] Reservando memoria para matriz %dx%d...\n", ancho, alto);
    }

    /* Asignación de la matriz 2D dinámica */
    ImagenBMP *img = crear_imagen(ancho, alto);
    if (!img) {
        fclose(f);
        fprintf(stderr, "ERROR: Falló malloc al reservar memoria para matriz %dx%d\n", ancho, alto);
        return RET_ERR_MEMORIA;
    }

    img->res_horizontal = info_hdr.res_horizontal;
    img->res_vertical = info_hdr.res_vertical;

    if (verbose) {
        printf("[INFO] Memoria reservada exitosamente (%d píxeles)\n", ancho * alto);
        printf("[INFO] Leyendo datos de imagen...\n");
    }

    /* Saltar directamente al offset de datos de imagen */
    if (fseek(f, file_hdr.offset_datos, SEEK_SET) != 0) {
        liberar_imagen(img);
        fclose(f);
        fprintf(stderr, "ERROR: No se pudo acceder al offset de píxeles\n");
        return RET_ERR_BMP;
    }

    int padding = calcular_padding(ancho);

    /*
     * Lectura de píxeles (Bottom-Up):
     * La primera fila en el archivo corresponde a la fila inferior visual (y = alto - 1).
     * Luego de leer cada fila, se saltean los bytes de padding si los hubiera.
     */
    for (int y = alto - 1; y >= 0; y--) {
        if (fread(img->matriz[y], sizeof(Pixel), ancho, f) != (size_t)ancho) {
            liberar_imagen(img);
            fclose(f);
            fprintf(stderr, "ERROR: Fin prematuro del archivo leyendo datos de píxeles\n");
            return RET_ERR_BMP;
        }
        if (padding > 0) {
            if (fseek(f, padding, SEEK_CUR) != 0) {
                liberar_imagen(img);
                fclose(f);
                fprintf(stderr, "ERROR: Error saltando padding\n");
                return RET_ERR_BMP;
            }
        }
    }

    fclose(f);

    if (verbose) {
        printf("[INFO] Datos cargados correctamente\n");
    }

    *salida_img = img;
    return RET_EXITO;
}

int guardar_bmp(const char *ruta, const ImagenBMP *img, bool verbose)
{
    if (verbose) {
        printf("[INFO] Guardando resultado: %s\n", ruta);
    }

    FILE *f = fopen(ruta, "wb");
    if (!f) {
        fprintf(stderr, "ERROR: No se pudo abrir el archivo para escritura '%s'\n", ruta);
        return RET_ERR_ARCHIVO;
    }

    int padding = calcular_padding(img->ancho);
    uint32_t tam_fila = (uint32_t)(img->ancho * 3 + padding);
    uint32_t tam_imagen = tam_fila * (uint32_t)img->alto;
    uint32_t tam_archivo = (uint32_t)(sizeof(EncabezadoArchivoBMP) + sizeof(EncabezadoInfoBMP)) + tam_imagen;

    /* Armado de la cabecera de archivo (14 bytes) */
    EncabezadoArchivoBMP file_hdr;
    file_hdr.tipo = BMP_SIGNATURE;
    file_hdr.tamano_archivo = tam_archivo;
    file_hdr.reservado1 = 0;
    file_hdr.reservado2 = 0;
    file_hdr.offset_datos = sizeof(EncabezadoArchivoBMP) + sizeof(EncabezadoInfoBMP); /* 54 bytes */

    /* Armado de la cabecera de información DIB (40 bytes) */
    EncabezadoInfoBMP info_hdr;
    info_hdr.tamano_cabecera = sizeof(EncabezadoInfoBMP);
    info_hdr.ancho = img->ancho;
    info_hdr.alto = img->alto;
    info_hdr.planos = 1;
    info_hdr.bpp = BMP_BPP_24;
    info_hdr.compresion = BMP_BI_RGB;
    info_hdr.tamano_imagen = tam_imagen;
    info_hdr.res_horizontal = (img->res_horizontal > 0) ? img->res_horizontal : 2835;
    info_hdr.res_vertical = (img->res_vertical > 0) ? img->res_vertical : 2835;
    info_hdr.tabla_color = 0;
    info_hdr.colores_import = 0;

    /* Escritura de ambas cabeceras contiguas (54 bytes) */
    if (fwrite(&file_hdr, sizeof(EncabezadoArchivoBMP), 1, f) != 1 ||
        fwrite(&info_hdr, sizeof(EncabezadoInfoBMP), 1, f) != 1) {
        fclose(f);
        fprintf(stderr, "ERROR: Error escribiendo cabeceras BMP en '%s'\n", ruta);
        return RET_ERR_ARCHIVO;
    }

    uint8_t bytes_padding[3] = {0, 0, 0};

    /*
     * Escritura de filas (Bottom-Up):
     * Se escribe desde la fila visual inferior (y = alto - 1) hacia la superior (y = 0).
     * Al final de cada fila, se escriben los bytes de padding en cero.
     */
    for (int y = img->alto - 1; y >= 0; y--) {
        if (fwrite(img->matriz[y], sizeof(Pixel), img->ancho, f) != (size_t)img->ancho) {
            fclose(f);
            fprintf(stderr, "ERROR: Error escribiendo píxeles en '%s'\n", ruta);
            return RET_ERR_ARCHIVO;
        }
        if (padding > 0) {
            if (fwrite(bytes_padding, 1, padding, f) != (size_t)padding) {
                fclose(f);
                fprintf(stderr, "ERROR: Error escribiendo padding en '%s'\n", ruta);
                return RET_ERR_ARCHIVO;
            }
        }
    }

    fclose(f);
    return RET_EXITO;
}
