# Nome do executável final
TARGET = main

# Compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -Wall -g -O2 -std=c++17

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Arquivos-fonte e objetos
SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Regra principal
all: $(BIN_DIR)/$(TARGET)

# Compilação final
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilação dos .cpp para .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Executa o programa
run: all
	$(BIN_DIR)/$(TARGET)

# Limpa arquivos gerados
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Recompila tudo do zero
rebuild: clean all
