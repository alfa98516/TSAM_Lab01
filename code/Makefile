# c++ files 
MAIN=main.cpp

# hpp files

# CXXflags
STD=-std=c++17 # i like c++17 :)
WARNINGS=-Wpedantic
SYMBOLS=-g 
SANITIZE=-fsanitize=undefined,address,leak
CC=g++
CXXFLAGS=$(SYMBOLS) $(WARNINGS) $(STD) $(SANITIZE)

# Out file 
OUT = scanner

OUT: $(MAIN) $(TREE)
	$(CC) $(CXXFLAGS) $(MAIN) -o $(OUT)

clean:
	rm -f a.out
.PHONY: clean out
