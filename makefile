CXX=g++
CFLAGS=-Wall -std=c++11
LIBS=-lsfml-window -lsfml-system -lsfml-graphics -lboost_program_options

DEPS=planarIntersections.h segment.h disjoint.h
OBJ=main.o planarIntersections.o segment.o disjoint.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS) $(LIBS)

planar: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm *.o
