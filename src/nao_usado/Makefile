CPLEXFLAGS=-O3 -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD -I/opt/ibm/ILOG/CPLEX_Studio129/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio129/concert/include  -L/opt/ibm/ILOG/CPLEX_Studio129/cplex/lib/x86-64_linux/static_pic -lilocplex -lcplex -L/opt/ibm/ILOG/CPLEX_Studio129/concert/lib/x86-64_linux/static_pic -lconcert -lm -pthread -std=c++0x -ldl

CFLAGS=-std=c++11 -static-libstdc++ -static-libgcc -Wall -g
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)


all: $(OBJ)
	g++  $(CFLAGS) -o pcdp.run $^ $(CPLEXFLAGS) 

%.o: %.cpp %.hpp
	g++ $(CFLAGS) -c $< -o $@  

cmsa.o: cmsa.cpp
	g++ $(CFLAGS) -c -o cmsa.o cmsa.cpp  $(CPLEXFLAGS)

exato.o: exato.cpp 
	g++ $(CFLAGS) -c $< -o $@ $(CPLEXFLAGS)
	
cplex_pcdp.o: cplex_pcdp.cpp 
	g++ $(CFLAGS) -c $< -o $@ $(CPLEXFLAGS)

clean:
	rm -f *.o *.run
