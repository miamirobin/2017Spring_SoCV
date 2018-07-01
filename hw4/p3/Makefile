CFLAGS = -O3 -Wall
CFLAGS = -g -Wall

seatAss: clean File.o Proof.o Solver.o seatAss.o
	g++ -o $@ $(CFLAGS) File.o Proof.o Solver.o seatAss.o

File.o: File.cpp
	g++ -c $(CFLAGS) File.cpp

Proof.o: Proof.cpp
	g++ -c $(CFLAGS) Proof.cpp

Solve.o: Solver.cpp
	g++ -c $(CFLAGS) Solver.cpp

seatAss.o: seatAss.cpp
	g++ -c $(CFLAGS) seatAss.cpp

clean:
	rm -f *.o seatAss core* tags
