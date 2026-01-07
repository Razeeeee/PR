# Kompilator C z ścieżkami do Microsoft MPI
CC = gcc
MPI_INCLUDE = -I"/c/Program Files (x86)/Microsoft SDKs/MPI/Include"
MPI_LIB = -L"/c/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64" -lmsmpi -lws2_32

# Program uruchomieniowy MPI
MPIRUN = mpiexec

# Opcje kompilacji
CFLAGS = -Wall -g $(MPI_INCLUDE)

# Liczba procesów dla różnych programów
NP_1 = 4
NP_2 = 5
NP_3 = 2
NP_4 = 4
NP_5 = 2

# Programy
PROG_1 = zad1_hostname
PROG_2 = zad2_sztafeta
PROG_3 = zad3_packed
PROG_4 = zad4_potok
PROG_5 = zad5_custom_type

ALL_PROGS = $(PROG_1) $(PROG_2) $(PROG_3) $(PROG_4) $(PROG_5)

# Domyślny cel
all: $(ALL_PROGS)

# Kompilacja programów
$(PROG_1): $(PROG_1).c
	$(CC) $(CFLAGS) -o $(PROG_1) $(PROG_1).c $(MPI_LIB)

$(PROG_2): $(PROG_2).c
	$(CC) $(CFLAGS) -o $(PROG_2) $(PROG_2).c $(MPI_LIB)

$(PROG_3): $(PROG_3).c
	$(CC) $(CFLAGS) -o $(PROG_3) $(PROG_3).c $(MPI_LIB)

$(PROG_4): $(PROG_4).c
	$(CC) $(CFLAGS) -o $(PROG_4) $(PROG_4).c $(MPI_LIB)

$(PROG_5): $(PROG_5).c
	$(CC) $(CFLAGS) -o $(PROG_5) $(PROG_5).c $(MPI_LIB)

# Zadanie 1 (3.0) - Wysyłanie hostname
1: clean-1 $(PROG_1)
	@echo "Uruchamiam zadanie 1..."
	$(MPIRUN) -np $(NP_1) ./$(PROG_1)

# Zadanie 2 (3.0) - Sztafeta
2: clean-2 $(PROG_2)
	@echo "Uruchamiam zadanie 2..."
	$(MPIRUN) -np $(NP_2) ./$(PROG_2)

# Zadanie 3 (3.5) - MPI_PACKED
3: clean-3 $(PROG_3)
	@echo "Uruchamiam zadanie 3..."
	$(MPIRUN) -np $(NP_3) ./$(PROG_3)

# Zadanie 4 (4.5) - Przetwarzanie potokowe
4: clean-4 $(PROG_4)
	@echo "Uruchamiam zadanie 4..."
	$(MPIRUN) -np $(NP_4) ./$(PROG_4)

# Zadanie 5 (5.0+) - MPI_Type_create_struct
5: clean-5 $(PROG_5)
	@echo "Uruchamiam zadanie 5..."
	$(MPIRUN) -np $(NP_5) ./$(PROG_5)

# Czyszczenie poszczególnych programów
clean-1:
	@rm -f $(PROG_1) *.o

clean-2:
	@rm -f $(PROG_2) *.o

clean-3:
	@rm -f $(PROG_3) *.o

clean-4:
	@rm -f $(PROG_4) *.o

clean-5:
	@rm -f $(PROG_5) *.o

# Czyszczenie wszystkich programów
clean:
	rm -f $(ALL_PROGS) *.o

.PHONY: all clean clean-1 clean-2 clean-3 clean-4 clean-5 1 2 3 4 5
