# BMPMANIPULEITOR — Guía de Estudio y Arquitectura

**Universidad Nacional de La Matanza (UNLaM)**  
**Materia:** Tópicos de Programación (3635)  
**Proyecto:** Manipulación de imágenes sin compresión (BMP 24 bits)

---

## 👥 Datos del Grupo

> **Regla de nomenclatura del grupo:**  
> Debe ser una única palabra válida del diccionario de la RAE, escrita en **MAYÚSCULAS** (ASCII `0x41` a `0x5A`), sin espacios, números, tildes ni caracteres especiales (ejemplo: `INVISIBLE`).

- **Nombre del Grupo:** `GRUPO`  *(reemplazar con el nombre elegido)*
- **Integrantes:**
  1. `// Apellido: APELLIDO, Nombre` — `// DNI: 00000000` — `// Entrega: Si`
  2. `// Apellido: APELLIDO, Nombre` — `// DNI: 00000000` — `// Entrega: Si`
  3. `// Apellido: APELLIDO, Nombre` — `// DNI: 00000000` — `// Entrega: Si`

---

## ⚡ Guía Rápida: Compilar, Probar y Lanzar

### 1. Compilación (0 warnings garantizados)
Compilar con `gcc` directamente bajo estándar `C99` y banderas estrictas:
```bash
gcc -Wall -Wextra -pedantic -std=c99 -o bmpmanipuleitor.exe main.c funciones_grupo.c imagen.c bmp_io.c filtros.c parser.c
```
O usando `make`:
```bash
make
```

### 2. Generar imágenes de prueba
El proyecto incluye un generador de casos límite (`generar_pruebas.c`):
```bash
gcc -Wall -Wextra -pedantic -std=c99 -o generar_pruebas.exe generar_pruebas.c
./generar_pruebas.exe
```
Esto creará en la carpeta `img/`:
- `cuadrada_100x100.bmp`: Imagen cuadrada con padding = 0.
- `rectangular_150x75.bmp`: Imagen rectangular con padding = 2 bytes.
- `con_padding_239x80.bmp`: Ancho impar exigente con padding = 3 bytes.
- `minima_1x1.bmp`: Imagen mínima de 1x1 píxel con padding = 1 byte.
- `corrupta_firma.bmp`: Archivo sin la firma `BM` (para probar error 4).
- `corrupta_8bits.bmp`: Archivo de 8 bits en vez de 24 (para probar error 4).

### 3. Comandos de prueba recomendados

```bash
# Ayuda y sintaxis
./bmpmanipuleitor.exe --help

# Inspección de encabezados y padding
./bmpmanipuleitor.exe --info img/con_padding_239x80.bmp

# Validación formal sin procesar
./bmpmanipuleitor.exe --validar img/rectangular_150x75.bmp
./bmpmanipuleitor.exe --validar img/corrupta_8bits.bmp    # Retorna código 4

# Modo verbose con múltiples filtros en una sola llamada
./bmpmanipuleitor.exe --verbose --negativo --escala-de-grises --aumentar-contraste=25 img/cuadrada_100x100.bmp

# Concatenación de dos imágenes (lado a lado o vertical)
./bmpmanipuleitor.exe --concatenar-horizontal img/cuadrada_100x100.bmp img/rectangular_150x75.bmp
```

---

## 📁 Arquitectura Modular: ¿Cómo está organizado el código?

El proyecto está diseñado con **responsabilidad única (Single Responsibility Principle)** para que puedas estudiar o programar cada parte de forma independiente:

```
bmpmanipuleitor/
├── main.c              -> Punto de entrada mínimo (solo delega a procesar_imagen).
├── funciones_grupo.h   -> Interfaz obligatoria de la cátedra; incluye todos los módulos.
├── funciones_grupo.c   -> Implementación de procesar_imagen delegando al parser.
│
├── imagen.h / .c       -> MÓDULO 1: Gestión de memoria dinámica 2D (crear, clonar, liberar).
├── bmp_io.h / .c       -> MÓDULO 2: Lectura/escritura binaria de BMP, padding y validación.
├── filtros.h / .c      -> MÓDULO 3: Algoritmos puros de transformación de píxeles.
├── parser.h / .c       -> MÓDULO 4: Intérprete de línea de comandos, deduplicación y orquestación.
│
├── generar_pruebas.c   -> Herramienta de generación de archivos BMP de prueba.
├── Makefile            -> Script de automatización de compilación y limpieza.
└── img/                -> Banco de pruebas con imágenes válidas y corruptas.
```

---

## 🧠 Conceptos Teóricos Clave para Estudiar y Programar a Mano

### 1. Anatomía Binaria de un Archivo BMP de 24 bits
Un archivo BMP se compone de 3 secciones continuas:

```
+-------------------------------------------------------------+
| 1. EncabezadoArchivoBMP (BITMAPFILEHEADER)   [14 bytes]     |
|    - uint16_t tipo            -> "BM" (0x4D42 en little-endian) |
|    - uint32_t tamano_archivo  -> Tamaño total en bytes      |
|    - uint16_t reservado1, 2   -> 0                          |
|    - uint32_t offset_datos    -> Posición de los píxeles (54)|
+-------------------------------------------------------------+
| 2. EncabezadoInfoBMP (BITMAPINFOHEADER)      [40 bytes]     |
|    - uint32_t tamano_cabecera -> 40                         |
|    - int32_t  ancho, alto     -> Dimensiones en píxeles     |
|    - uint16_t planos          -> 1                          |
|    - uint16_t bpp             -> 24 bits                    |
|    - uint32_t compresion      -> 0 (BI_RGB sin compresión)  |
|    - uint32_t tamano_imagen   -> (ancho*3 + pad) * alto     |
+-------------------------------------------------------------+
| 3. Matriz de Píxeles (BGR de abajo hacia arriba)            |
|    - Cada píxel = 3 bytes consecutivos: Azul, Verde, Rojo   |
|    - Padding = Bytes nulos al final de cada fila            |
+-------------------------------------------------------------+
```

> **¡Cuidado con el empaquetamiento de structs!**  
> En C, los compiladores suelen agregar *padding bytes* internos a los structs para alinearlos a múltiplos de 4 u 8 bytes. Si no usas `#pragma pack(push, 1)`, `EncabezadoArchivoBMP` medirá 16 bytes en vez de 14, desplazando toda la lectura y corrompiendo el archivo.

---

### 2. El Padding: ¿Qué es y por qué existe?
En la arquitectura Windows/x86, leer bloques alineados a 4 bytes (32 bits) es mucho más veloz en hardware.  
Como cada píxel ocupa 3 bytes, el total de bytes útiles de una fila es `ancho * 3`. Si ese valor no es múltiplo de 4, el formato BMP rellena con ceros al final de la fila.

$$\text{padding} = (4 - ((\text{ancho} \times 3) \pmod 4)) \pmod 4$$

**Ejemplos prácticos:**
- Si `ancho = 100` $\implies 100 \times 3 = 300 \implies 300 \pmod 4 = 0 \implies$ **0 bytes de padding**.
- Si `ancho = 150` $\implies 150 \times 3 = 450 \implies 450 \pmod 4 = 2 \implies$ **2 bytes de padding**.
- Si `ancho = 239` $\implies 239 \times 3 = 717 \implies 717 \pmod 4 = 1 \implies$ **3 bytes de padding**.
- Si `ancho = 1` $\implies 1 \times 3 = 3 \implies 3 \pmod 4 = 3 \implies$ **1 byte de padding**.

---

### 3. Orientación de Filas: Bottom-Up vs. Matriz Visual
En los BMP estándar, la fila 0 del archivo corresponde a la **fila inferior** de la imagen.  
Para evitar confusiones en los filtros, este proyecto mapea la imagen a una matriz visual intuitiva:
- `matriz[0]` es la fila **superior**.
- `matriz[alto - 1]` es la fila **inferior**.

Por lo tanto:
- **Al leer de disco:** Se lee desde `y = alto - 1` decreciendo hasta `0`.
- **Al escribir a disco:** Se escribe desde `y = alto - 1` decreciendo hasta `0` (agregando el padding tras cada fila).
- **En los filtros:** Trabajas con `matriz[y][x]` de forma totalmente natural.

---

### 4. Matrices Dinámicas 2D y Prevención de Fugas de Memoria
Para cumplir con el requisito obligatorio de matrices 2D, se utiliza un puntero doble:
```c
Pixel **matriz;
```

#### Asignación segura con Rollback (`crear_imagen`):
Si reservamos 1000 filas y en la fila 500 falla `malloc`, no se puede retornar directamente porque las 499 filas previas quedarían perdidas en memoria (memory leak). El patrón de rollback libera lo previo:
```c
img->matriz = (Pixel **)malloc(alto * sizeof(Pixel *));
for (int y = 0; y < alto; y++) {
    img->matriz[y] = (Pixel *)malloc(ancho * sizeof(Pixel));
    if (!img->matriz[y]) {
        for (int k = 0; k < y; k++) free(img->matriz[k]);
        free(img->matriz);
        free(img);
        return NULL; /* Retorna código 3 (RET_ERR_MEMORIA) */
    }
}
```

#### Liberación completa (`liberar_imagen`):
Se liberan primero las filas individuales, luego el vector de punteros y finalmente la estructura.

---

## 🎨 Resumen de Filtros y Matemáticas

| Filtro | Argumento | Fórmula / Lógica |
|---|---|---|
| **Negativo** | `--negativo` | `color = 255 - original` en B, G y R. |
| **Escala de Grises** | `--escala-de-grises` | Promedio aritmético: `gris = (r + g + b) / 3`. |
| **Espejar Horizontal** | `--espejar-horizontal` | Intercambio de columnas: `dst[y][x] = src[y][ancho - 1 - x]`. |
| **Espejar Vertical** | `--espejar-vertical` | Intercambio de filas: `dst[y][x] = src[alto - 1 - y][x]`. |
| **Aumentar Contraste** | `--aumentar-contraste=X` | $Factor = 1.0 + (X / 100.0)$. $Nuevo = 128 + (Original - 128) \times Factor$. |
| **Reducir Contraste** | `--reducir-contraste=X` | $Factor = 1.0 - (X / 100.0)$. $Nuevo = 128 + (Original - 128) \times Factor$. |
| **Tonalidades** | `--tonalidad-[azul\|verde\|roja]=X` | Canal elegido multiplicado por $(1.0 + X / 100.0)$ clampeado a 255. |
| **Recortar** | `--recortar=X` | Conserva el $X\%$ inferior izquierdo: columnas `[0, nuevo_ancho)` y filas `[alto - nuevo_alto, alto)`. |
| **Achicar** | `--achicar=X` | Muestreo de vecino más cercano: `orig_x = (x * ancho) / nuevo_ancho`. |
| **Rotar Derecha** | `--rotar-derecha` | Giro 90° horario: $nuevo\_ancho = alto$, $nuevo\_alto = ancho$. `dst[x][alto - 1 - y] = src[y][x]`. |
| **Rotar Izquierda** | `--rotar-izquierda` | Giro 90° antihorario: `dst[ancho - 1 - x][y] = src[y][x]`. |
| **Concatenar Horizontal** | `--concatenar-horizontal` | Lado a lado (primero img1). Sobrante rellenado con teal `{128, 128, 0}`. |
| **Concatenar Vertical** | `--concatenar-vertical` | Una sobre otra (img1 arriba). Sobrante rellenado con teal. |
| **Comodín** | `--comodin` | Efecto Sepia (Vintage cálido) mediante pesos cromáticos estándar W3C. |

---

## 🛑 Códigos de Retorno del Programa

| Código | Macro | Causa |
|---|---|---|
| `0` | `RET_EXITO` | Todo se completó con éxito (o se invocó `--help`). |
| `1` | `RET_ERR_ARGUMENTOS` | Faltan argumentos, sin imágenes, cantidad incorrecta de imágenes para concatenación (deben ser 2) o filtros regulares (debe ser 1). |
| `2` | `RET_ERR_ARCHIVO` | El archivo no existe o no tiene permisos de lectura/escritura. |
| `3` | `RET_ERR_MEMORIA` | Falló alguna llamada a `malloc`. |
| `4` | `RET_ERR_BMP` | Archivo con firma distinta de `BM`, profundidad distinta de 24 bits, comprimido o truncado. |

---

## 📋 Reglas de Entrega de la UNLaM

- **Nombre del archivo comprimido:**  
  `TOPICOS_BMP_YYYY_CC_DIA_GRUPO.zip`  
  *(Ejemplo: `TOPICOS_BMP_2026_C2_MIERCOLES_INVISIBLE.zip`)*
- **Archivos binarios prohibidos:**  
  **No incluir** ningún `.exe`, `.o`, `.obj` ni imágenes temporales de salida (`GRUPO_*.bmp`).
- **Comentarios en `funciones_grupo.h`:**  
  Deben figurar los datos de todos los integrantes con el formato exacto requerido:
  ```c
  // Apellido: Spinetta, Luis Alberto
  // DNI: 12345678
  // Entrega: Si
  ```
