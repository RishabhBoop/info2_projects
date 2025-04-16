CXX = g++
CXXFLAGS = -fdiagnostics-color=always -g
CPPFLAGS = -D NCURSES=1

main: main.o field.o
	@$(CXX) $(CXXFLAGS) field.o main.o -o checkers

main.o: main.cpp
	@$(CXX) $(CXXFLAGS) -c main.cpp 

field.o: field.cpp field.hpp
	@$(CXX) $(CXXFLAGS) -c field.cpp
# --------------------------------------------------
ncurses: field_ncurses.o main_ncurses.o 
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) field_ncurses.o main.o -o checkers -lncurses

field_ncurses.o: field.cpp field.hpp
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c field.cpp -o field_ncurses.o

main_ncurses.o: main.cpp
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c main.cpp 
# ---------------------------------------------------
clean:	
	@rm -f *.o *.out checkers