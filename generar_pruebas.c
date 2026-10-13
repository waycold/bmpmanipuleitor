#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * ============================================================================
 * UTILIDAD DE TESTING: GENERADOR DE CASOS DE PRUEBA (generar_pruebas.c)
 * ============================================================================
 * Genera automáticamente en la carpeta `img/` los casos de prueba recomendados
 * en la página 5 de la consigna oficial:
 *
 *   1. Imagen cuadrada (100x100) -> padding = 0 bytes
 *   2. Imagen rectangular (150x75) -> padding = 2 bytes
 *   3. Imagen con padding no múltiplo (239x80) -> padding = 3 bytes
 *   4. Imagen mínima (1x1) -> padding = 1 byte
 *   5. Archivo con firma corrupta (no 'BM') -> prueba de código de error 4
 *   6. Archivo con profundidad corrupta (8 bpp) -> prueba de código de error 4
 * ============================================================================
 */

#pragma pack(push, 1)
typedef struct {
    uint16_t tipo;
    uint32_t tamano_archivo;
    uint16_t reservado1;
    uint16_t reservado2;
    uint32_t offset_datos;
} EncabezadoArchivoBMP;

typedef struct {
    uint32_t tamano_cabecera;
    int32_t  ancho;
    int32_t  alto;
    uint16_t planos;
    uint16_t bpp;
    uint32_t compresion;
    uint32_t tamano_imagen;
    int32_t  res_horizontal;
    int32_t  res_vertical;
    uint32_t tabla_color;
    uint32_t colores_import;
} EncabezadoInfoBMP;

typedef struct {
    uint8_t b, g, r;
} Pixel;
#pragma pack(pop)

void crear_bmp_test(const char *ruta, int ancho, int alto, int patron)
{
    int padding = (4 - ((ancho * 3) % 4)) % 4;
    uint32_t tam_fila = (uint32_t)(ancho * 3 + padding);
    uint32_t tam_imagen = tam_fila * (uint32_t)alto;
    uint32_t tam_archivo = 54 + tam_imagen;

    EncabezadoArchivoBMP fh = {0x4D42, tam_archivo, 0, 0, 54};
    EncabezadoInfoBMP ih = {40, ancho, alto, 1, 24, 0, tam_imagen, 2835, 2835, 0, 0};

    FILE *f = fopen(ruta, "wb");
    if (!f) {
        printf("Error creando %s\n", ruta);
        return;
    }
    fwrite(&fh, sizeof(fh), 1, f);
    fwrite(&ih, sizeof(ih), 1, f);

    uint8_t pad[3] = {0, 0, 0};
    for (int y = alto - 1; y >= 0; y--) {
        for (int x = 0; x < ancho; x++) {
            Pixel p;
            if (patron == 1) { /* Bandas de colores primarios */
                if (x < ancho / 3) {
                    p.r = 255; p.g = 0; p.b = 0; /* Rojo */
                } else if (x < (2 * ancho) / 3) {
                    p.r = 0; p.g = 255; p.b = 0; /* Verde */
                } else {
                    p.r = 0; p.g = 0; p.b = 255; /* Azul */
                }
            } else if (patron == 2) { /* Degradado suave */
                p.r = (uint8_t)((x * 255) / (ancho > 1 ? (ancho - 1) : 1));
                p.g = (uint8_t)((y * 255) / (alto > 1 ? (alto - 1) : 1));
                p.b = 128;
            } else { /* Tablero de ajedrez con padding */
                int c = ((x / 10) + (y / 10)) % 2;
                p.r = c ? 220 : 40;
                p.g = c ? 220 : 40;
                p.b = c ? 220 : 40;
            }
            fwrite(&p, sizeof(Pixel), 1, f);
        }
        if (padding > 0) {
            fwrite(pad, 1, padding, f);
        }
    }
    fclose(f);
    printf("Creado: %s (%dx%d, padding=%d bytes)\n", ruta, ancho, alto, padding);
}

int main(void)
{
    printf("====================================================\n");
    printf("Generando imagenes de prueba en carpeta img/...\n");
    printf("====================================================\n\n");

    /* 1. Imagen cuadrada 100x100 (padding = 0) */
    crear_bmp_test("img/cuadrada_100x100.bmp", 100, 100, 1);

    /* 2. Imagen rectangular 150x75 (padding = 2) */
    crear_bmp_test("img/rectangular_150x75.bmp", 150, 75, 2);

    /* 3. Imagen con padding no multiplo 239x80 (padding = 3) */
    crear_bmp_test("img/con_padding_239x80.bmp", 239, 80, 3);

    /* 4. Imagen minima 1x1 (padding = 1) */
    crear_bmp_test("img/minima_1x1.bmp", 1, 1, 1);

    /* 5. Archivo corrupto (firma no 'BM') */
    FILE *f_bad = fopen("img/corrupta_firma.bmp", "wb");
    if (f_bad) {
        char basura[60] = "NO_ES_UN_BMP_VALIDO_HEADER_CORRUPTO";
        fwrite(basura, 1, sizeof(basura), f_bad);
        fclose(f_bad);
        printf("Creado: img/corrupta_firma.bmp (Firma corrupta)\n");
    }

    /* 6. Archivo corrupto (8 bits por pixel) */
    EncabezadoArchivoBMP fh8 = {0x4D42, 54 + 100, 0, 0, 54};
    EncabezadoInfoBMP ih8 = {40, 10, 10, 1, 8, 0, 100, 2835, 2835, 0, 0};
    FILE *f8 = fopen("img/corrupta_8bits.bmp", "wb");
    if (f8) {
        fwrite(&fh8, sizeof(fh8), 1, f8);
        fwrite(&ih8, sizeof(ih8), 1, f8);
        char ceros[100] = {0};
        fwrite(ceros, 1, 100, f8);
        fclose(f8);
        printf("Creado: img/corrupta_8bits.bmp (Profundidad 8 bits en vez de 24)\n");
    }

    printf("\nTodas las imagenes de prueba fueron generadas correctamente en img/.\n");
    return 0;
}
