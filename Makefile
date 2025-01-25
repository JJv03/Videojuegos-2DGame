# Variables
CC=g++
CFLAGS=-I".\SDL3\include" -g -Wall -std=c++23	# Asegurarse de que esto apunta a la carpeta "include" de la libreria
LDFLAGS=-L"." -lSDL3	# Poner entre las comillas dobles la ruta hacia el sdl3.dll
SRC=main.cpp
OBJ=main.o
EXEC=game

# Regla por defecto (compilar todo)
all: $(EXEC)

# Enlazar el ejecutable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Compilar el archivo .cpp a .o
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Limpiar archivos generados
clean:
	del -f $(OBJ) $(EXEC)
