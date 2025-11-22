CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRC = main.cpp
OUT = raytracer

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
	./$(OUT) example.txt

clean:
	rm -f $(OUT) out.png
