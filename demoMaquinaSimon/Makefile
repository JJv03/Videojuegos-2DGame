# Definir el compilador y las opciones de compilación
CXX = g++
CXXFLAGS = -I"../SFML/include" -g -Wall -std=c++20
LDFLAGS= -L"../SFML/lib" -lSFML-audio -lSFML-graphics -lSFML-window -lSFML-system
OBJ_DIR=build
EXEC=$(OBJ_DIR)/game.exe

# Definir el ejecutable
TARGET = main

# Buscar todos los archivos .cpp en el directorio actual
SRCS = $(wildcard *.cpp)

# Generar los nombres de los archivos .o correspondientes
OBJS = $(SRCS:.cpp=.o)

# Regla por defecto que se ejecuta cuando solo escribes `make`
all: $(TARGET)

# Regla para construir el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Regla para compilar cada archivo .cpp a .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

# Regla para compilar cada archivo .cpp a .o
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)
	
# Regla para limpiar los archivos generados
clean:
	del /Q *.o $(TARGET).exe
