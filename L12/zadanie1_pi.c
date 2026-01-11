// Zadanie 1: PI z MPI_Bcast i MPI_Reduce (wzor Leibniza)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

int main(int argc, char *argv[]) {
    int rank, size;
    long long N = 100000000;
    double suma_lok = 0.0, suma_glob = 0.0;
    double t1, t2;

    // MPI_Init(&argc, &argv): int - inicjalizacja MPI
    MPI_Init(&argc, &argv);
    // MPI_Comm_rank(comm, &rank): int - numer procesu
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(comm, &size): int - liczba procesow
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // MPI_Bcast(buf, count, type, root, comm): int - broadcast od root do wszystkich
    MPI_Bcast(&N, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // MPI_Barrier(comm): int - synchronizacja wszystkich procesow
    MPI_Barrier(MPI_COMM_WORLD);
    // MPI_Wtime(): double - aktualny czas w sekundach
    t1 = MPI_Wtime();

    for (long long i = rank; i < N; i += size) {
        double znak = (i % 2 == 0) ? 1.0 : -1.0;
        suma_lok += znak / (2.0 * i + 1.0);
    }

    // MPI_Reduce(sendbuf, recvbuf, count, type, op, root, comm): int - redukcja do root
    MPI_Reduce(&suma_lok, &suma_glob, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    t2 = MPI_Wtime();

    if (rank == 0) {
        double pi = 4.0 * suma_glob;
        printf("PI=%.15lf, blad=%.2e, czas=%.6lf s, procesy=%d\n", 
               pi, fabs(pi - M_PI), t2 - t1, size);
    }

    // MPI_Finalize(): int - zakonczenie MPI
    MPI_Finalize();
    return 0;
}
