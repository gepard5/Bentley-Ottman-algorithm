all: main.cpp segment.cpp disjoint.cpp planarIntersections.cpp 
	g++ main.cpp segment.cpp disjoint.cpp planarIntersections.cpp -std=c++11 -Wall -lsfml-window -lsfml-system -lsfml-graphics -lboost_program_options -o test

