// Zadanie 4: mat_vec z MPI_IN_PLACE (Allgather i Reduce)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 5040

int main(int argc, char *argv[]) {
    int rank, size, n = N;
    double t1, t2;
    
    // MPI_Init(&argc, &argv): int - inicjalizacja MPI
    MPI_Init(&argc, &argv);
    // MPI_Comm_rank(comm, &rank): int - numer procesu
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(comm, &size): int - liczba procesow
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (n % size != 0) {
        if (rank == 0) printf("Blad: N nie podzielne przez size\n");
        MPI_Finalize();
        return 1;
    }
    
    int n_wier = n / size;
    double *x = malloc(n * sizeof(double));
    double *y = malloc(n * sizeof(double));
    double *y_ref = malloc(n * sizeof(double));
    double *a = NULL;
    double *a_loc = malloc(n_wier * n * sizeof(double));
    
    for (int i = 0; i < n; i++) y[i] = 0.0;
    
    if (rank == 0) {
        a = malloc(n * n * sizeof(double));
        for (int i = 0; i < n * n; i++) a[i] = (i % 100) / 100.0;
        for (int i = 0; i < n; i++) x[i] = (double)(n - i) / n;
        for (int i = 0; i < n; i++) {
            double sum = 0.0;
            for (int j = 0; j < n; j++) sum += a[i*n+j] * x[j];
            y_ref[i] = sum;
        }
    }
    
    // MPI_Bcast(buf, count, type, root, comm): int - broadcast od root
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // MPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm): int - rozeslanie czesci
    MPI_Scatter(a, n_wier * n, MPI_DOUBLE, a_loc, n_wier * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // MPI_Barrier(comm): int - synchronizacja
    MPI_Barrier(MPI_COMM_WORLD);
    // MPI_Wtime(): double - czas
    t1 = MPI_Wtime();
    
    int start = rank * n_wier;
    for (int i = 0; i < n_wier; i++) {
        double sum = 0.0;
        for (int j = 0; j < n; j++) sum += a_loc[i * n + j] * x[j];
        y[start + i] = sum;
    }
    
    // MPI_Allgather(MPI_IN_PLACE, cnt, type, buf, cnt, type, comm): int - IN_PLACE: dane juz w buforze
    MPI_Allgather(MPI_IN_PLACE, n_wier, MPI_DOUBLE, y, n_wier, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();
    
    double err = 0.0;
    if (rank == 0) {
        for (int i = 0; i < n; i++) err += (y[i] - y_ref[i]) * (y[i] - y_ref[i]);
        // MPI_Reduce(MPI_IN_PLACE, buf, cnt, type, op, root, comm): int - root: IN_PLACE = wynik w buf
        MPI_Reduce(MPI_IN_PLACE, &err, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    } else {
        // MPI_Reduce(sendbuf, NULL, cnt, type, op, root, comm): int - nie-root: wysyla do root
        MPI_Reduce(&err, NULL, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    
    if (rank == 0) {
        printf("Zad4 (IN_PLACE): procesy=%d, czas=%.6lf, Gflop/s=%.3lf, err=%.2e\n", 
               size, t2-t1, 2.0e-9*n*n/(t2-t1), sqrt(err));
        free(a);
    }
    
    free(x); free(y); free(y_ref); free(a_loc);
    // MPI_Finalize(): int - zakonczenie MPI
    MPI_Finalize();
    return 0;
}
