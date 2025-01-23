# Variables
CC=g++
CFLAGS=-I"C:/SDL3-3.2.0/include" -g -Wall	# Asegurarse de que esto apunta a la carpeta "include" de la libreria
LDFLAGS=-L"C:/SDL3-3.2.0/lib/x64" -lSDL3	# Poner entre las comillas dobles la ruta hacia el sdl3.dll
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
	rm -f $(OBJ) $(EXEC)
