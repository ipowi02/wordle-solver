CXX = g++
CXXFLAGS = -Wall -Wextra -g -std=c++20

main: main.cpp
	$(CXX) $(CXXFLAGS) -o main main.cpp

clean:
	rm -f main