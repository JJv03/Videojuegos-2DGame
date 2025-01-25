# Variables
CC=g++
CFLAGS=-I"./SDL3/include" -I"./SDL3_image/include" -g -Wall -std=c++23	# -I"<ruta/a/carpeta/include/de/libreria>"
LDFLAGS=-L"." -lSDL3 -L"." -lSDL3_image 		# poner entre las comillas dobles ("") la ruta hacia el .dll de la librería
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
	del -f $(OBJ) $(EXEC).exe
