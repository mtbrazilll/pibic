# Flags de compilação
CFLAGS=-std=c++14 -static-libstdc++ -static-libgcc -Wall -g

# Flags de inclusão e outras flags do CPLEX
CPLEX_INCLUDE_FLAGS=-O3 -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -I/opt/ibm/ILOG/CPLEX_Studio129/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio129/concert/include

# Flags de ligação do CPLEX e bibliotecas relacionadas
CPLEX_LIB_FLAGS=-L/opt/ibm/ILOG/CPLEX_Studio129/cplex/lib/x86-64_linux/static_pic -lilocplex -lcplex -L/opt/ibm/ILOG/CPLEX_Studio129/concert/lib/x86-64_linux/static_pic -lconcert -lm -pthread -ldl

# Flags de ligação da GMP
GMP_LIB_FLAGS=-lgmp

# Fontes e objetos
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

# Regra principal
all: $(OBJ)
	g++ $(CFLAGS) -o pcdp.run $^ $(GMP_LIB_FLAGS) $(CPLEX_LIB_FLAGS) 

# Regras para compilar arquivos fonte individuais
obj/%.o: src/%.cpp src/%.hpp
	g++ $(CFLAGS) $(CPLEX_INCLUDE_FLAGS) -c $< -o $@

obj/%.o: src/%.cpp
	g++ $(CFLAGS) $(CPLEX_INCLUDE_FLAGS) -c $< -o $@

# Regra de limpeza
clean:
	rm -f obj/*.o *.run

# Regra adicional
deus:
	rm -f obj/cmsa.o *.run
