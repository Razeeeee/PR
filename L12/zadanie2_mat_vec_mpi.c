// Zadanie 2: mat_vec MPI - pomiary wydajnosci
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 10000

int main(int argc, char *argv[]) {
    int rank, size;
    double t1, t2;
    
    // MPI_Init(&argc, &argv): int - inicjalizacja MPI
    MPI_Init(&argc, &argv);
    // MPI_Comm_rank(comm, &rank): int - numer procesu
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // MPI_Comm_size(comm, &size): int - liczba procesow
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (N % size != 0) {
        if (rank == 0) printf("Blad: N nie podzielne przez size\n");
        MPI_Finalize();
        return 1;
    }
    
    int n_wier = N / size;
    double *x = malloc(N * sizeof(double));
    double *y = malloc(N * sizeof(double));
    double *a = NULL;
    double *a_loc = malloc(n_wier * N * sizeof(double));
    double *y_loc = malloc(n_wier * sizeof(double));
    
    if (rank == 0) {
        a = malloc(N * N * sizeof(double));
        for (int i = 0; i < N * N; i++) a[i] = (i % 100) / 100.0;
        for (int i = 0; i < N; i++) x[i] = (double)(N - i) / N;
    }
    
    // MPI_Bcast(buf, count, type, root, comm): int - broadcast od root
    MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // MPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm): int - rozeslanie czesci danych
    MPI_Scatter(a, n_wier * N, MPI_DOUBLE, a_loc, n_wier * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // MPI_Barrier(comm): int - synchronizacja
    MPI_Barrier(MPI_COMM_WORLD);
    // MPI_Wtime(): double - czas w sekundach
    t1 = MPI_Wtime();
    
    for (int i = 0; i < n_wier; i++) {
        double sum = 0.0;
        for (int j = 0; j < N; j++) sum += a_loc[i * N + j] * x[j];
        y_loc[i] = sum;
    }
    
    // MPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm): int - zebranie danych do root
    MPI_Gather(y_loc, n_wier, MPI_DOUBLE, y, n_wier, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();
    
    if (rank == 0) {
        double czas = t2 - t1;
        printf("MPI: procesy=%d, czas=%.6lf, Gflop/s=%.3lf\n", size, czas, 2.0e-9*N*N/czas);
        
        FILE *fp = fopen("wyniki_mpi.csv", "a");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            if (ftell(fp) == 0) fprintf(fp, "procesy,czas,gflops\n");
            fprintf(fp, "%d,%.9lf,%.6lf\n", size, czas, 2.0e-9*N*N/czas);
            fclose(fp);
        }
        free(a);
    }
    
    free(x); free(y); free(a_loc); free(y_loc);
    // MPI_Finalize(): int - zakonczenie MPI
    MPI_Finalize();
    return 0;
}
