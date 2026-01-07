#include <stdio.h>
#include <string.h>
#include "mpi.h"

typedef struct {
    int id;
    double temperature;
    char status[10];
} SensorData;

int main(int argc, char** argv) {
    int rank, size;
    MPI_Status status;
    MPI_Datatype sensor_type;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Definicja własnego typu danych MPI dla struktury SensorData
    // blocklengths - ile elementów każdego pola: 1 int, 1 double, 10 charów
    // types - odpowiadające typy MPI dla każdego pola
    int blocklengths[3] = {1, 1, 10};
    MPI_Aint displacements[3];
    MPI_Datatype types[3] = {MPI_INT, MPI_DOUBLE, MPI_CHAR};
    
    // MPI_Get_address - pobiera adresy pól struktury
    // Potrzebne do obliczenia przesunieć (offsetów) między polami
    // displacements[i] - wyjście, adres w formacie MPI_Aint
    SensorData sample;
    MPI_Get_address(&sample.id, &displacements[0]);
    MPI_Get_address(&sample.temperature, &displacements[1]);
    MPI_Get_address(&sample.status, &displacements[2]);
    
    // Obliczenie przesunięć względem początku struktury
    displacements[2] -= displacements[0];
    displacements[1] -= displacements[0];
    displacements[0] = 0;
    
    // MPI_Type_create_struct - tworzy nowy typ MPI pasujący do struktury
    // 3 - liczba pól, blocklengths - ile elementów, displacements - offsety
    // types - typy MPI, &sensor_type - wyjście (nowy typ)
    MPI_Type_create_struct(3, blocklengths, displacements, types, &sensor_type);
    
    // MPI_Type_commit - rejestruje typ w MPI (musi być przed użyciem)
    MPI_Type_commit(&sensor_type);
    
    if (size < 2) {
        if (rank == 0) {
            printf("Program wymaga co najmniej 2 procesow\n");
        }
        MPI_Type_free(&sensor_type);
        MPI_Finalize();
        return 1;
    }
    
    if (rank == 0) {
        // Proces 0 wysyła strukturę do procesu 1
        SensorData data = {101, 23.5, "OK"};
        
        printf("Proces 0 wysyla: id=%d, temp=%.1fC, status=%s\n", 
               data.id, data.temperature, data.status);
        
        // MPI_Send z własnym typem - wysyłamy całą strukturę jednym wywołaniem
        // &data - adres struktury, 1 - wysyłamy 1 strukturę, sensor_type - nasz typ
        MPI_Send(&data, 1, sensor_type, 1, 0, MPI_COMM_WORLD);
    } 
    else if (rank == 1) {
        // Proces 1 odbiera strukturę
        SensorData received;
        
        // MPI_Recv z własnym typem - odbieramy całą strukturę bezpośrednio
        MPI_Recv(&received, 1, sensor_type, 0, 0, MPI_COMM_WORLD, &status);
        
        printf("Proces 1 otrzymal: id=%d, temp=%.1fC, status=%s\n", 
               received.id, received.temperature, received.status);
    }
    
    // MPI_Type_free - zwalnia zasób typów (przed MPI_Finalize)
    MPI_Type_free(&sensor_type);
    MPI_Finalize();
    return 0;
}
