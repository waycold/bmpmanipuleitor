# Compilador y opciones obligatorias (sin warnings)
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Nombre del ejecutable y archivos fuentes modulares
TARGET = bmpmanipuleitor.exe
SRCS = main.c funciones_grupo.c imagen.c bmp_io.c filtros.c parser.c

# Regla principal: compila el programa
all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Regla para compilar y generar imagenes de prueba
pruebas:
	$(CC) $(CFLAGS) -o generar_pruebas.exe generar_pruebas.c
	./generar_pruebas.exe

# Comando de limpieza compatible con Windows y Linux
ifeq ($(OS),Windows_NT)
    CLEAN_CMD = cmd /c "del /f /q $(TARGET) generar_pruebas.exe *.o GRUPO_*.bmp 2>nul || exit 0"
else
    CLEAN_CMD = rm -f $(TARGET) generar_pruebas.exe *.o GRUPO_*.bmp
endif

# Regla para limpiar archivos generados
clean:
	$(CLEAN_CMD)
