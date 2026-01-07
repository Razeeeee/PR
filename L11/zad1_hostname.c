#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "mpi.h"

#define HOSTNAME_LEN 256

int main(int argc, char** argv) {
    int rank, size;
    char hostname[HOSTNAME_LEN];
    char original_hostname[HOSTNAME_LEN];
    char received_hostname[HOSTNAME_LEN];
    MPI_Status status;
    
    // MPI_Init - inicjalizacja środowiska MPI, musi być pierwsze
    // argc, argv - przekazujemy argumenty z main (MPI może używać flag jak --np)
    // Zwraca status operacji
    MPI_Init(&argc, &argv);
    
    // MPI_Comm_rank - pobiera numer bieżącego procesu
    // MPI_COMM_WORLD - domyślny komunikator (wszystkie procesy)
    // &rank - tu zostanie zapisany numer procesu (0, 1, 2...)
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    // MPI_Comm_size - pobiera całkowitą liczbę uruchomionych procesów
    // &size - tu zostanie zapisana liczba procesów
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Kazdy proces tworzy unikalna nazwe (hostname + rank)
    gethostname(original_hostname, HOSTNAME_LEN);
    sprintf(hostname, "Host-%s-Proc%d", original_hostname, rank);
    
    if (rank != 0) {
        // MPI_Send - wysyła dane do innego procesu (blokujące)
        // hostname - bufor z danymi, HOSTNAME_LEN - ile bajtów, MPI_CHAR - typ char
        // 0 - rank odbiorcy (proces 0), 0 - tag wiadomości, MPI_COMM_WORLD - komunikator
        // Blokuje do momentu bezpiecznego skopiowania danych
        MPI_Send(hostname, HOSTNAME_LEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    } else {
        // Proces 0 zbiera informacje od innych
        printf("[Proces 0] Moja nazwa: %s\n", hostname);
        printf("[Proces 0] Czekam na wiadomosci od pozostalych procesow...\n\n");
        
        for (int i = 1; i < size; i++) {
            // MPI_Recv - odbiera dane od innego procesu (blokujące)
            // received_hostname - bufor na dane, HOSTNAME_LEN - maks rozmiar, MPI_CHAR - typ
            // MPI_ANY_SOURCE - odbierz od dowolnego procesu, MPI_ANY_TAG - dowolny tag
            // &status - tu zostanie info o nadawcy (status.MPI_SOURCE)
            MPI_Recv(received_hostname, HOSTNAME_LEN, MPI_CHAR, MPI_ANY_SOURCE, 
                     MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("[Proces 0] Otrzymano od procesu %d: nazwa = %s\n", 
                   status.MPI_SOURCE, received_hostname);
        }
    }
    
    // MPI_Finalize - zamyka środowisko MPI, musi być ostatnie
    // Po wywołaniu nie można używać funkcji MPI
    MPI_Finalize();
    return 0;
}
