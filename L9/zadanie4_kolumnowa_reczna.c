#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define WYMIAR 10
#define MAX_THREADS 8
#define EPS 1e-6

int main() {
    double a[WYMIAR][WYMIAR];
    
    printf("Rozmiar tablicy: %d x %d\n", WYMIAR, WYMIAR);
    
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
    
    // METODA 1: Użycie tablicy lokalnych sum wątków
    printf("METODA 1: Tablica sum lokalnych dla kazdego watku\n");

    int num_threads = 4;
    double sumy_lokalne[MAX_THREADS] = {0.0};
    
    double start = omp_get_wtime();
    
    #pragma omp parallel default(none) shared(a, sumy_lokalne, num_threads) num_threads(4)
    {
        int id = omp_get_thread_num();
        double moja_suma = 0.0;
        
        // Każdy wątek przetwarza swoje kolumny
        #pragma omp for
        for(int j = 0; j < WYMIAR; j++) {
            double suma_kolumny = 0.0;
            
            for(int i = 0; i < WYMIAR; i++) {
                suma_kolumny += a[i][j];
            }
            
            moja_suma += suma_kolumny;
            
            printf("Watek %d przetwarza kolumne %d, suma kolumny = %.6f\n", 
                   id, j, suma_kolumny);
        }
        
        // Zapisanie lokalnej sumy wątku (bez synchronizacji, bo każdy wątek ma swoją komórkę)
        sumy_lokalne[id] = moja_suma;
        
        printf("Watek %d: lokalna suma = %.6f\n", id, moja_suma);
    }
    
    // Ręczne zsumowanie wyników wszystkich wątków
    double suma_metoda1 = 0.0;
    for(int i = 0; i < num_threads; i++) {
        suma_metoda1 += sumy_lokalne[i];
    }
    
    double end = omp_get_wtime();
    
    printf("\nWynik koncowy (metoda 1): %.6f\n", suma_metoda1);
    printf("Zgodnosc z sekwencja: %s\n", (suma_metoda1 - suma_sekw < EPS) ? "TAK" : "NIE");
    printf("Czas: %.6f s\n\n", end - start);
    
    // METODA 2: Użycie sekcji krytycznej do bezpiecznej aktualizacji
    printf("METODA 2: Sekcja krytyczna\n");
    
    double suma_metoda2 = 0.0;
    start = omp_get_wtime();
    
    #pragma omp parallel default(none) shared(a, suma_metoda2) num_threads(4)
    {
        int id = omp_get_thread_num();
        double moja_suma = 0.0;
        
        #pragma omp for
        for(int j = 0; j < WYMIAR; j++) {
            for(int i = 0; i < WYMIAR; i++) {
                moja_suma += a[i][j];
            }
        }
        
        // Sekcja krytyczna - tylko jeden wątek może aktualizować sumę globalną
        #pragma omp critical
        {
            printf("Watek %d dodaje swoja sume: %.6f\n", id, moja_suma);
            suma_metoda2 += moja_suma;
        }
    }
    
    end = omp_get_wtime();
    
    printf("\nWynik koncowy (metoda 2): %.6f\n", suma_metoda2);
    printf("Zgodnosc z sekwencja: %s\n", (suma_metoda2 - suma_sekw < EPS) ? "TAK" : "NIE");
    printf("Czas: %.6f s\n\n", end - start);
    
    // METODA 3: Użycie atomic do atomowej aktualizacji
    printf("METODA 3: Operacja atomowa\n");
    
    double suma_metoda3 = 0.0;
    start = omp_get_wtime();
    
    #pragma omp parallel default(none) shared(a, suma_metoda3) num_threads(4)
    {
        int id = omp_get_thread_num();
        double moja_suma = 0.0;
        
        #pragma omp for
        for(int j = 0; j < WYMIAR; j++) {
            for(int i = 0; i < WYMIAR; i++) {
                moja_suma += a[i][j];
            }
        }
        
        // Atomowa aktualizacja - szybsza niż critical dla prostych operacji
        #pragma omp atomic
        suma_metoda3 += moja_suma;
        
        printf("Watek %d: moja suma = %.6f\n", id, moja_suma);
    }
    
    end = omp_get_wtime();
    
    printf("\nWynik koncowy (metoda 3): %.6f\n", suma_metoda3);
    printf("Zgodnosc z sekwencja: %s\n", (suma_metoda3 - suma_sekw < EPS) ? "TAK" : "NIE");
    printf("Czas: %.6f s\n\n", end - start);
    
    // Porównanie wydajności na większej tablicy
    printf("POROWNANIE WYDAJNOSCI - tablica 20000x20000\n");

    int N = 20000;
    double **big_array = (double**)malloc(N * sizeof(double*));
    for(int i = 0; i < N; i++) {
        big_array[i] = (double*)malloc(N * sizeof(double));
        for(int j = 0; j < N; j++) {
            big_array[i][j] = i * 0.01 + j * 0.001;
        }
    }
    
    // Test 1: Tablica lokalnych sum
    double sumy_big[MAX_THREADS] = {0.0};
    start = omp_get_wtime();
    #pragma omp parallel default(none) shared(N, big_array, sumy_big)
    {
        int id = omp_get_thread_num();
        double moja = 0.0;
        #pragma omp for
        for(int j = 0; j < N; j++) {
            for(int i = 0; i < N; i++) {
                moja += big_array[i][j];
            }
        }
        sumy_big[id] = moja;
    }
    double suma_big1 = 0.0;
    for(int i = 0; i < omp_get_max_threads(); i++) suma_big1 += sumy_big[i];
    double czas1 = omp_get_wtime() - start;
    
    // Test 2: Critical
    double suma_big2 = 0.0;
    start = omp_get_wtime();
    #pragma omp parallel default(none) shared(N, big_array, suma_big2)
    {
        double moja = 0.0;
        #pragma omp for
        for(int j = 0; j < N; j++) {
            for(int i = 0; i < N; i++) {
                moja += big_array[i][j];
            }
        }
        #pragma omp critical
        suma_big2 += moja;
    }
    double czas2 = omp_get_wtime() - start;
    
    // Test 3: Atomic
    double suma_big3 = 0.0;
    start = omp_get_wtime();
    #pragma omp parallel default(none) shared(N, big_array, suma_big3)
    {
        double moja = 0.0;
        #pragma omp for
        for(int j = 0; j < N; j++) {
            for(int i = 0; i < N; i++) {
                moja += big_array[i][j];
            }
        }
        #pragma omp atomic
        suma_big3 += moja;
    }
    double czas3 = omp_get_wtime() - start;
    
    // Test 4: Reduction (dla porównania)
    double suma_big4 = 0.0;
    start = omp_get_wtime();
    #pragma omp parallel for default(none) shared(N, big_array) reduction(+:suma_big4)
    for(int j = 0; j < N; j++) {
        for(int i = 0; i < N; i++) {
            suma_big4 += big_array[i][j];
        }
    }
    double czas4 = omp_get_wtime() - start;
    
    printf("Tablica lokalnych sum: %.6f s\n", czas1);
    printf("Critical:              %.6f s\n", czas2);
    printf("Atomic:                %.6f s\n", czas3);
    printf("Reduction:             %.6f s\n", czas4);

    /*
    Porównując:
    Tablica lokalnych sum jest zwykle najszybsza,
    Critical jest najwolniejszy ze względu na blokowanie,
    Atomic jest szybszy niż critical, ale wolniejszy niż lokalne sumy,
    Reduction jest bardzo szybki i prosty w użyciu.
    */
    
    // Zwolnienie pamięci
    for(int i = 0; i < N; i++) {
        free(big_array[i]);
    }
    free(big_array);
    
    return 0;
}
