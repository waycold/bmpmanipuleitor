#include "parser.h"
#include "funciones_grupo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * ============================================================================
 * IMPLEMENTACIÓN: PARSEO Y ORQUESTADOR CLI (parser.c)
 * ============================================================================
 * Responsabilidad:
 *   Interpreta la línea de comandos de forma agnóstica a la posición de
 *   banderas y nombres de archivo. Implementa los requisitos clave:
 *     1. Deduplicación: Un filtro repetido solo se ejecuta una vez.
 *     2. Tolerancia a errores: Argumentos inválidos se reportan con mensaje de
 *        error pero se continúa procesando otros filtros válidos.
 *     3. Nombres de salida: GRUPO_filtro[-valor]_archivo.bmp.
 *     4. Códigos de retorno: Retorna 0, 1, 2, 3 o 4 según el resultado.
 * ============================================================================
 */

typedef enum {
    FILTRO_NINGUNO = 0,
    FILTRO_NEGATIVO,
    FILTRO_ESCALA_GRISES,
    FILTRO_ESPEJAR_H,
    FILTRO_ESPEJAR_V,
    FILTRO_AUMENTAR_CONTRASTE,
    FILTRO_REDUCIR_CONTRASTE,
    FILTRO_TONALIDAD_AZUL,
    FILTRO_TONALIDAD_VERDE,
    FILTRO_TONALIDAD_ROJA,
    FILTRO_RECORTAR,
    FILTRO_ACHICAR,
    FILTRO_ROTAR_DERECHA,
    FILTRO_ROTAR_IZQUIERDA,
    FILTRO_CONCATENAR_H,
    FILTRO_CONCATENAR_V,
    FILTRO_COMODIN
} TipoFiltro;

typedef struct {
    TipoFiltro tipo;
    int param;                     /* Porcentaje (0-100 o 1-100) */
    char nombre_identificador[64]; /* Ej: "negativo", "aumentar-contraste-25" */
} TareaFiltro;

#define MAX_TAREAS 64
#define MAX_ARCHIVOS 16

void mostrar_ayuda(void)
{
    printf("BMPMANIPULEITOR - Manipulador de imágenes BMP 24 bits\n\n");
    printf("GRUPO:\n");
    printf("Integrantes:\n");
    printf("  1. DNI - APELLIDO, Nombre\n");
    printf("  2. DNI - APELLIDO, Nombre\n");
    printf("  3. DNI - APELLIDO, Nombre\n\n");
    printf("Uso: bmpmanipuleitor.exe [OPCIONES]\n\n");
    printf("EJEMPLOS:\n");
    printf("  bmpmanipuleitor.exe --negativo foto.bmp\n");
    printf("  bmpmanipuleitor.exe --info imagen.bmp --validar\n");
    printf("  bmpmanipuleitor.exe foto.bmp --verbose --escala-de-grises --aumentar-contraste=25\n\n");
    printf("FILTROS\n");
    printf("  --comodin Aplica efecto sepia (vintage) a la imagen recalculando tonos cálidos.\n");
}

int ejecutar_procesamiento(int argc, char *argv[])
{
    if (argc < 2) {
        mostrar_ayuda();
        return RET_ERR_ARGUMENTOS;
    }

    bool opt_help = false;
    bool opt_verbose = false;
    bool opt_info = false;
    bool opt_validar = false;

    char *archivos[MAX_ARCHIVOS];
    int cant_archivos = 0;

    TareaFiltro tareas[MAX_TAREAS];
    int cant_tareas = 0;

    /*
     * 1. RECORRIDO Y CLASIFICACIÓN DE ARGUMENTOS
     * Se procesa argv permitiendo que los archivos aparezcan antes, después
     * o intercalados entre las banderas.
     */
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (strcmp(arg, "--help") == 0) {
            opt_help = true;
        } else if (strcmp(arg, "--verbose") == 0) {
            opt_verbose = true;
        } else if (strcmp(arg, "--info") == 0) {
            opt_info = true;
        } else if (strcmp(arg, "--validar") == 0) {
            opt_validar = true;
        } else if (strcmp(arg, "--negativo") == 0 ||
                   strcmp(arg, "--escala-de-grises") == 0 ||
                   strcmp(arg, "--espejar-horizontal") == 0 ||
                   strcmp(arg, "--espejar-vertical") == 0 ||
                   strcmp(arg, "--rotar-derecha") == 0 ||
                   strcmp(arg, "--rotar-izquierda") == 0 ||
                   strcmp(arg, "--concatenar-horizontal") == 0 ||
                   strcmp(arg, "--concatenar-vertical") == 0 ||
                   strcmp(arg, "--comodin") == 0) {

            TipoFiltro tipo = FILTRO_NINGUNO;
            const char *nombre = arg + 2;

            if (strcmp(arg, "--negativo") == 0) tipo = FILTRO_NEGATIVO;
            else if (strcmp(arg, "--escala-de-grises") == 0) tipo = FILTRO_ESCALA_GRISES;
            else if (strcmp(arg, "--espejar-horizontal") == 0) tipo = FILTRO_ESPEJAR_H;
            else if (strcmp(arg, "--espejar-vertical") == 0) tipo = FILTRO_ESPEJAR_V;
            else if (strcmp(arg, "--rotar-derecha") == 0) tipo = FILTRO_ROTAR_DERECHA;
            else if (strcmp(arg, "--rotar-izquierda") == 0) tipo = FILTRO_ROTAR_IZQUIERDA;
            else if (strcmp(arg, "--concatenar-horizontal") == 0) tipo = FILTRO_CONCATENAR_H;
            else if (strcmp(arg, "--concatenar-vertical") == 0) tipo = FILTRO_CONCATENAR_V;
            else if (strcmp(arg, "--comodin") == 0) tipo = FILTRO_COMODIN;

            /* Deduplicación: Si ya fue agregado previamente, se ignora */
            bool ya_existe = false;
            for (int t = 0; t < cant_tareas; t++) {
                if (tareas[t].tipo == tipo) {
                    ya_existe = true;
                    break;
                }
            }

            if (!ya_existe && cant_tareas < MAX_TAREAS) {
                tareas[cant_tareas].tipo = tipo;
                tareas[cant_tareas].param = 0;
                strncpy(tareas[cant_tareas].nombre_identificador, nombre, 63);
                tareas[cant_tareas].nombre_identificador[63] = '\0';
                cant_tareas++;
            }
        } else if (strncmp(arg, "--aumentar-contraste=", 21) == 0 ||
                   strncmp(arg, "--reducir-contraste=", 20) == 0 ||
                   strncmp(arg, "--tonalidad-azul=", 17) == 0 ||
                   strncmp(arg, "--tonalidad-verde=", 18) == 0 ||
                   strncmp(arg, "--tonalidad-roja=", 17) == 0 ||
                   strncmp(arg, "--recortar=", 11) == 0 ||
                   strncmp(arg, "--achicar=", 10) == 0) {

            char *igual = strchr(arg, '=');
            int val = -1;
            if (igual && *(igual + 1) != '\0') {
                val = atoi(igual + 1);
            }

            TipoFiltro tipo = FILTRO_NINGUNO;
            char prefijo[32] = {0};
            bool es_crop_o_resize = false;

            if (strncmp(arg, "--aumentar-contraste=", 21) == 0) {
                tipo = FILTRO_AUMENTAR_CONTRASTE;
                strcpy(prefijo, "aumentar-contraste");
            } else if (strncmp(arg, "--reducir-contraste=", 20) == 0) {
                tipo = FILTRO_REDUCIR_CONTRASTE;
                strcpy(prefijo, "reducir-contraste");
            } else if (strncmp(arg, "--tonalidad-azul=", 17) == 0) {
                tipo = FILTRO_TONALIDAD_AZUL;
                strcpy(prefijo, "tonalidad-azul");
            } else if (strncmp(arg, "--tonalidad-verde=", 18) == 0) {
                tipo = FILTRO_TONALIDAD_VERDE;
                strcpy(prefijo, "tonalidad-verde");
            } else if (strncmp(arg, "--tonalidad-roja=", 17) == 0) {
                tipo = FILTRO_TONALIDAD_ROJA;
                strcpy(prefijo, "tonalidad-roja");
            } else if (strncmp(arg, "--recortar=", 11) == 0) {
                tipo = FILTRO_RECORTAR;
                strcpy(prefijo, "recortar");
                es_crop_o_resize = true;
            } else if (strncmp(arg, "--achicar=", 10) == 0) {
                tipo = FILTRO_ACHICAR;
                strcpy(prefijo, "achicar");
                es_crop_o_resize = true;
            }

            /* Validación de rangos: 0-100 para color/contraste, 1-100 para recorte/escala */
            int min_val = es_crop_o_resize ? 1 : 0;
            if (val < min_val || val > 100) {
                fprintf(stderr, "ERROR: Parametro fuera de rango para '%s' (esperado %d-100)\n", arg, min_val);
            } else {
                /* Deduplicación de filtro paramétrico exacto */
                bool ya_existe = false;
                for (int t = 0; t < cant_tareas; t++) {
                    if (tareas[t].tipo == tipo && tareas[t].param == val) {
                        ya_existe = true;
                        break;
                    }
                }

                if (!ya_existe && cant_tareas < MAX_TAREAS) {
                    tareas[cant_tareas].tipo = tipo;
                    tareas[cant_tareas].param = val;
                    snprintf(tareas[cant_tareas].nombre_identificador, 64, "%s-%d", prefijo, val);
                    cant_tareas++;
                }
            }
        } else if (strncmp(arg, "--", 2) == 0) {
            /* Argumento desconocido: se reporta el error y se continúa procesando */
            fprintf(stderr, "ERROR: Argumento invalido o desconocido: %s\n", arg);
        } else {
            /* Es una ruta de archivo de imagen */
            if (cant_archivos < MAX_ARCHIVOS) {
                archivos[cant_archivos++] = arg;
            }
        }
    }

    if (opt_help) {
        mostrar_ayuda();
        return RET_EXITO;
    }

    if (opt_verbose) {
        printf("[INFO] Iniciando bmpmanipuleitor...\n");
        printf("[INFO] Argumentos detectados:");
        for (int i = 1; i < argc; i++) {
            printf(" %s", argv[i]);
        }
        printf("\n");
    }

    /* Validar presencia de imágenes cuando no es --help */
    if (cant_archivos == 0) {
        fprintf(stderr, "ERROR: No se especificó ningún archivo BMP.\n");
        return RET_ERR_ARGUMENTOS;
    }

    /* 2. EJECUCIÓN DE DIAGNÓSTICO: --validar */
    int ret_val = RET_EXITO;
    if (opt_validar) {
        for (int i = 0; i < cant_archivos; i++) {
            int r = validar_archivo_bmp(archivos[i]);
            if (r != RET_EXITO) {
                ret_val = r;
            }
        }
        if (cant_tareas == 0 && !opt_info) {
            return ret_val;
        }
    }

    /* 3. EJECUCIÓN DE DIAGNÓSTICO: --info */
    int ret_info = RET_EXITO;
    if (opt_info) {
        for (int i = 0; i < cant_archivos; i++) {
            int r = mostrar_info_bmp(archivos[i]);
            if (r != RET_EXITO) {
                ret_info = r;
            }
        }
        if (cant_tareas == 0) {
            /* Si --validar falló, priorizar su código de error */
            return (ret_val != RET_EXITO) ? ret_val : ret_info;
        }
    }

    /* Si solo se especificaron archivos sin filtros ni diagnóstico */
    if (cant_tareas == 0) {
        fprintf(stderr, "ERROR: No se especificó ningún filtro ni acción a realizar.\n");
        return RET_ERR_ARGUMENTOS;
    }

    /* 4. VALIDACIÓN DE CANTIDAD DE IMÁGENES POR TIPO DE FILTRO */
    bool tiene_concatenacion = false;
    bool tiene_filtro_regular = false;
    for (int t = 0; t < cant_tareas; t++) {
        if (tareas[t].tipo == FILTRO_CONCATENAR_H || tareas[t].tipo == FILTRO_CONCATENAR_V) {
            tiene_concatenacion = true;
        } else {
            tiene_filtro_regular = true;
        }
    }

    if (tiene_concatenacion && cant_archivos != 2) {
        fprintf(stderr, "ERROR: Los filtros de concatenación requieren exactamente dos archivos de imagen.\n");
        return RET_ERR_ARGUMENTOS;
    }

    if (tiene_filtro_regular && cant_archivos != 1) {
        fprintf(stderr, "ERROR: Los filtros regulares reciben únicamente una imagen.\n");
        return RET_ERR_ARGUMENTOS;
    }

    /* 5. APLICACIÓN DE CONCATENACIONES (2 imágenes) */
    if (tiene_concatenacion) {
        ImagenBMP *img1 = NULL;
        ImagenBMP *img2 = NULL;

        int r1 = cargar_bmp(archivos[0], &img1, opt_verbose);
        if (r1 != RET_EXITO) return r1;

        int r2 = cargar_bmp(archivos[1], &img2, opt_verbose);
        if (r2 != RET_EXITO) {
            liberar_imagen(img1);
            return r2;
        }

        const char *base1 = obtener_nombre_base(archivos[0]);
        int archivos_generados = 0;

        for (int t = 0; t < cant_tareas; t++) {
            if (tareas[t].tipo != FILTRO_CONCATENAR_H && tareas[t].tipo != FILTRO_CONCATENAR_V) {
                continue;
            }

            if (opt_verbose) {
                printf("[INFO] Aplicando filtro: %s\n", tareas[t].nombre_identificador);
            }

            ImagenBMP *res = (tareas[t].tipo == FILTRO_CONCATENAR_H)
                ? filtro_concatenar_horizontal(img1, img2)
                : filtro_concatenar_vertical(img1, img2);

            if (!res) {
                liberar_imagen(img1);
                liberar_imagen(img2);
                fprintf(stderr, "ERROR: Falló malloc aplicando %s\n", tareas[t].nombre_identificador);
                return RET_ERR_MEMORIA;
            }

            char ruta_salida[256];
            snprintf(ruta_salida, sizeof(ruta_salida), "%s_%s_%s",
                     NOMBRE_GRUPO, tareas[t].nombre_identificador, base1);

            int r_guardar = guardar_bmp(ruta_salida, res, opt_verbose);
            liberar_imagen(res);

            if (r_guardar == RET_EXITO) {
                archivos_generados++;
                if (opt_verbose) {
                    printf("[INFO] Filtro %s completado exitosamente\n", tareas[t].nombre_identificador);
                }
            } else {
                fprintf(stderr, "ERROR: No se pudo guardar '%s'. Continuando con otros filtros.\n", ruta_salida);
            }
        }

        if (opt_verbose) {
            printf("[INFO] Liberando memoria...\n");
        }
        liberar_imagen(img1);
        liberar_imagen(img2);

        if (opt_verbose) {
            printf("[INFO] Proceso finalizado - %d archivos generados\n", archivos_generados);
        }
        return RET_EXITO;
    }

    /* 6. APLICACIÓN DE FILTROS REGULARES (1 imagen) */
    ImagenBMP *img = NULL;
    int r = cargar_bmp(archivos[0], &img, opt_verbose);
    if (r != RET_EXITO) {
        return r;
    }

    const char *base = obtener_nombre_base(archivos[0]);
    int archivos_generados = 0;

    for (int t = 0; t < cant_tareas; t++) {
        if (opt_verbose) {
            printf("[INFO] Aplicando filtro: %s\n", tareas[t].nombre_identificador);
        }

        ImagenBMP *res = NULL;
        switch (tareas[t].tipo) {
            case FILTRO_NEGATIVO:
                res = filtro_negativo(img);
                break;
            case FILTRO_ESCALA_GRISES:
                res = filtro_escala_de_grises(img);
                break;
            case FILTRO_ESPEJAR_H:
                res = filtro_espejar_horizontal(img);
                break;
            case FILTRO_ESPEJAR_V:
                res = filtro_espejar_vertical(img);
                break;
            case FILTRO_AUMENTAR_CONTRASTE:
                res = filtro_aumentar_contraste(img, tareas[t].param);
                break;
            case FILTRO_REDUCIR_CONTRASTE:
                res = filtro_reducir_contraste(img, tareas[t].param);
                break;
            case FILTRO_TONALIDAD_AZUL:
                res = filtro_tonalidad_azul(img, tareas[t].param);
                break;
            case FILTRO_TONALIDAD_VERDE:
                res = filtro_tonalidad_verde(img, tareas[t].param);
                break;
            case FILTRO_TONALIDAD_ROJA:
                res = filtro_tonalidad_roja(img, tareas[t].param);
                break;
            case FILTRO_RECORTAR:
                res = filtro_recortar(img, tareas[t].param);
                break;
            case FILTRO_ACHICAR:
                res = filtro_achicar(img, tareas[t].param);
                break;
            case FILTRO_ROTAR_DERECHA:
                res = filtro_rotar_derecha(img);
                break;
            case FILTRO_ROTAR_IZQUIERDA:
                res = filtro_rotar_izquierda(img);
                break;
            case FILTRO_COMODIN:
                res = filtro_comodin(img);
                break;
            default:
                break;
        }

        if (!res) {
            liberar_imagen(img);
            fprintf(stderr, "ERROR: Falló malloc aplicando %s\n", tareas[t].nombre_identificador);
            return RET_ERR_MEMORIA;
        }

        /* Formato de salida: GRUPO_filtro[-valor]_archivo.bmp */
        char ruta_salida[256];
        snprintf(ruta_salida, sizeof(ruta_salida), "%s_%s_%s",
                 NOMBRE_GRUPO, tareas[t].nombre_identificador, base);

        int r_guardar = guardar_bmp(ruta_salida, res, opt_verbose);
        liberar_imagen(res);

        if (r_guardar == RET_EXITO) {
            archivos_generados++;
            if (opt_verbose) {
                printf("[INFO] Filtro %s completado exitosamente\n", tareas[t].nombre_identificador);
            }
        } else {
            fprintf(stderr, "ERROR: No se pudo guardar '%s'. Continuando con otros filtros.\n", ruta_salida);
        }
    }

    if (opt_verbose) {
        printf("[INFO] Liberando memoria...\n");
    }
    liberar_imagen(img);

    if (opt_verbose) {
        printf("[INFO] Proceso finalizado - %d archivos generados\n", archivos_generados);
    }

    return RET_EXITO;
}
