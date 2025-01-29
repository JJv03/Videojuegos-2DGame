# Variables
CC=g++
CFLAGS=-I"./SFML/include" -g -Wall -std=c++20	# -I"<ruta/a/carpeta/include/de/libreria>"
LDFLAGS= -L"./SFML/lib" -lSFML-audio -lSFML-graphics -lSFML-window -lSFML-system		# -L"<ruta/a/carpeta/lib/de/libreria>" -l<nombre_de_la_librería>
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
