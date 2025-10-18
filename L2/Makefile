# kompilator c
CCOMP = gcc

# konsolidator
LOADER = gcc

# opcje optymalizacji:
# wersja do debugowania
OPT = -g -DDEBUG
# wersja zoptymalizowana do mierzenia czasu
#OPT = -O3

# pliki naglowkowe
INC = -I../pomiar_czasu

# biblioteki
LIB = -L../pomiar_czasu -lpomiar_czasu -lm

# zaleznosci i komendy
fork: fork.o 
	$(LOADER) $(OPT) fork.o -o fork $(LIB)

# jak uzyskac plik fork.o ?
fork.o: fork.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) fork.c $(INC) 

# zaleznosci i komendy
clone: clone.o 
	$(LOADER) $(OPT) clone.o -o clone $(LIB)

# jak uzyskac plik clone.o ?
clone.o: clone.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) clone.c $(INC) 

# zaleznosci i komendy dla main
main: main.o 
	$(LOADER) $(OPT) main.o -o main $(LIB)

# jak uzyskac plik main.o ?
main.o: main.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) main.c $(INC) 

# zaleznosci i komendy dla watki
watki: watki.o 
	$(LOADER) $(OPT) watki.o -o watki $(LIB)

# jak uzyskac plik watki.o ?
watki.o: watki.c
	$(CCOMP) -c $(OPT) watki.c $(INC) 

# kompilacja program_exec wymaga program
program_exec: program_exec.o program
	$(LOADER) $(OPT) program_exec.o -o program_exec $(LIB)

# jak uzyskac plik program_exec.o ?
program_exec.o: program_exec.c
	$(CCOMP) -c $(OPT) program_exec.c $(INC)

program: program.c
	$(CCOMP) -o program program.c

stos: stos.o 
	$(LOADER) $(OPT) stos.o -o stos $(LIB)

# jak uzyskac plik stos.o ?
stos.o: stos.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) stos.c $(INC)

# regula do kompilacji wszystkich programow
all: fork clone main watki program_exec program stos

clean:
	rm -f *.o fork clone main watki
