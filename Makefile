CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2 $(NIX_CFLAGS_COMPILE)
LDFLAGS = $(NIX_LDFLAGS) -lGL -lGLU -lglut -lm

TARGET = black_hole_simulation
SRC = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
