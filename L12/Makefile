NUM_PROCS = 4

CC = gcc
PYTHON = py -3.10
MPI_INC = "C:/Program Files (x86)/Microsoft SDKs/MPI/Include"
MPI_LIB = "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64"
MPIEXEC = "C:/Program Files/Microsoft MPI/Bin/mpiexec"
CFLAGS = -O2 -Wall
BIN = bin

.PHONY: all clean 1 2 3 4

all: $(BIN)/zad1 $(BIN)/zad2_omp $(BIN)/zad2_mpi $(BIN)/zad3 $(BIN)/zad4

$(BIN):
	mkdir -p $(BIN)

$(BIN)/zad1: zadanie1_pi.c | $(BIN)
	$(CC) $(CFLAGS) -I$(MPI_INC) -o $@ $< -L$(MPI_LIB) -lmsmpi -lm

$(BIN)/zad2_omp: zadanie2_calka_omp.c | $(BIN)
	$(CC) $(CFLAGS) -fopenmp -o $@ $< -lm

$(BIN)/zad2_mpi: zadanie2_mat_vec_mpi.c | $(BIN)
	$(CC) $(CFLAGS) -I$(MPI_INC) -o $@ $< -L$(MPI_LIB) -lmsmpi -lm

$(BIN)/zad3: zadanie3_mat_vec_grupowa.c | $(BIN)
	$(CC) $(CFLAGS) -I$(MPI_INC) -o $@ $< -L$(MPI_LIB) -lmsmpi -lm

$(BIN)/zad4: zadanie4_mat_vec_inplace.c | $(BIN)
	$(CC) $(CFLAGS) -I$(MPI_INC) -o $@ $< -L$(MPI_LIB) -lmsmpi -lm

1: clean $(BIN)/zad1
	@echo ""
	@echo ""
	$(MPIEXEC) -np $(NUM_PROCS) $(BIN)/zad1

2: clean $(BIN)/zad2_omp $(BIN)/zad2_mpi
	@echo ""
	@echo ""
	@for t in 1 2 4 8; do OMP_NUM_THREADS=$$t $(BIN)/zad2_omp $$t; done
	@for p in 1 2 4 8; do $(MPIEXEC) -np $$p $(BIN)/zad2_mpi; done
	$(PYTHON) wykres.py

3: clean $(BIN)/zad3
	@echo ""
	@echo ""
	$(MPIEXEC) -np $(NUM_PROCS) $(BIN)/zad3

4: clean $(BIN)/zad4
	@echo ""
	@echo ""
	$(MPIEXEC) -np $(NUM_PROCS) $(BIN)/zad4

clean:
	rm -rf $(BIN) wyniki_omp.csv wyniki_mpi.csv
