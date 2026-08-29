# Compilador y opciones obligatorias (sin warnings)
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

# Nombre del ejecutable y archivos fuentes
TARGET = bmpmanipuleitor.exe
SRCS = main.c funciones_grupo.c

# Regla principal: compila el programa
all:
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Regla para limpiar archivos generados
clean:
	rm -f $(TARGET) *.o
