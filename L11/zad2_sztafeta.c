#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int rank, size;
    int data = 0;
    MPI_Status status;
    
    // MPI_Init, MPI_Comm_rank, MPI_Comm_size - standardowa inicjalizacja
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size < 2) {
        if (rank == 0) {
            printf("Program wymaga co najmniej 2 procesow\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    // Podzial na grupy: 0 - poczatek, 1..size-2 - srodek, size-1 - koniec
    
    if (rank == 0) {
        // Proces poczatkowy - rozpoczyna sztafete
        data = 100;
        printf("[POCZATEK] Proces %d wysyla wartosc: %d\n", rank, data);
        
        // MPI_Send - wysyła liczbę całkowitą do następnego procesu
        // &data - adres zmiennej, 1 - ile liczb, MPI_INT - typ int
        // rank+1 - do następnego procesu w kolejce
        MPI_Send(&data, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    } 
    else if (rank == size - 1) {
        // MPI_Recv - odbiera liczbę od poprzedniego procesu
        // rank-1 - konkretnie od poprzednika (sztafeta w kolejności)
        MPI_Recv(&data, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("[KONIEC] Proces %d otrzymal wartosc: %d\n", rank, data);
    } 
    else {
        // Proces srodkowy: odbiera -> modyfikuje -> wysyla
        MPI_Recv(&data, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        data += rank;
        printf("[SRODEK] Proces %d: otrzymal, dodal %d, wysyla: %d\n", rank, rank, data);
        MPI_Send(&data, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
