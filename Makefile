CXX = g++

CXXFLAGS = -std=c++17 -Wall

SRC = $(wildcard src/*.cpp)

all:
	$(CXX) $(SRC) -o server $(CXXFLAGS)

clean:
	rm -f server