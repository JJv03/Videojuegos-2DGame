# Variables del compilador y banderas
CXX         := g++
CXXFLAGS    := -I"./SFML/include" -g -Wall -std=c++20
LDFLAGS     := -L"./SFML/lib" -lSFML-audio -lSFML-graphics -lSFML-window -lSFML-system

# Lista de fuentes y objetos
#SRCS        := main.cpp camera.cpp game.cpp castlevania.cpp gameStateMachine.cpp #resources.cpp
OBJ_DIR     := build
SRC_DIR     := .

SRCS        := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/enemies/*.cpp)
OBJS        := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
EXEC        := $(OBJ_DIR)/game.exe

# Targets "phony"
.PHONY: all clean

# Target por defecto: compila todo
all: $(EXEC)

# Enlazar el ejecutable a partir de los objetos
$(EXEC): $(OBJS) | $(OBJ_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Regla general para compilar archivos .cpp en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpieza de archivos generados
clean:
	del "$(OBJ_DIR)\*.o"
	del "$(EXEC)"
