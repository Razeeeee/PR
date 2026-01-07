#include <stdio.h>
#include <string.h>
#include "mpi.h"

typedef struct {
    int stage;
    double result;
} PipelineData;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size < 3) {
        if (rank == 0) {
            printf("Program wymaga co najmniej 3 procesow (potok)\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    PipelineData data;
    char buffer[100];
    int position;
    
    if (rank == 0) {
        // Proces 0: inicjalizacja i mnożenie x 2
        data.stage = 0;
        data.result = 10.0;
        
        printf("Proces %d: inicjalizacja = %.2f\n", rank, data.result);
        data.result *= 2.0;
        data.stage++;
        printf("Proces %d: po operacji (x2) = %.2f, stage=%d\n", rank, data.result, data.stage);
        
        // Pakowanie struktury do bufora (SPMD - ten sam kod u wszystkich procesów)
        position = 0;
        MPI_Pack(&data.stage, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(&data.result, 1, MPI_DOUBLE, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, rank + 1, 0, MPI_COMM_WORLD);
    }
    else if (rank == size - 1) {
        // Ostatni proces: odbiór i dodanie + 5
        MPI_Recv(buffer, 100, MPI_PACKED, rank - 1, 0, MPI_COMM_WORLD, &status);
        position = 0;
        MPI_Unpack(buffer, 100, &position, &data.stage, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, 100, &position, &data.result, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        
        printf("Proces %d: otrzymano = %.2f, stage=%d\n", rank, data.result, data.stage);
        data.result += 5.0;
        data.stage++;
        printf("Proces %d: WYNIK KONCOWY (+5) = %.2f, stage=%d\n", rank, data.result, data.stage);
    }
    else {
        // Procesy środkowe: odbiór, operacja, wysłanie
        MPI_Recv(buffer, 100, MPI_PACKED, rank - 1, 0, MPI_COMM_WORLD, &status);
        position = 0;
        MPI_Unpack(buffer, 100, &position, &data.stage, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, 100, &position, &data.result, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        
        printf("Proces %d: otrzymano = %.2f, stage=%d\n", rank, data.result, data.stage);
        
        // Każdy proces wykonuje swoją operację
        if (rank % 2 == 1) {
            data.result += 10.0;
            printf("Proces %d: po operacji (+10) = %.2f\n", rank, data.result);
        } else {
            data.result *= 1.5;
            printf("Proces %d: po operacji (x1.5) = %.2f\n", rank, data.result);
        }
        data.stage++;
        
        position = 0;
        MPI_Pack(&data.stage, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(&data.result, 1, MPI_DOUBLE, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Send(buffer, position, MPI_PACKED, rank + 1, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
