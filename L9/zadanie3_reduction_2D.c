#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define WYMIAR 10
#define EPS 1e-6

int main() {
    double a[WYMIAR][WYMIAR];
    
    printf("Rozmiar tablicy: %d x %d\n\n", WYMIAR, WYMIAR);
    
    // Inicjalizacja tablicy
    for(int i = 0; i < WYMIAR; i++) {
        for(int j = 0; j < WYMIAR; j++) {
            a[i][j] = 1.02 * i + 1.01 * j;
        }
    }
    
    // Suma sekwencyjna dla weryfikacji
    double suma_sekw = 0.0;
    for(int i = 0; i < WYMIAR; i++) {
        for(int j = 0; j < WYMIAR; j++) {
            suma_sekw += a[i][j];
        }
    }
    printf("Suma sekwencyjna (referencja): %.6f\n\n", suma_sekw);
    
    // 1. DEKOMPOZYCJA WIERSZOWA z reduction
    printf("1. DEKOMPOZYCJA WIERSZOWA (po wierszach) z reduction:\n");
    printf("   Kazdy watek przetwarza pewne wiersze (zewnetrzna petla)\n");
    printf("   Klauzula reduction automatycznie agreguje wyniki\n\n");
    
    double suma_wierszowa = 0.0;
    double start = omp_get_wtime();
    
    #pragma omp parallel for default(none) shared(a) reduction(+:suma_wierszowa) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        double suma_lokalna = 0.0;
        
        for(int j = 0; j < WYMIAR; j++) {
            suma_lokalna += a[i][j];
        }
        
        suma_wierszowa += suma_lokalna;
        
        #pragma omp critical
        printf("   Watek %d przetwarza wiersz %d, suma = %.6f\n", id, i, suma_lokalna);
    }
    
    double end = omp_get_wtime();
    printf("\n   Suma wierszowa z reduction: %.6f\n", suma_wierszowa);
    printf("   Czas: %.6f s\n", end - start);
    printf("   Zgodnosc: %s\n\n", (suma_wierszowa - suma_sekw < EPS) ? "TAK" : "NIE");
    
    // 2. DEKOMPOZYCJA KOLUMNOWA z reduction
    printf("2. DEKOMPOZYCJA KOLUMNOWA (po kolumnach) z reduction:\n");
    printf("   Kazdy watek przetwarza pewne kolumny\n");
    printf("   Zewnetrzna petla iteruje po kolumnach (j)\n\n");
    
    double suma_kolumnowa = 0.0;
    start = omp_get_wtime();
    
    #pragma omp parallel for default(none) shared(a) reduction(+:suma_kolumnowa) num_threads(4)
    for(int j = 0; j < WYMIAR; j++) {
        int id = omp_get_thread_num();
        double suma_lokalna = 0.0;
        
        for(int i = 0; i < WYMIAR; i++) {
            suma_lokalna += a[i][j];
        }
        
        suma_kolumnowa += suma_lokalna;
        
        #pragma omp critical
        printf("   Watek %d przetwarza kolumne %d, suma = %.6f\n", id, j, suma_lokalna);
    }
    
    end = omp_get_wtime();
    printf("\n   Suma kolumnowa z reduction: %.6f\n", suma_kolumnowa);
    printf("   Czas: %.6f s\n", end - start);
    printf("   Zgodnosc: %s\n\n", (suma_kolumnowa - suma_sekw < EPS) ? "TAK" : "NIE");
    
    // 3. Porównanie wydajności na większej tablicy
    printf("3. POROWNANIE WYDAJNOSCI - duza tablica 10000x10000:\n\n");
    
    int N = 10000;
    double **big_array = (double**)malloc(N * sizeof(double*));
    for(int i = 0; i < N; i++) {
        big_array[i] = (double*)malloc(N * sizeof(double));
        for(int j = 0; j < N; j++) {
            big_array[i][j] = i * 0.01 + j * 0.001;
        }
    }
    
    // Dekompozycja wierszowa
    double suma_w = 0.0;
    start = omp_get_wtime();
    #pragma omp parallel for default(none) shared(N, big_array) reduction(+:suma_w)
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            suma_w += big_array[i][j];
        }
    }
    end = omp_get_wtime();
    double czas_w = end - start;
    
    // Dekompozycja kolumnowa
    double suma_k = 0.0;
    start = omp_get_wtime();
    #pragma omp parallel for default(none) shared(N, big_array) reduction(+:suma_k)
    for(int j = 0; j < N; j++) {
        for(int i = 0; i < N; i++) {
            suma_k += big_array[i][j];
        }
    }
    end = omp_get_wtime();
    double czas_k = end - start;
    
    printf("   Wierszowa: suma = %.2f, czas = %.6f s\n", suma_w, czas_w);
    printf("   Kolumnowa: suma = %.2f, czas = %.6f s\n", suma_k, czas_k);
    printf("\n   Wierszowa jest zazwyczaj szybsza ze wzgledu na\n");
    printf("   lokalnosc pamieci w jezyku C\n");
    printf("   (tablice 2D sa przechowywane wierszami)\n");
    
    // Zwolnienie pamięci
    for(int i = 0; i < N; i++) {
        free(big_array[i]);
    }
    free(big_array);
    
    return 0;
}
