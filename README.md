# BMPMANIPULEITOR

**Universidad Nacional de La Matanza (UNLaM)**  
**Materia:** Tópicos de Programación (3635)  
**Proyecto:** Manipulación de imágenes sin compresión (BMP 24 bits)

---

## 👥 Datos del Grupo

> **Regla del nombre de grupo:** Debe ser una única palabra válida del diccionario RAE, en MAYÚSCULAS (ASCII `0x41` a `0x5A`), sin espacios, tildes, números ni caracteres especiales (ej: `INVISIBLE`).

- **Nombre del Grupo:** `[NOMBRE_GRUPO]`
- **Integrantes:**
  1. `[DNI]` - `[APELLIDO, Nombre]` (Entrega: Si/No)
  2. `[DNI]` - `[APELLIDO, Nombre]` (Entrega: Si/No)
  3. `[DNI]` - `[APELLIDO, Nombre]` (Entrega: Si/No)

---

## 📁 Estructura del Proyecto

```text
bmpmanipuleitor/
├── img/                       # Carpeta para imágenes BMP de prueba
│   └── .gitkeep
├── .gitignore                 # Archivos y binarios ignorados por Git
├── funciones_grupo.c          # Implementación de funciones del grupo y procesar_imagen
├── funciones_grupo.h          # Cabecera principal con includes, estructuras BMP y constantes
├── main.c                     # Punto de entrada modular (delega a procesar_imagen)
├── Makefile                   # Script de compilación y empaquetado de entrega
└── README.md                  # Documentación del proyecto
```

*(Opcional: Si dividen el desarrollo por integrante, pueden crear `funciones_<apellido>.c` y `funciones_<apellido>.h` e incluirlos en `funciones_grupo.h`).*

---

## ⚙️ Compilación y Ejecución

### Compilación con `gcc` directamente:
```bash
gcc -Wall -Wextra -pedantic -std=c99 -o bmpmanipuleitor.exe main.c funciones_grupo.c
```

### Compilación con `make`:
```bash
make
```

### Limpiar compilados:
```bash
make clean
```


---

## 🚀 Uso y Comandos

Formato general:
```bash
bmpmanipuleitor.exe [OPCIONES] [ARCHIVO(S)]
```

Los archivos resultantes se guardan en el directorio actual con el formato:
`GRUPO_filtro[-valor]_archivo.bmp`

### Utilidades
- `--help`: Muestra la ayuda de uso y lista de comandos disponibles.
- `--info <archivo.bmp>`: Muestra información del encabezado BMP (dimensiones, tamaño, padding, etc.).
- `--validar <archivo.bmp>`: Valida la firma y formato del archivo sin procesarlo.
- `--verbose`: Modo detallado de ejecución con mensajes informativos paso a paso.

### Filtros Básicos
- `--negativo`: Invierte los colores de la imagen.
- `--escala-de-grises`: Convierte a escala de grises promediando componentes RGB.
- `--espejar-horizontal`: Voltea la imagen horizontalmente.
- `--espejar-vertical`: Voltea la imagen verticalmente.

### Filtros con Parámetros (0-100%)
- `--aumentar-contraste=X`: Aumenta el contraste en un `X%`.
- `--reducir-contraste=X`: Reduce el contraste en un `X%`.
- `--tonalidad-azul=X`: Aumenta en `X%` la intensidad del color azul.
- `--tonalidad-verde=X`: Aumenta en `X%` la intensidad del color verde.
- `--tonalidad-roja=X`: Aumenta en `X%` la intensidad del color rojo.
- `--recortar=X`: Mantiene solo el `X%` del tamaño original desde la esquina inferior izquierda (rango `1-100`).
- `--achicar=X`: Reduce el tamaño reescalando la imagen al `X%` (rango `1-100`).

### Rotaciones (90°)
- `--rotar-derecha`: Rota 90° en sentido horario.
- `--rotar-izquierda`: Rota 90° en sentido antihorario.

### Concatenaciones (Requiere 2 imágenes)
- `--concatenar-horizontal`: Une dos imágenes lado a lado alineando por altura.
- `--concatenar-vertical`: Une dos imágenes una sobre otra alineando por ancho.

### Comodín
- `--comodin`: Funcionalidad personalizada elegida por el grupo (debe explicarse en `--help`).

---

## 🛑 Códigos de Retorno

| Código | Constante | Descripción |
|---|---|---|
| `0` | `RET_EXITO` | Proceso ejecutado con éxito. |
| `1` | `RET_ERR_ARGUMENTOS` | Error en los argumentos recibidos. |
| `2` | `RET_ERR_ARCHIVO` | Archivo no encontrado o sin permisos. |
| `3` | `RET_ERR_MEMORIA` | Fallo de reserva de memoria (`malloc`). |
| `4` | `RET_ERR_BMP` | Formato BMP inválido o no soportado (no 24 bits / comprimido). |

---

## 📋 Requisitos de Entrega

- Formato de compresión: `TOPICOS_BMP_YYYY_CC_DIA_GRUPO.zip` (Ejemplo: `TOPICOS_BMP_2026_C2_MIERCOLES_INVISIBLE.zip`).
- **No incluir** archivos binarios (`.exe`, `.o`, `.obj`, etc.) en el zip.
- **Cero warnings** de compilación (`-Wall -Wextra -pedantic`).
- En `funciones_grupo.h` deben figurar los datos de los integrantes en el formato de comentarios especificado.
