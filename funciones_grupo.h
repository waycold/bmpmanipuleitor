#ifndef FUNCIONES_GRUPO_H
#define FUNCIONES_GRUPO_H

// Apellido: APELLIDO, Nombre
// DNI: 00000000
// Entrega: Si

// Apellido: APELLIDO, Nombre
// DNI: 00000000
// Entrega: Si

// Apellido: APELLIDO, Nombre
// DNI: 00000000
// Entrega: Si

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* Nombre del grupo para los nombres de archivo de salida (ej: GRUPO_filtro_archivo.bmp) */
#define NOMBRE_GRUPO "GRUPO"

/* Inclusión de todos los módulos del proyecto según requerimiento de la cátedra */
#include "imagen.h"
#include "bmp_io.h"
#include "filtros.h"
#include "parser.h"

/* Función principal requerida que delega la ejecución */
int procesar_imagen(int argc, char *argv[]);

#endif // FUNCIONES_GRUPO_H
