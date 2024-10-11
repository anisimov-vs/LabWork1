CXX = g++
CXXFLAGS = -std=c++17

all: bitmap clean

bitmap: main.o bitmap.o
	$(CXX) $(CXXFLAGS) -o main main.o bitmap.o

main.o: src/main.cpp src/bitmap.h
	$(CXX) $(CXXFLAGS) -c src/main.cpp

bitmap.o: src/bitmap.cpp src/bitmap.h
	$(CXX) $(CXXFLAGS) -c src/bitmap.cpp

run: bitmap clean
	./main

clean:
	rm -f *.o


