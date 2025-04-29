# Variables del compilador y banderas
CXX         := g++
CXXFLAGS    := -I"./SFML/include" -g -Wall -std=c++20
LDFLAGS     := -L"./SFML/lib" -lSFML-audio -lSFML-graphics -lSFML-window -lSFML-system

# Archivos fuente y objetos
OBJ_DIR     := build
SRC_DIR     := .

SRCS        := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/enemies/*.cpp) $(wildcard $(SRC_DIR)/bosses/*.cpp)
OBJS        := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
EXEC        := $(OBJ_DIR)/Castlevania.exe

# Recurso (icono)
RC_FILE     := icon.rc
RES_FILE    := $(OBJ_DIR)/icon.rc

# Targets "phony"
.PHONY: all clean

# Target por defecto
all: $(EXEC)

# Enlazar ejecutable con objetos y recursos
$(EXEC): $(OBJS) $(RES_FILE) | $(OBJ_DIR)
	$(CXX) $(OBJS) $(RES_FILE) -o $@ $(LDFLAGS)
#	$(CXX) $(OBJS) $(RES_FILE) -mwindows -o $@ $(LDFLAGS)

# Compilar .cpp en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar archivo de recursos
$(RES_FILE): $(RC_FILE) | $(OBJ_DIR)
	windres $(RC_FILE) -O coff -o $(RES_FILE)

# Limpiar archivos generados
clean:
	del "$(OBJ_DIR)\*.o"
	del "$(OBJ_DIR)\enemies\*.o"
	del "$(OBJ_DIR)\bosses\*.o"
	del "$(OBJ_DIR)\*.exe"
