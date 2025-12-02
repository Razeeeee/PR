#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define WYMIAR 12
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
    
    // METODA 1: Collapse - spłaszczenie pętli zagnieżdżonych
    printf("METODA 1: COLLAPSE - splaszczenie petli zagniezdonych\n");
    printf("Klauzula collapse(2) traktuje obie petle jako jedna przestrzen iteracji\n");
    printf("Przestrzen iteracji: %d x %d = %d iteracji\n\n", WYMIAR, WYMIAR, WYMIAR*WYMIAR);
    
    double suma_collapse = 0.0;
    double start = omp_get_wtime();
    
    #pragma omp parallel for collapse(2) default(none) shared(a) reduction(+:suma_collapse) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        for(int j = 0; j < WYMIAR; j++) {
            suma_collapse += a[i][j];
            
            // Uwaga: ordered nie działa z collapse, więc usunięto wypisywanie w pętli
        }
    }
    
    double end = omp_get_wtime();
    
    printf("Suma z collapse(2): %.6f\n", suma_collapse);
    printf("Zgodnosc: %s\n", (suma_collapse - suma_sekw < EPS) ? "TAK" : "NIE");
    printf("Czas: %.6f s\n\n", end - start);
    
    // METODA 2: Nested parallelism - równoległość zagnieżdżona
    printf("METODA 2: NESTED PARALLELISM - rownolegle petle zagniezdzone\n");
    printf("Wlaczenie zagniezdzonej rownolegloci: omp_set_nested(1)\n");
    printf("Zewnetrzna petla: poziom 1 rownolegloci\n");
    printf("Wewnetrzna petla: poziom 2 rownolegloci\n\n");
    
    omp_set_nested(1); // Włączenie nested parallelism
    omp_set_max_active_levels(2); // Maksymalnie 2 poziomy
    
    double suma_nested = 0.0;
    start = omp_get_wtime();
    
    #pragma omp parallel num_threads(3) default(none) shared(a, suma_nested)
    {
        #pragma omp for reduction(+:suma_nested)
        for(int i = 0; i < WYMIAR; i++) {
            int outer_id = omp_get_thread_num();
            int outer_level = omp_get_level();
            
            double suma_wiersza = 0.0;
            
            // Zagnieżdżona równoległość na wewnętrznej pętli
            #pragma omp parallel for num_threads(2) default(none) shared(a, i, outer_id, outer_level) reduction(+:suma_wiersza)
            for(int j = 0; j < WYMIAR; j++) {
                int inner_id = omp_get_thread_num();
                int inner_level = omp_get_level();
                
                suma_wiersza += a[i][j];
                
                #pragma omp critical
                {
                    printf("a[%2d][%2d]: Outer(W%d,L%d) -> Inner(W%d,L%d)\n", 
                           i, j, outer_id, outer_level, inner_id, inner_level);
                }
            }
            
            suma_nested += suma_wiersza;
        }
    }
    
    end = omp_get_wtime();
    
    printf("\nSuma z nested parallelism: %.6f\n", suma_nested);
    printf("Zgodnosc: %s\n", (suma_nested - suma_sekw < EPS) ? "TAK" : "NIE");
    printf("Czas: %.6f s\n\n", end - start);
    
    // METODA 3: Ręczny podział 2D - blokowy
    printf("METODA 3: RECZNY PODZIAL 2D - dekompozycja blokowa\n");
    printf("Tablica dzielona na bloki 2D miedzy watki\n\n");
    
    omp_set_nested(0); // Wyłączenie nested
    
    double suma_blokowa = 0.0;
    int blok_size = WYMIAR / 2; // Podział 2x2 bloków
    
    start = omp_get_wtime();
    
    #pragma omp parallel default(none) shared(a, blok_size) reduction(+:suma_blokowa) num_threads(4)
    {
        int id = omp_get_thread_num();
        
        // Obliczenie zakresu bloku dla tego wątku
        int bloki_per_dim = WYMIAR / blok_size;
        int blok_id = id % (bloki_per_dim * bloki_per_dim);
        int blok_i = (blok_id / bloki_per_dim) * blok_size;
        int blok_j = (blok_id % bloki_per_dim) * blok_size;
        
        double suma_lokalna = 0.0;
        
        if(id < bloki_per_dim * bloki_per_dim) {
            printf("Watek %d przetwarza blok [%d-%d][%d-%d]\n", 
                   id, blok_i, blok_i+blok_size-1, blok_j, blok_j+blok_size-1);
            
            for(int i = blok_i; i < blok_i + blok_size && i < WYMIAR; i++) {
                for(int j = blok_j; j < blok_j + blok_size && j < WYMIAR; j++) {
                    suma_lokalna += a[i][j];
                }
            }
        }
        
        suma_blokowa += suma_lokalna;
    }
    
    end = omp_get_wtime();
    
    printf("\nSuma z podzialu blokowego: %.6f\n", suma_blokowa);
    printf("Zgodnosc: %s\n", (suma_blokowa - suma_sekw < EPS) ? "TAK" : "NIE");
    printf("Czas: %.6f s\n\n", end - start);
    
    // Porównanie wydajności różnych metod
    printf("POROWNANIE WYDAJNOSCI - tablica 10000x10000\n");

    int N = 10000;
    double **big = (double**)malloc(N * sizeof(double*));
    for(int i = 0; i < N; i++) {
        big[i] = (double*)malloc(N * sizeof(double));
        for(int j = 0; j < N; j++) {
            big[i][j] = i * 0.01 + j * 0.001;
        }
    }
    
    // Test 1: Pojedyncza pętla (dekompozycja wierszowa)
    double s1 = 0.0;
    start = omp_get_wtime();
    #pragma omp parallel for default(none) shared(N, big) reduction(+:s1)
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            s1 += big[i][j];
        }
    }
    double t1 = omp_get_wtime() - start;
    
    // Test 2: Collapse
    double s2 = 0.0;
    start = omp_get_wtime();
    #pragma omp parallel for collapse(2) default(none) shared(N, big) reduction(+:s2)
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            s2 += big[i][j];
        }
    }
    double t2 = omp_get_wtime() - start;
    
    // Test 3: Nested (może być wolne!)
    omp_set_nested(1);
    double s3 = 0.0;
    start = omp_get_wtime();
    #pragma omp parallel num_threads(4) default(none) shared(N, big, s3)
    {
        #pragma omp for reduction(+:s3)
        for(int i = 0; i < N; i++) {
            double sr = 0.0;
            #pragma omp parallel for num_threads(2) default(none) shared(big, i, N) reduction(+:sr)
            for(int j = 0; j < N; j++) {
                sr += big[i][j];
            }
            s3 += sr;
        }
    }
    double t3 = omp_get_wtime() - start;
    omp_set_nested(0);
    
    printf("Dekompozycja wierszowa:  %.6f s\n", t1);
    printf("Collapse(2):             %.6f s\n", t2);
    printf("Nested parallelism:      %.6f s\n", t3);
    
    for(int i = 0; i < N; i++) free(big[i]);
    free(big);

    /*
    Wnioski:
    - Dekompozycja wierszowa jest najprostsza i najszybsza przy rownym obciazeniu
    - Collapse oferuje dobry kompromis miedzy prostota a wydajnoscia
    - Nested parallelism jest najbardziej elastyczny, ale ma duzy narzut i jest wolniejszy
    */
    
    return 0;
}
