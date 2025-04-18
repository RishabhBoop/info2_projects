CXX = g++
CXXFLAGS = -fdiagnostics-color=always -g

main: main.o state.o
	@$(CXX) $(CXXFLAGS) state.o main.o -o checkers

main.o: main.cpp
	@$(CXX) $(CXXFLAGS) -c main.cpp 

state.o: state.cpp state.hpp
	@$(CXX) $(CXXFLAGS) -c state.cpp

clean:	
	@rm -f *.o *.out checkers