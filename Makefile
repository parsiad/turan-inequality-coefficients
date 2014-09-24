BIN=coefficients

all: $(BIN)

%: %.cpp $(wildcard *.hpp)
	$(CXX) -std=c++11 -Wall -pedantic -DNDEBUG -O2 -fopenmp -lgmpxx -lgmp -o $@ $<

clean:
	$(RM) $(BIN)

.PHONY: all clean

