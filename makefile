stamp: stamp.o main.o
	g++ -Wall -g main.o stamp.o -o stamp -lcrypto

stamp.o: stamp.cpp stamp.h
	g++ -Wall -c stamp.cpp

main.o: main.cpp stamp.h
	g++ -Wall -c main.cpp

clean:
	rm -rf stamp main.o stamp.o