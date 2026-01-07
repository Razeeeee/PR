#include <stdio.h>
#include <string.h>
#include "mpi.h"

typedef struct {
    int id;
    double value;
    char name[20];
} DataStruct;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Status status;
    
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
    
    if (rank == 0) {
        // Proces 0 wysyła strukturę do procesu 1
        DataStruct data = {42, 3.14159, "MPI_PACKED"};
        
        // Pakowanie danych
        char buffer[100];
        int position = 0;
        
        // MPI_Pack - pakuje dane do bufora w standardowym formacie MPI
        // &data.id - adres pola do zapakowania, 1 - ile elementów, MPI_INT - typ
        // buffer - gdzie pakujemy, 100 - rozmiar bufora, &position - aktualna pozycja (in/out)
        // position automatycznie rośnie po każdym MPI_Pack
        MPI_Pack(&data.id, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(&data.value, 1, MPI_DOUBLE, buffer, 100, &position, MPI_COMM_WORLD);
        MPI_Pack(data.name, 20, MPI_CHAR, buffer, 100, &position, MPI_COMM_WORLD);
        
        printf("Proces 0 wysyla: id=%d, value=%.5f, name=%s (packed size=%d)\n", 
               data.id, data.value, data.name, position);
        
        // MPI_Send z typem MPI_PACKED - wysyła zapakowane dane
        // buffer - dane, position - ile bajtów (nie 100!), MPI_PACKED - specjalny typ
        MPI_Send(buffer, position, MPI_PACKED, 1, 0, MPI_COMM_WORLD);
    } 
    else if (rank == 1) {
        // Proces 1 odbiera i rozpakowuje strukturę
        DataStruct received;
        char buffer[100];
        int position = 0;
        
        // MPI_Recv z typem MPI_PACKED - odbiera zapakowane dane
        MPI_Recv(buffer, 100, MPI_PACKED, 0, 0, MPI_COMM_WORLD, &status);
        
        // MPI_Unpack - rozpakowuje dane z bufora
        // buffer - źródło, 100 - rozmiar, &position - pozycja (in/out)
        // &received.id - gdzie zapisać, 1 - ile, MPI_INT - typ
        // WAŻNE: rozpakowywać w tej samej kolejności co pakowanie!
        MPI_Unpack(buffer, 100, &position, &received.id, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, 100, &position, &received.value, 1, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Unpack(buffer, 100, &position, received.name, 20, MPI_CHAR, MPI_COMM_WORLD);
        
        printf("Proces 1 otrzymal: id=%d, value=%.5f, name=%s\n", 
               received.id, received.value, received.name);
    }
    
    MPI_Finalize();
    return 0;
}
