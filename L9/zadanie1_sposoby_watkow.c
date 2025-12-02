#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

int main() {
    // Sposób 1: Zmienna środowiskowa OMP_NUM_THREADS
    // Należy ustawić przed uruchomieniem programu: export OMP_NUM_THREADS=4
    printf("1. ZMIENNA SRODOWISKOWA OMP_NUM_THREADS:\n");
    printf("   Przed uruchomieniem programu nalezy ustawic:\n");
    printf("   export OMP_NUM_THREADS=4\n");
    #pragma omp parallel default(none)
    {
        #pragma omp master
        {
            printf("   Liczba watkow z zmiennej srodowiskowej: %d\n", omp_get_num_threads());
        }
    }
    printf("\n");
    
    // Sposób 2: Funkcja omp_set_num_threads()
    printf("2. FUNKCJA omp_set_num_threads():\n");
    omp_set_num_threads(6);
    printf("   Wywolano: omp_set_num_threads(6)\n");
    #pragma omp parallel default(none)
    {
        #pragma omp master
        {
            printf("   Liczba watkow ustawiona funkcja: %d\n", omp_get_num_threads());
        }
    }
    printf("\n");
    
    // Sposób 3: Klauzula num_threads()
    printf("3. KLAUZULA num_threads():\n");
    printf("   W dyrektywie: #pragma omp parallel num_threads(8)\n");
    #pragma omp parallel num_threads(8) default(none)
    {
        #pragma omp master
        {
            printf("   Liczba watkow z klauzuli num_threads: %d\n", omp_get_num_threads());
        }
    }
    printf("\n");
    
    // Demonstracja priorytetu
    printf("4. PRIORYTET METOD (najwyzszy -> najnizszy):\n");
    printf("   klauzula num_threads() > omp_set_num_threads() > OMP_NUM_THREADS\n\n");
    
    omp_set_num_threads(3);
    printf("   Ustawiono funkcja: omp_set_num_threads(3)\n");
    
    #pragma omp parallel default(none)
    {
        #pragma omp master
        printf("   Bez klauzuli num_threads - liczba watkow: %d\n", omp_get_num_threads());
    }
    
    #pragma omp parallel num_threads(10) default(none)
    {
        #pragma omp master
        printf("   Z klauzula num_threads(10) - liczba watkow: %d\n", omp_get_num_threads());
    }
    
    return 0;
}
