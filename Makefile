
all: matmultiply

matmultiply: matmultiply.cpp
	g++ -o matmultiply matmultiply.cpp -pthread 
clean: 
	rm -f *.o matmultiply *.*~