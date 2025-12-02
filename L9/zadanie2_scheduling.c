#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define WYMIAR 24

int main() {
    double a[WYMIAR];
    
    printf("Rozmiar tablicy: %d elementow\n", WYMIAR);
    printf("Liczba watkow: 4\n\n");
    
    // Inicjalizacja tablicy
    for(int i = 0; i < WYMIAR; i++) {
        a[i] = 1.02 * i;
    }
    
    // 1. STATIC (domyślny podział)
    printf("1. SCHEDULE(STATIC) - domyslny podzial rownomierny:\n");
    printf("   Kazdy watek dostaje rowny blok iteracji\n");
    printf("   Podzial: bloki po %d iteracji\n", WYMIAR/4);
    #pragma omp parallel for ordered schedule(static) default(none) shared(a) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        #pragma omp ordered
        printf("   a[%2d] -> Watek_%d\n", i, id);
    }
    printf("\n");
    
    // 2. STATIC(3)
    printf("2. SCHEDULE(STATIC, 3) - bloki po 3 iteracje:\n");
    printf("   Watki otrzymuja bloki po 3 iteracje w sposob cykliczny\n");
    #pragma omp parallel for ordered schedule(static, 3) default(none) shared(a) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        #pragma omp ordered
        printf("   a[%2d] -> Watek_%d\n", i, id);
    }
    printf("\n");
    
    // 3. DYNAMIC (domyślny chunk = 1)
    printf("3. SCHEDULE(DYNAMIC) - dynamiczny podzial (chunk=1):\n");
    printf("   Watki pobieraja pojedyncze iteracje dynamicznie\n");
    printf("   (wyniki moga sie roznic przy kazdym uruchomieniu)\n");
    #pragma omp parallel for ordered schedule(dynamic) default(none) shared(a) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        #pragma omp ordered
        printf("   a[%2d] -> Watek_%d\n", i, id);
    }
    printf("\n");
    
    // 4. DYNAMIC(2)
    printf("4. SCHEDULE(DYNAMIC, 2) - dynamiczny podzial (chunk=2):\n");
    printf("   Watki pobieraja bloki po 2 iteracje dynamicznie\n");
    #pragma omp parallel for ordered schedule(dynamic, 2) default(none) shared(a) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        #pragma omp ordered
        printf("   a[%2d] -> Watek_%d\n", i, id);
    }
    printf("\n");
    
    // 5. GUIDED
    printf("5. SCHEDULE(GUIDED) - adaptacyjny podzial:\n");
    printf("   Rozmiar blokow maleje w miare postepu (zaczyna od duzych, konczy na malych)\n");
    #pragma omp parallel for ordered schedule(guided) default(none) shared(a) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        #pragma omp ordered
        printf("   a[%2d] -> Watek_%d\n", i, id);
    }
    printf("\n");
    
    // 6. GUIDED(4)
    printf("6. SCHEDULE(GUIDED, 4) - adaptacyjny z minimalnym rozmiarem bloku 4:\n");
    printf("   Jak guided, ale minimalna wielkosc bloku to 4 iteracje\n");
    #pragma omp parallel for ordered schedule(guided, 4) default(none) shared(a) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        #pragma omp ordered
        printf("   a[%2d] -> Watek_%d\n", i, id);
    }
    printf("\n");
    
    // 7. AUTO
    printf("7. SCHEDULE(AUTO) - kompilator decyduje:\n");
    printf("   Kompilator wybiera optymalny podzial\n");
    #pragma omp parallel for ordered schedule(auto) default(none) shared(a) num_threads(4)
    for(int i = 0; i < WYMIAR; i++) {
        int id = omp_get_thread_num();
        #pragma omp ordered
        printf("   a[%2d] -> Watek_%d\n", i, id);
    }
    printf("\n");
    
    // Porównanie wydajności
    printf("8. POROWNANIE WYDAJNOSCI - sumowanie duzej tablicy:\n");
    int n = 200000000;
    long double *big_array = (long double*)malloc(n * sizeof(long double));
    for(int i = 0; i < n; i++) big_array[i] = i * 0.1;
    
    const char* schedules[] = {"static", "static,1000", "dynamic", "dynamic,1000", "guided", "auto"};
    
    for(int sched = 0; sched < 6; sched++) {
        long double suma = 0.0;
        double start = omp_get_wtime();
        
        switch(sched) {
            case 0:
                #pragma omp parallel for schedule(static) default(none) shared(n, big_array) reduction(+:suma)
                for(int i = 0; i < n; i++) suma += big_array[i];
                break;
            case 1:
                #pragma omp parallel for schedule(static, 1000) default(none) shared(n, big_array) reduction(+:suma)
                for(int i = 0; i < n; i++) suma += big_array[i];
                break;
            case 2:
                #pragma omp parallel for schedule(dynamic) default(none) shared(n, big_array) reduction(+:suma)
                for(int i = 0; i < n; i++) suma += big_array[i];
                break;
            case 3:
                #pragma omp parallel for schedule(dynamic, 1000) default(none) shared(n, big_array) reduction(+:suma)
                for(int i = 0; i < n; i++) suma += big_array[i];
                break;
            case 4:
                #pragma omp parallel for schedule(guided) default(none) shared(n, big_array) reduction(+:suma)
                for(int i = 0; i < n; i++) suma += big_array[i];
                break;
            case 5:
                #pragma omp parallel for schedule(auto) default(none) shared(n, big_array) reduction(+:suma)
                for(int i = 0; i < n; i++) suma += big_array[i];
                break;
        }
        
        double end = omp_get_wtime();
        printf("   %-15s: czas = %.6f s\n", schedules[sched], end - start);
    }
    
    free(big_array);
    
    printf("STATIC:  najszybszy, deterministyczny, najlepszy dla rownych obciazen\n");
    printf("DYNAMIC: elastyczny, dla nierownych obciazen, wiekszy narzut\n");
    printf("GUIDED:  kompromis - adaptuje sie do obciazenia, mniejszy narzut niz dynamic\n");
    printf("AUTO:    kompilator optymalizuje\n");
    
    return 0;
}
