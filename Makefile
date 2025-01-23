# Variables
CC=g++
CFLAGS=-I"C:/SDL3-3.2.0/include" -g -Wall
LDFLAGS=-L"C:/SDL3-3.2.0/lib/x64" -lSDL3
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
