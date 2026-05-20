CXX = g++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -pthread
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = engineSimulator

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
