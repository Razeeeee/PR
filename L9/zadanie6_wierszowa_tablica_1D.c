#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define WYMIAR 10
#define EPS 1e-6

int main() {
    double a[WYMIAR][WYMIAR];
    double sumy_wierszy[WYMIAR]; // Tablica 1D do przechowywania sum wierszy
    
    printf("Rozmiar tablicy: %d x %d\n", WYMIAR, WYMIAR);
    printf("Tablica pomocnicza: 1D o rozmiarze %d (po jednym elemencie na wiersz)\n\n", WYMIAR);
    
    // Inicjalizacja tablicy 2D
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
    
    // ETAP 1: Równoległe obliczanie sum wierszy -> tablica 1D
    printf("ETAP 1: Rownolegle obliczanie sum wierszy\n");
    printf("Kazdy watek przetwarza pewne wiersze i zapisuje wyniki do tablicy 1D\n\n");
    
    double start = omp_get_wtime();
    
    // Równoległe wypełnianie tablicy sum wierszy
    #pragma omp parallel for default(none) shared(a, sumy_wierszy) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        double suma_wiersza = 0.0;
        
        // Sumowanie elementów w wierszu i
        for(int j = 0; j < WYMIAR; j++) {
            suma_wiersza += a[i][j];
        }
        
        sumy_wierszy[i] = suma_wiersza;
        
        printf("Watek %d: wiersz %d -> suma = %.6f\n", id, i, suma_wiersza);
    }
    
    printf("\nTablica 1D sum wierszy:\n");
    for(int i = 0; i < WYMIAR; i++) {
        printf("sumy_wierszy[%d] = %.6f\n", i, sumy_wierszy[i]);
    }
    printf("\n");
    
    // ETAP 2: Sumowanie tablicy 1D różnymi metodami
    printf("ETAP 2: Sumowanie tablicy 1D sum wierszy\n");

    // Metoda 2a: Reduction na tablicy 1D
    printf("Metoda 2a: Reduction na tablicy 1D\n");
    double suma_reduction = 0.0;
    #pragma omp parallel for default(none) shared(sumy_wierszy) reduction(+:suma_reduction)
    for(int i = 0; i < WYMIAR; i++) {
        suma_reduction += sumy_wierszy[i];
    }
    printf("Suma (reduction): %.6f\n\n", suma_reduction);
    
    // Metoda 2b: Sekwencyjne sumowanie tablicy 1D (mała tablica)
    printf("Metoda 2b: Sekwencyjne sumowanie tablicy 1D\n");
    double suma_sekw_1d = 0.0;
    for(int i = 0; i < WYMIAR; i++) {
        suma_sekw_1d += sumy_wierszy[i];
    }
    printf("Suma (sekwencyjna): %.6f\n\n", suma_sekw_1d);
    
    double end = omp_get_wtime();
    
    printf("Zgodnosc z referencja: %s\n", (suma_reduction - suma_sekw < EPS) ? "TAK" : "NIE");
    printf("Calkowity czas: %.6f s\n\n", end - start);
    
    // Jedna operacja - parallel for z lokalną tablicą
    printf("Wszystko w jednej dyrektywie parallel\n");
    
    double sumy_w2[WYMIAR];
    double suma_finalna = 0.0;
    
    start = omp_get_wtime();
    
    #pragma omp parallel default(none) shared(a, sumy_w2, suma_finalna)
    {
        // Faza 1: Obliczanie sum wierszy
        #pragma omp for
        for(int i = 0; i < WYMIAR; i++) {
            double suma = 0.0;
            for(int j = 0; j < WYMIAR; j++) {
                suma += a[i][j];
            }
            sumy_w2[i] = suma;
        }
        
        // Bariera niejawna po 'omp for'
        
        // Faza 2: Jeden wątek sumuje tablicę 1D
        #pragma omp single
        {
            printf("Watek %d sumuje tablice 1D:\n", omp_get_thread_num());
            for(int i = 0; i < WYMIAR; i++) {
                suma_finalna += sumy_w2[i];
            }
        }
    }
    
    end = omp_get_wtime();
    
    printf("Suma finalna: %.6f\n", suma_finalna);
    printf("Zgodnosc: %s\n", (suma_finalna - suma_sekw < EPS) ? "TAK" : "NIE");
    printf("Czas: %.6f s\n\n", end - start);
    
    // Praktyczny przykład: Większa tablica z analizą wydajności
    printf("PRZYKLAD PRAKTYCZNY - tablica 10000x1000\n");
    
    int N = 10000;
    double **big = (double**)malloc(N * sizeof(double*));
    for(int i = 0; i < N; i++) {
        big[i] = (double*)malloc(N * sizeof(double));
        for(int j = 0; j < N; j++) {
            big[i][j] = i * 0.01 + j * 0.001;
        }
    }
    double *sumy_big = (double*)malloc(N * sizeof(double));
    
    // Metoda z tablicą 1D
    start = omp_get_wtime();
    
    #pragma omp parallel for default(none) shared(N, big, sumy_big)
    for(int i = 0; i < N; i++) {
        double suma = 0.0;
        for(int j = 0; j < N; j++) {
            suma += big[i][j];
        }
        sumy_big[i] = suma;
    }
    
    double suma_big = 0.0;
    for(int i = 0; i < N; i++) {
        suma_big += sumy_big[i];
    }
    
    double czas_1d = omp_get_wtime() - start;
    
    // Dla porównania: bezpośrednie reduction
    double suma_direct = 0.0;
    start = omp_get_wtime();
    
    #pragma omp parallel for default(none) shared(N, big) reduction(+:suma_direct)
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            suma_direct += big[i][j];
        }
    }
    
    double czas_direct = omp_get_wtime() - start;
    
    printf("Z tablica 1D:       %.6f s, suma = %.2f\n", czas_1d, suma_big);
    printf("Bezposrednie:       %.6f s, suma = %.2f\n", czas_direct, suma_direct);
    printf("\n");
    
    // Zwolnienie pamięci
    for(int i = 0; i < N; i++) free(big[i]);
    free(big);
    free(sumy_big);
    
    // Przykład: znajdowanie wiersza o maksymalnej sumie
    int max_idx = 0;
    double max_suma = sumy_wierszy[0];
    
    #pragma omp parallel for default(none) shared(sumy_wierszy, max_idx, max_suma)
    for(int i = 1; i < WYMIAR; i++) {
        #pragma omp critical
        {
            if(sumy_wierszy[i] > max_suma) {
                max_suma = sumy_wierszy[i];
                max_idx = i;
            }
        }
    }
    
    printf("Wiersz o maksymalnej sumie: %d (suma = %.6f)\n\n", max_idx, max_suma);
    
    return 0;
}
